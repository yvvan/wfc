#pragma once

#include <functional>
#include <vector>

#include <wfc/image_generator.h>
#include <wfc/imodel.h>

using RandomDouble = std::function<double()>;

enum class Result {

  kSuccess,

  kFail,

  kUnfinished,

};

struct EntropyResult {

  Result code;

  // Only initialized if result is ResultCode::kUnfinished
  Index2D minIndex;
};

struct EntropyValue {

  size_t num_superimposed;

  double entropy;
};

const char *result2str(const Result result);

double calc_sum(const std::vector<double> &a);

// Pick a random index weighted by a
size_t weightedIndexSelect(const std::vector<double> &a, double randFraction);

EntropyResult find_lowest_entropy(const CommonParams &commonParams,
                                  const Model &model,
                                  const Array3D<Bool> &wave);

Result observe(const CommonParams &commonParams, const Model &model,
               AlgorithmData *algorithmData, RandomDouble &random_double);

Result run(const CommonParams &commonParams, AlgorithmData &algorithmData,
           const Model &model, size_t seed,
           size_t limit = 0);

std::unique_ptr<Image>
createImage(const CommonParams &commonParams, const Model &model, size_t seed,
            size_t limit = 0);

AlgorithmData initialOutput(const CommonParams &commonParams,
                            const Model &model);

class OverlappingModelConfig;
class TileModelInternal;
class OverlappingComputedInfo;

ImageGenerator overlappingGenerator(const OverlappingComputedInfo &config,
                                    size_t limit = 0);

ImageGenerator tileGenerator(const TileModelInternal &config,
                             size_t limit = 0);

std::vector<double> createDistribution(const Index2D &index2D,
                                       int numberPatterns,
                                       const std::vector<double> &weights,
                                       const Array3D<Bool> &wave);

Index3D waveIndex(const Index2D &imageIndex, int patternIndex);

size_t selectPattern(const Index2D &index2D, int numPatterns,
                     const std::vector<double> &weights,
                     const Array3D<Bool> &wave,
                     const RandomDouble &randomDouble);

void updateSelectedPattern(AlgorithmData &algorithmData, const Index2D &index2D,
                           int numPatterns, size_t pattern);

EntropyValue calculateEntropy(const Array3D<Bool> &wave, const Index2D &index2D,
                              size_t numPatterns,
                              const std::vector<double> &patternWeights);
