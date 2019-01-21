#ifndef _WFC_IMODEL_H_
#define _WFC_IMODEL_H_

#include <vector>
#include <experimental/optional>

#include <wfc/rgba.h>
#include <wfc/arrays.h>

// To avoid problems with vector<bool>
using Bool = uint8_t;

// What actually changes
struct Output
{
	// _width X _height X num_patterns
	// _wave.get(x, y, t) == is the pattern t possible at x, y?
	// Starts off true everywhere.
	Array3D<Bool> _wave;
	Array2D<Bool> _changes; // _width X _height. Starts off false everywhere.
};

struct OutputProperties
{
	Dimension2D dimensions; // Of output image.
	bool periodic;
};

struct CommonParams
{

	OutputProperties mOutputProperties;

	size_t _num_patterns;

	// Index of pattern which is at the base of the image if the image has a base. Otherwise, kInvalidIndex
	std::experimental::optional<size_t> _foundation; 

	// The weight of each pattern (e.g. how often that pattern occurs in the sample image).
	std::vector<double> _pattern_weight; // num_patterns

};

using Image = Array2D<RGBA>;

class Model
{

	public:

		virtual bool propagate(Output& output) const = 0;

		virtual bool on_boundary(const Index2D& index) const = 0;

		virtual Image image(const Output& output) const = 0;

};


#endif
