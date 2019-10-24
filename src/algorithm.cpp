#include <wfc/algorithm.h>

#include <wfc/overlapping_model.h>
#include <wfc/ranges.h>
#include <wfc/tile_model.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>

const char *result2str(const Result result) {
  return result == Result::kSuccess
             ? "success"
             : result == Result::kFail ? "fail" : "unfinished";
}

double calc_sum(const std::vector<double> &a) {
  return std::accumulate(a.begin(), a.end(), 0.0);
}

// Pick a random index weighted by a
size_t weightedIndexSelect(const std::vector<double> &a, double randFraction) {
  double sum = calc_sum(a);

  if (sum == 0.0) {
    // ???
    return std::floor(randFraction * a.size());
  }

  double between_zero_and_sum = randFraction * sum;

  double accumulated = 0;

  for (int i = 0; i < a.size(); ++i) {
    accumulated += a[i];
    if (between_zero_and_sum <= accumulated) {
      return i;
    }
  }

  return 0;
}

EntropyValue calculateEntropy(const Array3D<Bool> &wave, const Index2D &index2D,
                              size_t numPatterns,
                              const std::vector<double> &patternWeights) {
  EntropyValue entropyResult = {0, 0};

  for (int t = 0; t < numPatterns; ++t) {
    Index3D index = append(index2D, t);
    if (wave[index]) {
      entropyResult.num_superimposed += 1;
      entropyResult.entropy += patternWeights[t];
    }
  }
  return entropyResult;
}

EntropyResult find_lowest_entropy(const CommonParams &commonParams,
                                  const Model &model,
                                  const Array3D<Bool> &wave) {
  // We actually calculate exp(entropy), i.e. the sum of the weights of the
  // possible patterns

  double min = std::numeric_limits<double>::infinity();

  // TODO: This is almost always (0, 0) for the initial iteration. Perhaps an
  // explicit seeding should be used? Note: it is not (0, 0) when the pattern
  // has foundation, as this modifies the wave, resulting in a value besides (0,
  // 0)
  Index2D minIndex;

  bool fail = false;

  auto func = [&](auto index2D) {
    if (model.on_boundary(index2D)) {
      return false;
    }

    EntropyValue entropyResult = calculateEntropy(
        wave, index2D, commonParams.numPatterns, commonParams.patternWeights);

    if (entropyResult.entropy == 0 || entropyResult.num_superimposed == 0) {
      fail = true;
      return true;
    }

    if (entropyResult.num_superimposed == 1) {
      // Cell pattern is finalized
      return false;
    }

    // TODO: Add this back in, or remove?
    /*
    // Add a tie-breaking bias:
    const double noise = 0.5 * random_double();
    entropy += noise;
    */

    if (entropyResult.entropy < min) {
      min = entropyResult.entropy;
      minIndex = index2D;
    }
    return false;
  };

  Dimension2D dimension = commonParams.mOutputProperties.dimensions;
  BreakRange::runForDimension(dimension, func);

  Result result;
  if (fail) {
    // Fail because a cell in the wave had no possible patterns that will fit
    result = Result::kFail;
  } else if (min == std::numeric_limits<double>::infinity()) {
    // All cells were finalized "num_superimposed == 1"
    result = Result::kSuccess;
  } else {
    result = Result::kUnfinished;
  }

  return EntropyResult{result, minIndex};
}

Index3D waveIndex(const Index2D &imageIndex, int patternIndex) {
  return append(imageIndex, patternIndex);
}

std::vector<double> createDistribution(const Index2D &index2D, int numPatterns,
                                       const std::vector<double> &weights,
                                       const Array3D<Bool> &wave) {
  std::vector<double> distribution(numPatterns);
  for (int patternIndex = 0; patternIndex < numPatterns; ++patternIndex) {
    Index3D index3D = waveIndex(index2D, patternIndex);

    distribution[patternIndex] = wave[index3D] ? weights[patternIndex] : 0;
  }
  return distribution;
}

size_t selectPattern(const Index2D &index2D, int numPatterns,
                     const std::vector<double> &weights,
                     const Array3D<Bool> &wave,
                     const RandomDouble &randomDouble) {
  std::vector<double> distribution =
      createDistribution(index2D, numPatterns, weights, wave);

  return weightedIndexSelect(distribution, randomDouble());
}

void updateSelectedPattern(AlgorithmData &algorithmData, const Index2D &index2D,
                           int numPatterns, size_t pattern) {
  for (int t = 0; t < numPatterns; ++t) {
    Index3D index3D = waveIndex(index2D, t);

    // Set pattern to true, everything else false
    algorithmData._wave[index3D] = (t == pattern);
  }
  algorithmData._changes[index2D] = true;
}

Result observe(const CommonParams &commonParams, const Model &model,
               AlgorithmData &algorithmData, RandomDouble &random_double) {
  // Find the index in the image with the lowest entropy
  const auto result =
      find_lowest_entropy(commonParams, model, algorithmData._wave);

  if (result.code != Result::kUnfinished) {
    return result.code;
  }

  Index2D index2D = result.minIndex;

  // Select a pattern (with some randomness)
  size_t r = selectPattern(index2D, commonParams.numPatterns,
                           commonParams.patternWeights, algorithmData._wave,
                           random_double);

  // The index is modified in the following way:
  // - Wave set to true at pattern index, false everywhere else
  // - The index is marked in changes
  updateSelectedPattern(algorithmData, index2D, commonParams.numPatterns, r);

  return Result::kUnfinished;
}

Result run(const CommonParams &commonParams, AlgorithmData &algorithmData,
           const Model &model, size_t seed,
           size_t limit) {
  std::mt19937 gen(seed);
  std::uniform_real_distribution<double> dis(0.0, 1.0);
  RandomDouble random_double = [&]() { return dis(gen); };

  for (size_t l = 0; limit == 0 || l < limit; ++l) {
    Result result = observe(commonParams, model, algorithmData, random_double);

    if (result != Result::kUnfinished) {
      std::cout << result2str(result) << " after " << l << " iterations\n";
      return result;
    }
    while (model.propagate(algorithmData))
      ;
  }

  std::cout << "Unfinished after " << limit << " iterations\n";
  return Result::kUnfinished;
}

std::unique_ptr<Image>
createImage(const CommonParams &commonParams, const Model &model, size_t seed,
            size_t limit) {
  AlgorithmData algorithmData = model.initAlgorithmData();

  const auto result = run(commonParams, algorithmData, model, seed, limit);

  if (result == Result::kSuccess) {
    return model.image(algorithmData);
  } else {
    return nullptr;
  }
}

ImageGenerator overlappingGenerator(const OverlappingComputedInfo &config,
                                    size_t limit) {
  OverlappingModel model(config);
  return [limit, model, &config](size_t seed) {
    return createImage(config.commonParams, model, seed, limit);
  };
}

ImageGenerator tileGenerator(const TileModelInternal &config,
                             size_t limit) {
  TileModel model(config);
  return [limit, model, &config](size_t seed) {
    return createImage(config.mCommonParams, model, seed, limit);
  };
}
