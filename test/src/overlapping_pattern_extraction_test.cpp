#include <gtest/gtest.h>

#include <wfc/ranges.h>

#include <wfc/overlapping_pattern_extraction.h>
#include <wfc/overlapping_types.h>
#include <wfc/pattern_properties_comparison.h>

// ========================================================

struct TransformOccurrence {

  PatternTransformProperties transform;

  int occurrence;
};

Array2D<int> convert(std::initializer_list<TransformOccurrence> toConvert) {
  Array2D<int> toReturn({4, 2});
  for (const auto &transformOccurence : toConvert) {
    Index2D index = enumerateTransformProperties(transformOccurence.transform);
    toReturn[index] = transformOccurence.occurrence;
  }
  return toReturn;
}

// ========================================================

constexpr RGBA white = {255, 255, 255};
constexpr RGBA black = {0, 0, 0};

// Makes a black/white checkerboard pattern of the desired size where the bottom
// left corner is black.
PalettedImage checkerBoard(size_t width) {
  Dimension2D dimension{width, width};

  Array2D<ColorIndex> grid(dimension);

  auto functor = [&](const Index2D &index) {
    bool gridVal = ((index.x + index.y) % 2);
    grid[index] = gridVal ? 1 : 0;
    return false;
  };

  runForDimension(dimension, functor);

  return {grid, {white, black}};
}

ImagePatternProperties expectedEvenCheckerboardProperties(int sizeFactor) {
  const size_t size = sizeFactor * 2;

  Pattern pattern{{0, 1}, {1, 0}};

  Dimension2D dimension{size, size};
  Array2D<PatternIdentifier> grid(dimension);

  PatternTransformProperties normalTransform = {0, false};
  PatternTransformProperties reflectedTransform = {0, true};

  Index2D normalEnumeratedTransform =
      enumerateTransformProperties(normalTransform);
  Index2D reflectedEnumeratedTransform =
      enumerateTransformProperties(reflectedTransform);

  auto fillGrid = [&](const Index2D &index) {
    PatternIdentifier indexIdentifier;

    // Only one pattern for even checkerboards, so all elements will have this
    // index
    indexIdentifier.patternIndex = 0;
    if ((index.x + index.y) % 2) {
      indexIdentifier.enumeratedTransform = reflectedEnumeratedTransform;
    } else {
      indexIdentifier.enumeratedTransform = normalEnumeratedTransform;
    }

    grid[index] = indexIdentifier;
  };

  runForDimension(dimension, fillGrid);

  int gridArea = size * size;
  return ImagePatternProperties{
      {{pattern,
        convert({{{0, false}, gridArea / 2}, {{0, true}, gridArea / 2}})}},
      grid};
}

void evenCheckerboardTest(int sizeFactor) {
  const size_t size = sizeFactor * 2;
  const int n = 2;

  auto sample = checkerBoard(size);
  ImagePatternProperties expectedProperties =
      expectedEvenCheckerboardProperties(sizeFactor);

  ImagePatternProperties properties = extractPatternsFromImage(sample, n);

  ASSERT_TRUE(imagePropertiesEquivalent(properties, expectedProperties));
  ASSERT_TRUE(imageGridEquivalent(properties, expectedProperties));
}

TEST(OverlappingExtractionTest, test1) {
  for (int i = 1; i <= 6; i++) {
    evenCheckerboardTest(i);
  }
}

ImagePatternProperties expectedOddCheckerboardProperties(int sizeFactor) {
  const size_t size = (sizeFactor * 2) - 1;

  Pattern pattern{{0, 1}, {1, 0}};

  Pattern sidePattern{{1, 0}, {1, 0}};

  Pattern topPattern{{0, 0}, {0, 0}};

  Dimension2D dimension{size, size};
  Array2D<PatternIdentifier> grid(dimension);

  PatternTransformProperties normalTransform = {0, false};
  PatternTransformProperties reflectedTransform = {0, true};

  Index2D normalEnumeratedTransform =
      enumerateTransformProperties(normalTransform);
  Index2D reflectedEnumeratedTransform =
      enumerateTransformProperties(reflectedTransform);

  auto fillGrid = [&](const Index2D &index) {
    PatternIdentifier indexIdentifier;

    // Only one pattern for even checkerboards, so all elements will have this
    // index
    indexIdentifier.patternIndex = 0;
    if ((index.x + index.y) % 2) {
      indexIdentifier.enumeratedTransform = reflectedEnumeratedTransform;
    } else {
      indexIdentifier.enumeratedTransform = normalEnumeratedTransform;
    }

    grid[index] = indexIdentifier;
  };

  Dimension2D reducedDimension{size - 1, size - 1};
  runForDimension(reducedDimension, fillGrid);

  for (size_t x = 0; x < size - 1; ++x) {
    const size_t y = size - 1;
    PatternIdentifier indexIdentifier;

    indexIdentifier.patternIndex = 1;
    if (x % 2) {
      indexIdentifier.enumeratedTransform = reflectedEnumeratedTransform;
    } else {
      indexIdentifier.enumeratedTransform = normalEnumeratedTransform;
    }
    grid[{x, y}] = indexIdentifier;
  }

  PatternTransformProperties rotatedNormalTransform = {1, false};
  PatternTransformProperties rotatedReflectedTransform = {3, false};

  Index2D rotatedNormalEnumeratedTransform =
      enumerateTransformProperties(rotatedNormalTransform);
  Index2D rotatedReflectedEnumeratedTransform =
      enumerateTransformProperties(rotatedReflectedTransform);

  for (size_t y = 0; y < size - 1; ++y) {
    const size_t x = size - 1;
    PatternIdentifier indexIdentifier;

    indexIdentifier.patternIndex = 1;
    if (y % 2) {
      indexIdentifier.enumeratedTransform = rotatedNormalEnumeratedTransform;
    } else {
      indexIdentifier.enumeratedTransform = rotatedReflectedEnumeratedTransform;
    }
    grid[{x, y}] = indexIdentifier;
  }

  grid[{size - 1, size - 1}] = {2, {0, 0}};

  int subCheckerboardArea = (size - 1) * (size - 1);

  int subCheckerboardOccurrence = subCheckerboardArea / 2;
  int sideSubOccurrences = static_cast<int>((size - 1) / 2);

  return ImagePatternProperties{
      {{pattern, convert({{{0, false}, subCheckerboardOccurrence},
                          {{0, true}, subCheckerboardOccurrence}})},
       {sidePattern, convert({{{0, false}, sideSubOccurrences},
                              {{1, false}, sideSubOccurrences},
                              {{2, false}, sideSubOccurrences},
                              {{3, false}, sideSubOccurrences}})},
       {topPattern, convert({{{0, false}, 1}})}},
      grid};
}

void oddCheckerboardTest(int sizeFactor) {
  const size_t size = (sizeFactor * 2) - 1;
  const int n = 2;

  auto sample = checkerBoard(size);
  ImagePatternProperties expectedProperties =
      expectedOddCheckerboardProperties(sizeFactor);

  ImagePatternProperties properties = extractPatternsFromImage(sample, n);

  ASSERT_TRUE(imagePropertiesEquivalent(properties, expectedProperties));
  ASSERT_TRUE(imageGridEquivalent(properties, expectedProperties));
}

TEST(OverlappingExtractionTest, test2) {
  for (int i = 2; i <= 6; i++) {
    oddCheckerboardTest(i);
  }
}

// ========================================================

TEST(TransformTest, test1) {
  // Pattern chosen for not having any symmetry
  Pattern input = {{1, 2, 0}, {0, 0, 0}, {0, 0, 0}};

  Array2D<Pattern> expectedPatterns({4, 2});
  expectedPatterns[{0, 0}] = {{1, 2, 0}, {0, 0, 0}, {0, 0, 0}};

  expectedPatterns[{1, 0}] = {{0, 0, 1}, {0, 0, 2}, {0, 0, 0}};

  expectedPatterns[{2, 0}] = {{0, 0, 0}, {0, 0, 0}, {0, 2, 1}};

  expectedPatterns[{3, 0}] = {{0, 0, 0}, {2, 0, 0}, {1, 0, 0}};

  expectedPatterns[{0, 1}] = {{0, 2, 1}, {0, 0, 0}, {0, 0, 0}};

  expectedPatterns[{1, 1}] = {{0, 0, 0}, {0, 0, 2}, {0, 0, 1}};

  expectedPatterns[{2, 1}] = {{0, 0, 0}, {0, 0, 0}, {1, 2, 0}};

  expectedPatterns[{3, 1}] = {{1, 0, 0}, {2, 0, 0}, {0, 0, 0}};

  auto consumerFcn = [&](const Index2D &index) {
    Pattern result =
        transformPattern(input, denumerateTransformProperties(index));
    ASSERT_EQ(result, expectedPatterns[index]);
  };

  runForDimension({4, 2}, consumerFcn);
}
