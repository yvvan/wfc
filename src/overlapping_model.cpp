#include <wfc/overlapping_model.h>
#include <wfc/overlapping_pattern_extraction.h>
#include <wfc/ranges.h>

#include <algorithm>
#include <cmath>

RGBA collapsePixel(const std::vector<ColorIndex> &tile_contributors,
                   const Palette &palette) {
  if (tile_contributors.empty()) {
    // No contributors, so set to 0
    return {0, 0, 0, 255};
  } else if (tile_contributors.size() == 1) {
    // One contributor, so use that
    return palette[tile_contributors[0]];
  } else {
    // Multiple contributors, so average them
    return averageContributors(tile_contributors, palette);
  }
}

RGBA averageContributors(const std::vector<ColorIndex> &contributors,
                         const Palette &palette) {
  size_t r = 0;
  size_t g = 0;
  size_t b = 0;
  size_t a = 0;
  for (const auto tile : contributors) {
    r += palette[tile].r;
    g += palette[tile].g;
    b += palette[tile].b;
    a += palette[tile].a;
  }
  r /= contributors.size();
  g /= contributors.size();
  b /= contributors.size();
  a /= contributors.size();
  return {(uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a};
}

Image image_from_graphics(const Graphics &graphics, const Palette &palette) {
  Image result(graphics.size(), {0, 0, 0, 0});

  auto rangeFcn = [&](const Index2D &index) {
    const auto &tile_contributors = graphics[index];
    result[index] = collapsePixel(tile_contributors, palette);
  };

  runForDimension(graphics.size(), rangeFcn);

  return result;
}

bool agrees(const Pattern &p1, const Pattern &p2, int dx, int dy, int n) {
  int xmin, xmax, ymin, ymax;

  if (dx < 0) {
    xmin = 0;
    xmax = dx + n; // 1 to n-1
  } else {
    // dx: 0 -> 0 to n
    // Otherwise: (1 - n-1) to n
    xmin = dx; // 0 to n-1
    xmax = n;  // n
  }

  if (dy < 0) {
    ymin = 0;
    ymax = dy + n;
  } else {
    ymin = dy;
    ymax = n;
  }

  for (size_t y = ymin; y < ymax; ++y) {
    for (size_t x = xmin; x < xmax; ++x) {
      if (p1[{x, y}] != p2[{(x - dx), (y - dy)}]) {
        return false;
      }
    }
  }
  return true;
}

OverlappingModel::OverlappingModel(const OverlappingComputedInfo &config)
    : mInternal(config.internal) {
  mCommonParams = config.commonParams;
}

OverlappingComputedInfo fromConfig(const OverlappingModelConfig &config) {
  OverlappingComputedInfo toReturn;

  toReturn.commonParams.mOutputProperties = config.outputProperties;

  toReturn.internal._n = config.n;
  toReturn.internal._palette = config.sample_image.palette;

  PatternInfo patternInfo =
      calculatePatternInfo(config.sample_image, config.hasfoundation,
                           config.periodic_in, config.symmetry, config.n);

  std::vector<double> extractedWeights;
  extractedWeights.reserve(patternInfo.patterns.size());
  for (const auto &pattern : patternInfo.patterns) {
    extractedWeights.push_back(pattern.weight);
  }
  toReturn.commonParams.patternWeights = extractedWeights;

  std::vector<Pattern> extractedPatterns;
  extractedPatterns.reserve(patternInfo.patterns.size());
  for (const auto &pattern : patternInfo.patterns) {
    extractedPatterns.push_back(pattern.pattern);
  }

  toReturn.internal.foundation = patternInfo.foundation;
  toReturn.internal._patterns = extractedPatterns;

  toReturn.commonParams.numPatterns = toReturn.internal._patterns.size();
//  LOG_F(INFO, "Found %lu unique patterns in sample image",
//        toReturn.commonParams.numPatterns);

  toReturn.internal._propagator = createPropagator(
      toReturn.commonParams.numPatterns, config.n, toReturn.internal._patterns);

  PropagatorStatistics statistics = analyze(toReturn.internal._propagator);
//  LOG_F(INFO, "propagator length: mean/max/sum: %.1f, %lu, %lu",
//        statistics.average, statistics.longest_propagator,
//        statistics.sum_propagator);
  return toReturn;
}

Propagator createPropagator(size_t numPatterns, size_t n,
                            const std::vector<Pattern> &patterns) {
  Dimension3D propagatorSize{numPatterns, 2 * n - 1, 2 * n - 1};
  Propagator toReturn(propagatorSize, {});

  for (size_t t = 0; t < propagatorSize.width; ++t) {
    for (size_t x = 0; x < propagatorSize.height; ++x) {
      for (size_t y = 0; y < propagatorSize.depth; ++y) {
        Index3D index3D{t, x, y};

        auto &list = toReturn[index3D];
        for (int t2 = 0; t2 < numPatterns; ++t2) {
          if (agrees(patterns[t], patterns[t2], x - n + 1, y - n + 1, n)) {
            list.push_back(t2);
          }
        }
      }
    }
  }
  return toReturn;
}

PropagatorStatistics analyze(const Propagator &propagator) {
  // These are just used for printouts
  PropagatorStatistics statistics = {};

  Dimension3D dimensions = propagator.size();

  for (size_t t = 0; t < dimensions.width; ++t) {
    for (size_t x = 0; x < dimensions.height; ++x) {
      for (size_t y = 0; y < dimensions.depth; ++y) {
        Index3D index3D{t, x, y};

        auto &list = propagator[index3D];
        statistics.longest_propagator =
            std::max(statistics.longest_propagator, list.size());
        statistics.sum_propagator += list.size();
      }
    }
  }

  statistics.average = (double)statistics.sum_propagator / propagator.volume();

  return statistics;
}

bool OverlappingModel::propagate(AlgorithmData &algorithmData) const {
  bool did_change = false;
  Dimension2D dimension = mCommonParams.mOutputProperties.dimensions;

  // This whole set of nested loops looks very similar to the one in graphics()
  auto rangeFcn = [&](const Index2D &index) {
    if (!algorithmData._changes[index]) {
      return;
    }

    algorithmData._changes[index] = false;

    int rangeLimit = mInternal._n - 1;

    SquareRange range{{-rangeLimit, -rangeLimit}, {rangeLimit, rangeLimit}};

    auto rangeIterator = range2D(range);

    auto rangeFcn = [&](const Offset2D &offset) {
      int sx = static_cast<int>(index.x) + static_cast<int>(offset.x);
      int sy = static_cast<int>(index.y) + static_cast<int>(offset.y);

      // Do wrap around (always-positive modulus)
      if (sx < 0) {
        sx += dimension.width;
      } else if (sx >= dimension.width) {
        sx -= dimension.width;
      }

      if (sy < 0) {
        sy += dimension.height;
      } else if (sy >= dimension.height) {
        sy -= dimension.height;
      }

      Index2D sIndex{static_cast<size_t>(sx), static_cast<size_t>(sy)};

      if (on_boundary(sIndex)) {
        return;
      }

      for (size_t t2 = 0; t2 < mCommonParams.numPatterns; ++t2) {
        Index3D sPatternIndex = append(sIndex, t2);
        if (!algorithmData._wave[sPatternIndex]) {
          continue;
        }

        // This part below seems to be the only thing fundamentally diff from
        // graphics() algorithm:
        bool can_pattern_fit = false;

        Index3D shiftedIndex{t2, static_cast<size_t>(rangeLimit - offset.x), static_cast<size_t>(rangeLimit - offset.y)};
        const auto &prop = mInternal._propagator[shiftedIndex];
        for (const auto &t3 : prop) {
          if (algorithmData._wave[append(index, t3)]) {
            can_pattern_fit = true;
            break;
          }
        }

        if (!can_pattern_fit) {
          algorithmData._changes[sIndex] = true;
          algorithmData._wave[sPatternIndex] = false;
          did_change = true;
        }
      }
    };

    rangeIterator(rangeFcn);
  };

  runForDimension(dimension, rangeFcn);

  return did_change;
}

Graphics OverlappingModel::graphics(const AlgorithmData &algorithmData) const {
  Dimension2D dimension = mCommonParams.mOutputProperties.dimensions;

  Graphics result(dimension, {});

  auto rangeFcn = [&](const Index2D &index) {
    auto &tile_contributors = result[index];

    for (int dy = 0; dy < mInternal._n; ++dy) {
      for (int dx = 0; dx < mInternal._n; ++dx) {
        int sx = index.x - dx;
        if (sx < 0)
          sx += dimension.width;

        int sy = index.y - dy;
        if (sy < 0)
          sy += dimension.height;

        if (on_boundary({static_cast<size_t>(sx), static_cast<size_t>(sy)})) {
          continue;
        }

        for (size_t t = 0; t < mCommonParams.numPatterns; ++t) {
          Index3D index3D{static_cast<size_t>(sx), static_cast<size_t>(sy), t};
          if (algorithmData._wave[index3D]) {
            tile_contributors.push_back(mInternal._patterns[t][{static_cast<size_t>(dx), static_cast<size_t>(dy)}]);
          }
        }
      }
    }
  };

  runForDimension(dimension, rangeFcn);

  return result;
}

std::unique_ptr<Image> OverlappingModel::image(const AlgorithmData &algorithmData) const {
  return upsample(
      image_from_graphics(graphics(algorithmData), mInternal._palette));
}

std::unique_ptr<Image> upsample(const Image &image) {
  auto result =
      std::make_unique<Image>(Dimension2D{image.size().width * kUpscale,
                                          image.size().height * kUpscale},
                              RGBA{});

  auto rangeFcn = [&](const Index2D &index) {
    Index2D scaled{index.x / kUpscale, index.y / kUpscale};

    (*result)[index] = image[scaled];
  };

  runForDimension(result->size(), rangeFcn);

  return result;
}

AlgorithmData OverlappingModel::initAlgorithmData() const {
  AlgorithmData algorithmData = initialOutput(
      mCommonParams.mOutputProperties.dimensions, mCommonParams.numPatterns);
  if (mInternal.foundation) {
    // Tile has a clearly-defined "ground"/"foundation"
    modifyOutputForFoundation(mCommonParams, *this, mInternal.foundation,
                              algorithmData);
  }

  return algorithmData;
}

void modifyOutputForFoundation(const CommonParams &commonParams,
                               const Model &model, size_t foundation,
                               AlgorithmData &algorithmData) {
  Dimension2D dimension = algorithmData._changes.size();
  for (size_t x = 0; x < dimension.width; ++x) {
    // Setting the foundation section of the algorithmData wave only true for
    // foundation
    for (size_t t = 0; t < commonParams.numPatterns; ++t) {
      if (t != foundation) {
        Index3D index{x, dimension.height - 1, t};
        algorithmData._wave[index] = false;
      }
    }

    // Setting the rest of the algorithmData wave only true for not foundation
    for (size_t y = 0; y < dimension.height - 1; ++y) {
      Index3D index{x, y, foundation};
      algorithmData._wave[index] = false;
    }

    for (size_t y = 0; y < dimension.height; ++y) {
      Index2D index{x, y};
      algorithmData._changes[index] = true;
    }
  }

  while (model.propagate(algorithmData))
    ;
}
