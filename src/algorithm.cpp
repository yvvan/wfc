#include <wfc/algorithm.h>

#include <wfc/overlapping_model.h>
#include <wfc/tile_model.h>
#include <wfc/ranges.h>

#include <random>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <iostream>

#include <emilib/irange.hpp>

using emilib::irange;

const char* result2str(const Result result)
{
	return result == Result::kSuccess ? "success"
	     : result == Result::kFail    ? "fail"
	     : "unfinished";
}

double calc_sum(const std::vector<double>& a)
{
	return std::accumulate(a.begin(), a.end(), 0.0);
}

// Pick a random index weighted by a
size_t weightedIndexSelect(const std::vector<double>& a, double randFraction)
{
	double sum = calc_sum(a);

	if (sum == 0.0) 
	{
		// ???
		return std::floor(randFraction * a.size());
	}

	double between_zero_and_sum = randFraction * sum;

	double accumulated = 0;

	for (auto i : irange(a.size())) 
	{
		accumulated += a[i];
		if (between_zero_and_sum <= accumulated) 
		{
			return i;
		}
	}

	return 0;
}

Result find_lowest_entropy(const Model& model, const Output& output, Index2D& toReturn)
{
	// We actually calculate exp(entropy), i.e. the sum of the weights of the possible patterns

	double min = std::numeric_limits<double>::infinity();

	std::experimental::optional<Result> result;

	auto func = [&] (auto index2D)
	{
		if (model.on_boundary(index2D.x, index2D.y))
		{
			return false;
		}

		size_t num_superimposed = 0;
		double entropy = 0;

		for (int t = 0; t < model.mCommonParams._num_patterns; ++t) 
		{
			Index3D index = append(index2D, t);
			if (output._wave[index]) 
			{
				num_superimposed += 1;
				entropy += model.mCommonParams._pattern_weight[t];
			}
		}

		if (entropy == 0 || num_superimposed == 0) 
		{
			result = Result::kFail;
			return true;
		}

		if (num_superimposed == 1) 
		{
			return false; // Already frozen
		}

		// TODO: Add this back in, or remove?
		/*
		// Add a tie-breaking bias:
		const double noise = 0.5 * random_double();
		entropy += noise;
		*/

		if (entropy < min) 
		{
			min = entropy;
			toReturn = index2D;
		}
		return false;
	};

	Dimension2D dimension = model.mCommonParams.mOutsideCommonParams.dimension;
	BreakRange::runForDimension(dimension, func);

	if (result)
	{
		return *result;
	}

	if (min == std::numeric_limits<double>::infinity()) 
	{
		return Result::kSuccess;
	}
	else 
	{
		return Result::kUnfinished;
	}
}

Result observe(const Model& model, Output& output, RandomDouble& random_double)
{
	Index2D index2D;
	const auto result = find_lowest_entropy(model, output, index2D);
	if (result != Result::kUnfinished) { return result; }


	std::vector<double> distribution(model.mCommonParams._num_patterns);
	for (int t = 0; t < model.mCommonParams._num_patterns; ++t) 
	{
		Index3D index3D = append(index2D, t);
		distribution[t] = output._wave[index3D] ? model.mCommonParams._pattern_weight[t] : 0;
	}

	size_t r = weightedIndexSelect(distribution, random_double());
	for (int t = 0; t < model.mCommonParams._num_patterns; ++t) 
	{
		Index3D index3D = append(index2D, t);
		output._wave[index3D] = (t == r);
	}
	output._changes[index2D] = true;

	return Result::kUnfinished;
}

void modifyOutputForFoundation(const Model& model, Output& output)
{
	Dimension2D dimension = output._changes.size();
	for (const auto x : irange(dimension.width)) 
	{
		// Setting the foundation section of the output wave only true for foundation
		for (const auto t : irange(model.mCommonParams._num_patterns)) 
		{
			if (t != *(model.mCommonParams._foundation)) 
			{
				Index3D index{ x, dimension.height - 1, t };
				output._wave[index] = false;
			}
		}

		// Setting the rest of the output wave only true for not foundation
		for (const auto y : irange(dimension.height - 1)) 
		{
			Index3D index{ x, y, *(model.mCommonParams._foundation) };
			output._wave[index] = false;
		}

		for (const auto y : irange(dimension.height)) 
		{
			Index2D index{ x, y };
			output._changes[index] = true;
		}

		while (model.propagate(output));
	}
}

Output initialOutput(const Model& model)
{
	Dimension2D dimension = model.mCommonParams.mOutsideCommonParams.dimension;
	Dimension3D waveDimension = append(dimension, model.mCommonParams._num_patterns);
	return
	{
		._wave = Array3D<Bool>(waveDimension, true),
		._changes = Array2D<Bool>(dimension, false)
	};
}

Output create_output(const Model& model)
{
	Output output = initialOutput(model);
	if (model.mCommonParams._foundation) 
	{
		// Tile has a clearly-defined "ground"/"foundation"
		modifyOutputForFoundation(model, output);
	}

	return output;
}

Result run(Output& output, const Model& model, size_t seed, std::experimental::optional<size_t> limit)
{
	std::mt19937 gen(seed);
	std::uniform_real_distribution<double> dis(0.0, 1.0);
	RandomDouble random_double = [&]() { return dis(gen); };

	for (size_t l = 0; !(limit) || l < *limit; ++l) 
	{
		Result result = observe(model, output, random_double);

		if (result != Result::kUnfinished) 
		{

			LOG_F(INFO, "%s after %lu iterations", result2str(result), l);
			return result;
		}
		while (model.propagate(output));
	}

	LOG_F(INFO, "Unfinished after %lu iterations", limit);
	return Result::kUnfinished;
}

std::experimental::optional<Image> createImage(const Model& model, size_t seed, std::experimental::optional<size_t> limit)
{
	Output output = create_output(model);

	const auto result = run(output, model, seed, limit);

	if (result == Result::kSuccess) 
	{
		return model.image(output);
	}
	else
	{
		return std::experimental::optional<Image>();
	}
}

ImageGenerator overlappingGenerator(const OverlappingModelConfig& config, std::experimental::optional<size_t> limit)
{
	OverlappingModel model(config);
	return [=] (size_t seed)
	{
		return createImage(model, seed, limit);
	};
}

ImageGenerator tileGenerator(const TileModelInternal& config, std::experimental::optional<size_t> limit)
{
	TileModel model(config);
	return [=] (size_t seed)
	{
		return createImage(model, seed, limit);
	};
}
