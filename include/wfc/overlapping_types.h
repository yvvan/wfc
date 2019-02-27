#ifndef _OVERLAPPING_TYPES_H_
#define _OVERLAPPING_TYPES_H_

#include <wfc/rgba.h>
#include <wfc/arrays.h>

#include <vector>

using ColorIndex = uint8_t; // tile index or color index. If you have more than 255, don't.
using Palette = std::vector<RGBA>;
using Pattern = Array2D<ColorIndex>;
using PatternIndex = uint16_t;

struct PalettedImage
{
	Array2D<ColorIndex> data; 
	Palette palette;
};


inline std::ostream& operator<<(std::ostream& ostream, const Pattern& pattern)
{
	Dimension2D dimension = pattern.size();
	for (size_t x = 0; x < dimension.width; x++)
	{
		for (size_t y = 0; y < dimension.height; y++)
		{
			ostream << (int)pattern[{x, y}] << " ";

		}
		ostream << "\n";
	}
	return ostream;
}

#endif
