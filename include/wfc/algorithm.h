#ifndef _WFC_ALGORITHM_H_
#define _WFC_ALGORITHM_H_

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

using emilib::irange;

using RandomDouble      = std::function<double()>;



enum class Result
{
	kSuccess,
	kFail,
	kUnfinished,
};

const char* result2str(const Result result);

double calc_sum(const std::vector<double>& a);

// Pick a random index weighted by a
size_t spin_the_bottle(const std::vector<double>& a, double between_zero_and_one);

Result find_lowest_entropy(const Model& model, const Output& output, RandomDouble& random_double,
                           int* argminx, int* argminy);

Result observe(const Model& model, Output* output, RandomDouble& random_double);

Output create_output(const Model& model);

Result run(Output* output, const Model& model, size_t seed, size_t limit);

void run_and_write(const std::string& name, size_t limit, size_t screenshots, const Model& model);

#endif
