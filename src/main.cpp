#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>

#include <configuru.hpp>
#include <emilib/irange.hpp>
#include <emilib/strprintf.hpp>
#include <loguru.hpp>
#include <stb_image.h>
#include <stb_image_write.h>


#include <wfc/configuru.h>
#include <wfc/overlapping_model.h>
#include <wfc/tile_model.h>
#include <wfc/arrays.h>
#include <wfc/imodel.h>
#include <wfc/common.h>

const auto kUsage = R"(
wfc.bin [-h/--help] [job=samples.cfg, ...]
	-h/--help   Print this help
	file        Jobs to run
)";

using emilib::irange;

using RandomDouble      = std::function<double()>;



enum class Result
{
	kSuccess,
	kFail,
	kUnfinished,
};

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
size_t spin_the_bottle(const std::vector<double>& a, double between_zero_and_one)
{
	double sum = calc_sum(a);

	if (sum == 0.0) 
	{
		return std::floor(between_zero_and_one * a.size());
	}

	double between_zero_and_sum = between_zero_and_one * sum;

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

Result find_lowest_entropy(const Model& model, const Output& output, RandomDouble& random_double,
                           int* argminx, int* argminy)
{
	// We actually calculate exp(entropy), i.e. the sum of the weights of the possible patterns

	double min = std::numeric_limits<double>::infinity();

	for (int x = 0; x < model.mCommonParams.mOutsideCommonParams._width; ++x) 
	{
		for (int y = 0; y < model.mCommonParams.mOutsideCommonParams._height; ++y) 
		{
			if (model.on_boundary(x, y)) { continue; }

			size_t num_superimposed = 0;
			double entropy = 0;

			for (int t = 0; t < model.mCommonParams._num_patterns; ++t) 
			{
				if (output._wave.ref(x, y, t)) 
				{
					num_superimposed += 1;
					entropy += model.mCommonParams._pattern_weight[t];
				}
			}

			if (entropy == 0 || num_superimposed == 0) 
			{
				return Result::kFail;
			}

			if (num_superimposed == 1) 
			{
				continue; // Already frozen
			}

			// Add a tie-breaking bias:
			const double noise = 0.5 * random_double();
			entropy += noise;

			if (entropy < min) 
			{
				min = entropy;
				*argminx = x;
				*argminy = y;
			}
		}
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

Result observe(const Model& model, Output* output, RandomDouble& random_double)
{
	int argminx, argminy;
	const auto result = find_lowest_entropy(model, *output, random_double, &argminx, &argminy);
	if (result != Result::kUnfinished) { return result; }

	std::vector<double> distribution(model.mCommonParams._num_patterns);
	for (int t = 0; t < model.mCommonParams._num_patterns; ++t) 
	{
		distribution[t] = output->_wave.ref(argminx, argminy, t) ? model.mCommonParams._pattern_weight[t] : 0;
	}
	size_t r = spin_the_bottle(distribution, random_double());
	for (int t = 0; t < model.mCommonParams._num_patterns; ++t) 
	{
		output->_wave.ref(argminx, argminy, t) = (t == r);
	}
	output->_changes.ref(argminx, argminy) = true;

	return Result::kUnfinished;
}

Output create_output(const Model& model)
{
	Output output;
	output._wave = Array3D<Bool>(model.mCommonParams.mOutsideCommonParams._width, model.mCommonParams.mOutsideCommonParams._height, model.mCommonParams._num_patterns, true);
	output._changes = Array2D<Bool>(model.mCommonParams.mOutsideCommonParams._width, model.mCommonParams.mOutsideCommonParams._height, false);

	if (model.mCommonParams._foundation != kInvalidIndex) 
	{
		for (const auto x : irange(model.mCommonParams.mOutsideCommonParams._width)) 
		{
			for (const auto t : irange(model.mCommonParams._num_patterns)) 
			{
				if (t != model.mCommonParams._foundation) 
				{
					output._wave.ref(x, model.mCommonParams.mOutsideCommonParams._height - 1, t) = false;
				}
			}
			output._changes.ref(x, model.mCommonParams.mOutsideCommonParams._height - 1) = true;

			for (const auto y : irange(model.mCommonParams.mOutsideCommonParams._height - 1)) 
			{
				output._wave.ref(x, y, model.mCommonParams._foundation) = false;
				output._changes.ref(x, y) = true;
			}

			while (model.propagate(&output));
		}
	}

	return output;
}

Result run(Output* output, const Model& model, size_t seed, size_t limit)
{
	std::mt19937 gen(seed);
	std::uniform_real_distribution<double> dis(0.0, 1.0);
	RandomDouble random_double = [&]() { return dis(gen); };

	for (size_t l = 0; l < limit || limit == 0; ++l) 
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

void run_and_write(const std::string& name, size_t limit, size_t screenshots, const Model& model)
{
	for (const auto i : irange(screenshots)) 
	{
		for (const auto attempt : irange(10)) 
		{
			(void)attempt;
			int seed = rand();

			Output output = create_output(model);

			const auto result = run(&output, model, seed, limit);

			if (result == Result::kSuccess) 
			{
				const auto image = model.image(output);
				const auto out_path = emilib::strprintf("output/%s_%lu.png", name.c_str(), i);
				CHECK_F(stbi_write_png(out_path.c_str(), image.width(), image.height(), 4, image.data(), 0) != 0,
				        "Failed to write image to %s", out_path.c_str());
				break;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	loguru::init(argc, argv);

	std::vector<std::string> files;

	for (int i = 1; i < argc; ++i) 
	{
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) 
		{
			printf(kUsage);
			exit(0);
		} 
		else 
		{
			files.push_back(argv[i]);
		}
	}

	if (files.empty()) 
	{
		files.push_back("samples.cfg");
	}

	ConfigActions actions =
	{
		.overlappingAction = [] (const GeneralConfig& generalConfig, const OverlappingModelConfig& overlappingModelConfig)
		{
			auto model = std::make_unique<OverlappingModel>(overlappingModelConfig);
			run_and_write(generalConfig.name, generalConfig.limit, generalConfig.screenshots, *model);
		},
		.tileAction = [] (const GeneralConfig& generalConfig, const TileModelInternal& internal)
		{
			auto model = std::make_unique<TileModel>(internal);

			run_and_write(generalConfig.name, generalConfig.limit, generalConfig.screenshots, *model);
		}
	};

	for (const auto& file : files) 
	{
		run_config_file(file, actions);
	}
}
