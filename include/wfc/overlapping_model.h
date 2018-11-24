#ifndef _WFC_OVERLAPPING_MODEL_H_
#define _WFC_OVERLAPPING_MODEL_H_

#include <wfc/arrays.h>
#include <wfc/imodel.h>
#include <wfc/common.h>

const size_t kUpscale             =   4; // Upscale images before saving

struct PalettedImage
{
	size_t                  width, height;
	std::vector<ColorIndex> data; // width * height
	Palette                 palette;

	inline ColorIndex at_wrapped(size_t x, size_t y) const
	{
		return data[width * (y % height) + (x % width)];
	}
};


struct OverlappingModelConfig
{
	PalettedImage sample_image;
	bool periodic_in;
	size_t symmetry;
	bool has_foundation;
	const PatternPrevalence hashed_patterns;
	const Palette           palette;
	int                      n;
	OutsideCommonParams commonParam;
	PatternHash              foundation_pattern;
};

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

#endif
