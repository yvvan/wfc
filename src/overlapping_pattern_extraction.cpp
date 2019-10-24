#include <wfc/overlapping_pattern_extraction.h>

#include <array>
#include <cmath>
#include <iostream>

#include <wfc/ranges.h>

std::array<int, 4> shiftVector = {{0, 0, 1, 1}};

// Sin wave discretized at 90 degrees
std::array<int, 4> sinVector = {{0, 1, 0, -1}};

int sinN(int n) { return sinVector[n % 4]; }

int cosN(int n) { return sinN(n + 1); }

int sovN(int n) { return shiftVector[n % 4]; }

int covN(int n) { return sovN(n + 1); }

Pattern transformPattern(const Pattern &p,
                         const PatternTransformProperties &transform) {
  Dimension2D dimension = p.size();

  int n = dimension.width;

  Pattern result(dimension);

  int reflectFactor;
  int shiftDist;
  if (!transform.reflected) {
    reflectFactor = 1;
    shiftDist = transform.rotations;
  } else {
    reflectFactor = -1;
    shiftDist = (transform.rotations + 1);
  }

  int cR = cosN(transform.rotations);
  int sR = sinN(transform.rotations);
  int xShift = (n - 1) * covN(shiftDist);
  int yShift = (n - 1) * sovN(shiftDist);

  // Equivalent to:
  //         Rot             Reflect
  // | c -s (n * xShift) | | -1 0 n  |
  // | s  c (n * yShift) | |  0 1 0  |
  // | 0  0      1       | |  0 0 1  |
  //
  // Where yShift = { 0, 0, 1, 1 }
  // and xShift = yShift(k + 1)
  Array2D<int> matrix = {{reflectFactor * cR, -sR, xShift},
                         {reflectFactor * sR, cR, yShift}};

  auto rangeFcn = [&](const Index2D &index) {
    Index2D destination = {
        (matrix[{0, 0}] * index.x + matrix[{1, 0}] * index.y + matrix[{2, 0}]),
        (matrix[{0, 1}] * index.x + matrix[{1, 1}] * index.y + matrix[{2, 1}]),
    };
    result[destination] = p[index];
  };

  runForDimension(dimension, rangeFcn);

  return result;
}

template <class Functor> Pattern make_pattern(size_t n, Functor fun) {
  Dimension2D dimension{n, n};

  Pattern result(dimension);

  auto rangeFcn = [&](const Index2D &index) { result[index] = fun(index); };

  runForDimension(dimension, rangeFcn);

  return result;
}

PatternInfo calculatePatternInfo(const PalettedImage &image, bool hasFoundation,
                                 bool periodicIn, size_t symmetry, int n) {
  PatternInfo toReturn = {};

  PatternHash foundation = kInvalidHash;
  PatternHash *foundationPtr = (hasFoundation) ? &foundation : nullptr;
  const auto hashed_patterns =
      extract_patterns(image, n, periodicIn, symmetry, foundationPtr);

  for (const auto &it : hashed_patterns) {
    if (it.first.hash == foundation) {
      // size() = the current index. This should be more explicit.
      // This is also a really roundabout way of setting the foundation
      toReturn.foundation = toReturn.patterns.size();
    }

    WeightedPattern newItem{it.first.pattern, static_cast<double>(it.second)};
    toReturn.patterns.push_back(newItem);
  }
  return toReturn;
}

// n = side of the pattern, e.g. 3.
PatternPrevalence extract_patterns(const PalettedImage &sample, int n,
                                   bool periodic_in, size_t symmetry,
                                   PatternHash *out_lowest_pattern) {
  Dimension2D imageDimension = sample.data.size();

  std::unordered_map<HashedPattern, size_t, PatternHasher> newPatterns;

  PatternPrevalence patterns;

  Dimension2D dimension;
  if (periodic_in) {
    dimension = imageDimension;
  } else {
    dimension = {imageDimension.width - n + 1, imageDimension.height - n + 1};
  }

  auto rangeFcn = [&](const Index2D &index) {
    Array2D<Pattern> ps = generatePatterns(sample, n, index);
    for (size_t k = 0; k < symmetry; ++k) {
      Index2D transformIndex = {k / 2, (k % 2) == 1};
      HashedPattern hashedPattern{
          ps[transformIndex],
          hash_from_pattern(ps[transformIndex], sample.palette.size())};
      patterns[hashedPattern] += 1;

      if (out_lowest_pattern && index.y == imageDimension.height - 1) {
        *out_lowest_pattern = hashedPattern.hash;
      }
    }
  };

  runForDimension(dimension, rangeFcn);

  return patterns;
}

// A map from hashed pattern to the index in vector
using PatternMap = std::unordered_map<HashedPattern, size_t, PatternHasher>;

PatternTransformProperties convertTransformProperties(int enumerated) {
  return {enumerated / 2, ((enumerated % 2) == 1)};
};

auto runOverPatterns(const PalettedImage &sample, int n, const Index2D &index) {
  return [&sample, n, index](auto consumerFcn) {
    auto currentPattern = patternFromSample(sample, n, index);

    for (size_t i = 0; i < 4; ++i) {
      // size_t baseIndex = 2 * i;

      EnumeratedPattern toConsume{currentPattern, {i, 0}};
      if (consumerFcn(toConsume)) {
        return true;
      }

      toConsume = {reflect(currentPattern, n), {i, 1}};
      if (consumerFcn(toConsume)) {
        return true;
      }

      currentPattern = rotate(currentPattern, n);
    }
  };
}

ImagePatternProperties extractPatternsFromImage(const PalettedImage &sample,
                                                int n) {
  ImagePatternProperties toReturn;
  toReturn.grid = Array2D<PatternIdentifier>(sample.data.size());

  PatternMap patternMap;

  // int count = 0;

  auto rangeFcn = [&](const Index2D &index) {
    Array2D<Pattern> ps = generatePatterns(sample, n, index);

    auto hashedValue = patternMap.end();
    Index2D transformEnumeration = {0, 0};

    auto consumerFcn = [&](const Index2D &index) {
      const auto &pattern = ps[index];
      HashedPattern hashedPattern{
          pattern, hash_from_pattern(pattern, sample.palette.size())};

      hashedValue = patternMap.find(hashedPattern);
      transformEnumeration = index;

      return hashedValue != patternMap.end();
    };

    Dimension2D transformDimensions = ps.size();
    BreakRange::runForDimension(transformDimensions, consumerFcn);

    if (hashedValue != patternMap.end()) {
      PatternIdentifier identifier{hashedValue->second, transformEnumeration};
      toReturn.grid[index] = identifier;

      toReturn.patterns[hashedValue->second].occurrence[transformEnumeration]++;
    } else {
      toReturn.patterns.push_back({ps[{0, 0}], Array2D<int>({4, 2}, 0)});
      toReturn.patterns.back().occurrence[{0, 0}] = 1;

      PatternIdentifier identifier{toReturn.patterns.size() - 1, {0, 0}};
      toReturn.grid[index] = identifier;

      HashedPattern hashedPattern{
          ps[{0, 0}], hash_from_pattern(ps[{0, 0}], sample.palette.size())};
      patternMap[hashedPattern] = identifier.patternIndex;
    }
  };

  Dimension2D imageDimension = sample.data.size();
  runForDimension(imageDimension, rangeFcn);

  return toReturn;
}

PatternHash hash_from_pattern(const Pattern &pattern, size_t palette_size) {
  PatternHash result = 0;
  size_t power = 1;

  Dimension2D dim = pattern.size();
  for (int y = dim.height - 1; y >= 0; --y) {
    for (int x = dim.width - 1; x >= 0; --x) {
      result += pattern[{static_cast<size_t>(x), static_cast<size_t>(y)}] * power;
      power *= palette_size;
    }
  }
  return result;
}

Array2D<Pattern> generatePatterns(const PalettedImage &sample, int n,
                                  const Index2D &index) {
  Array2D<Pattern> toReturn({4, 2});
  auto consumerFcn = [&toReturn](const EnumeratedPattern &enumeratedPattern) {
    toReturn[enumeratedPattern.enumeratedTransform] = enumeratedPattern.pattern;
    return false;
  };

  auto iterable = runOverPatterns(sample, n, index);

  iterable(consumerFcn);

  return toReturn;
}

Pattern patternFromSample(const PalettedImage &sample, int n,
                          const Index2D &imageIndex) {
  auto functor = [&](const Index2D &patternIndex) {
    Index2D shiftedIndex = patternIndex + imageIndex;
    Dimension2D dimension = sample.data.size();
    auto index = wrapAroundIndex(shiftedIndex, dimension);
    return sample.data[index];
  };
  return make_pattern(n, functor);
}

Pattern rotate(const Pattern &p, int n) {
  auto functor = [&](const Index2D &patternIndex) {
    return p[{patternIndex.y, (n - 1) - patternIndex.x}];
  };
  return make_pattern(n, functor);
}

Pattern reflect(const Pattern &p, int n) {
  auto functor = [&](const Index2D &patternIndex) {
    return p[{(n - 1) - patternIndex.x, patternIndex.y}];
  };
  return make_pattern(n, functor);
}

Index2D wrapAroundIndex(const Index2D &index, const Dimension2D &dimension) {
  return {(index.x % dimension.width), (index.y % dimension.height)};
}

PatternTransformProperties
denumerateTransformProperties(Index2D enumeratedTransform) {
  return {static_cast<int>(enumeratedTransform.x),
          static_cast<bool>(enumeratedTransform.y)};
}

Index2D enumerateTransformProperties(
    const PatternTransformProperties &transformProperties) {
  return {static_cast<size_t>(transformProperties.rotations),
          static_cast<size_t>(transformProperties.reflected ? 1 : 0)};
}

Pattern createPattern(const Pattern &base,
                      const PatternTransformProperties &transformProperties) {
  Pattern toReturn = base;

  int n = base.size().width;

  for (int i = 0; i < transformProperties.rotations; i++) {
    toReturn = rotate(toReturn, n);
  }

  if (transformProperties.reflected) {
    toReturn = reflect(toReturn, n);
  }

  return toReturn;
}
