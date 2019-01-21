#ifndef _WFC_IMODEL_H_
#define _WFC_IMODEL_H_

#include <vector>
#include <experimental/optional>

#include <wfc/rgba.h>
#include <wfc/arrays.h>
#include <wfc/algorithm_data.h>

// Properties of algorithmData image.
struct OutputProperties
{

	Dimension2D dimensions;

	bool periodic;

};

struct CommonParams
{

	OutputProperties mOutputProperties;

	size_t numPatterns;

	// Index of pattern which is at the base of the image if the image has a base. Otherwise, kInvalidIndex
	std::experimental::optional<size_t> foundation; 

	// The weight of each pattern (e.g. how often that pattern occurs in the sample image).
	std::vector<double> patternWeights;

};

using Image = Array2D<RGBA>;

class Model
{

	public:

		virtual bool propagate(AlgorithmData& algorithmData) const = 0;

		virtual bool on_boundary(const Index2D& index) const = 0;

		virtual Image image(const AlgorithmData& algorithmData) const = 0;

		//virtual AlgorithmData createOutput() const = 0;

};


#endif
