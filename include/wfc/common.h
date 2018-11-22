#ifndef _WFC_COMMON_H_
#define _WFC_COMMON_H_

#include <unordered_map>
#include <cstddef>

const size_t kInvalidIndex = -1;
using ColorIndex        = uint8_t; // tile index or color index. If you have more than 255, don't.
using Palette           = std::vector<RGBA>;
using Pattern           = std::vector<ColorIndex>;
using PatternHash       = uint64_t; // Another representation of a Pattern.
using PatternPrevalence = std::unordered_map<PatternHash, size_t>;

using Graphics = Array2D<std::vector<ColorIndex>>;
using PatternIndex      = uint16_t;

#endif
