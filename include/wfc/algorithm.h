#ifndef _WFC_ALGORITHM_H_
#define _WFC_ALGORITHM_H_

#include <functional>
#include <vector>

#include <wfc/imodel.h>

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
size_t weightedIndexSelect(const std::vector<double>& a, double randFraction);

Result find_lowest_entropy(const Model& model, const Output& output, RandomDouble& random_double,
                           int* argminx, int* argminy);

Result observe(const Model& model, Output* output, RandomDouble& random_double);

Output create_output(const Model& model);

Result run(Output& output, const Model& model, size_t seed, size_t limit);


#endif
