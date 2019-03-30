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

#endif
