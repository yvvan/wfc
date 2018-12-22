#ifndef _WFC_OVERLAPPING_MODEL_H_
#define _WFC_OVERLAPPING_MODEL_H_

#include <wfc/arrays.h>
#include <wfc/imodel.h>

#include <unordered_map>


using ColorIndex        = uint8_t; // tile index or color index. If you have more than 255, don't.
using Palette           = std::vector<RGBA>;
using Pattern           = Array2D<ColorIndex>;

using Graphics = Array2D<std::vector<ColorIndex>>;
using PatternIndex      = uint16_t;
const size_t kUpscale             =   4; // Upscale images before saving

using PatternHash       = uint64_t; // Another representation of a Pattern.

using PatternPrevalence = std::unordered_map<PatternHash, size_t>;

struct PalettedImage
{
	size_t                  width, height;
	std::vector<ColorIndex> data; // width * height
	Palette                 palette;

	inline ColorIndex at_wrapped(const Index2D& index) const
	{
		return data[width * (index.y % height) + (index.x % width)];
	}
};

const PatternHash kInvalidHash = -1;


struct OverlappingModelConfig
{
	PalettedImage sample_image;
	bool periodic_in;
	size_t symmetry;
	bool has_foundation;
	int                      n;
	OutsideCommonParams commonParam;
};

// n = side of the pattern, e.g. 3.
PatternPrevalence extract_patterns(
	const PalettedImage& sample, int n, bool periodic_in, size_t symmetry,
	PatternHash* out_lowest_pattern);

class OverlappingModel : public Model
{
public:
	OverlappingModel(OverlappingModelConfig config);

	bool propagate(Output& output) const override;

	bool on_boundary(int x, int y) const override
	{
		return !mCommonParams.mOutsideCommonParams._periodic_out && (x + _n > mCommonParams.mOutsideCommonParams._width || y + _n > mCommonParams.mOutsideCommonParams._height);
	}

	Image image(const Output& output) const override;

	Graphics graphics(const Output& output) const;

private:
	int                       _n;
	// num_patterns X (2 * n - 1) X (2 * n - 1) X ???
	// list of other pattern indices that agree on this x/y offset (?)
	Array3D<std::vector<PatternIndex>> _propagator;
	std::vector<Pattern>               _patterns;
	Palette                            _palette;
};

Pattern pattern_from_hash(const PatternHash hash, int n, size_t palette_size);

Image image_from_graphics(const Graphics& graphics, const Palette& palette);

Image upsample(const Image& image);

Pattern patternFromSample(const PalettedImage& sample, int n, const Index2D& index);

Pattern rotate(const Pattern& p, int n);

Pattern reflect(const Pattern& p, int n);

PatternHash hash_from_pattern(const Pattern& pattern, size_t palette_size);

RGBA collapsePixel(const std::vector<ColorIndex>& tile_contributors, const Palette& palette);

#endif
