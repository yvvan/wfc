#ifndef _WFC_OVERLAPPING_MODEL_H_
#define _WFC_OVERLAPPING_MODEL_H_

#include <wfc/arrays.h>
#include <wfc/imodel.h>
#include <wfc/common.h>

struct OverlappingModelConfig
{
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

	bool propagate(Output* output) const override;

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

#endif
