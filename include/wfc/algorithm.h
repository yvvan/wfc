#ifndef _WFC_ALGORITHM_H_
#define _WFC_ALGORITHM_H_

#include <functional>
#include <vector>
#include <experimental/optional>

#include <wfc/imodel.h>
#include <wfc/image_generator.h>

using RandomDouble      = std::function<double()>;

enum class Result
{

	kSuccess,

	kFail,

	kUnfinished,

};

struct EntropyResult
{

	Result code;

	// Only initialized if result is ResultCode::kUnfinished
	Index2D minIndex;

};

struct EntropyValue
{

	size_t num_superimposed;

	double entropy;

};

const char* result2str(const Result result);

double calc_sum(const std::vector<double>& a);

// Pick a random index weighted by a
size_t weightedIndexSelect(const std::vector<double>& a, double randFraction);

EntropyResult find_lowest_entropy(const CommonParams& commonParams, const Model& model, const Array3D<Bool>& wave);

Result observe(const CommonParams& commonParams, const Model& model, AlgorithmData* algorithmData, RandomDouble& random_double);

AlgorithmData create_output(const CommonParams& commonParams, const Model& model);

Result run(const CommonParams& commonParams, AlgorithmData& algorithmData, const Model& model, size_t seed, std::experimental::optional<size_t> limit);

std::experimental::optional<Image> createImage(const CommonParams& commonParams, const Model& model, size_t seed, std::experimental::optional<size_t> limit);

AlgorithmData initialOutput(const CommonParams& commonParams, const Model& model);

void modifyOutputForFoundation(const CommonParams& commonParams, const Model& model, size_t foundation, AlgorithmData& algorithmData);

class OverlappingModelConfig;
class TileModelInternal;
class OverlappingComputedInfo;

ImageGenerator overlappingGenerator(const OverlappingComputedInfo& config, std::experimental::optional<size_t> limit);

ImageGenerator tileGenerator(const TileModelInternal& config, std::experimental::optional<size_t> limit);

std::vector<double> createDistribution(const Index2D& index2D, int numberPatterns, const std::vector<double>& weights, const Array3D<Bool>& wave);

Index3D waveIndex(const Index2D& imageIndex, int patternIndex);

size_t selectPattern(const Index2D& index2D, int numPatterns, const std::vector<double>& weights, const Array3D<Bool>& wave, const RandomDouble& randomDouble);

void updateSelectedPattern(AlgorithmData& algorithmData, const Index2D& index2D, int numPatterns, size_t pattern);

EntropyValue calculateEntropy(const Array3D<Bool>& wave, const Index2D& index2D, size_t numPatterns, const std::vector<double>& patternWeights);

#endif
