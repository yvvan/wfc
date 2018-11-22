#ifndef _WFC_IMODEL_H_
#define _WFC_IMODEL_H_

#include <vector>

#include <wfc/rgba.h>
#include <wfc/arrays.h>

using Bool              = uint8_t; // To avoid problems with vector<bool>

// What actually changes
struct Output
{
	// _width X _height X num_patterns
	// _wave.get(x, y, t) == is the pattern t possible at x, y?
	// Starts off true everywhere.
	Array3D<Bool> _wave;
	Array2D<Bool> _changes; // _width X _height. Starts off false everywhere.
};

struct OutsideCommonParams
{
	size_t _width;      // Of output image.
	size_t _height;     // Of output image.
	bool   _periodic_out;
};

struct CommonParams
{
	OutsideCommonParams mOutsideCommonParams;
	size_t _num_patterns;
	size_t _foundation; // Index of pattern which is at the base, or kInvalidIndex

	// The weight of each pattern (e.g. how often that pattern occurs in the sample image).
	std::vector<double> _pattern_weight; // num_patterns
};

using Image = Array2D<RGBA>;

class Model
{
public:

	CommonParams mCommonParams;

	virtual bool propagate(Output& output) const = 0;
	virtual bool on_boundary(int x, int y) const = 0;
	virtual Image image(const Output& output) const = 0;
};


#endif
