#ifndef _WFC_OVERLAPPING_MODEL_H_
#define _WFC_OVERLAPPING_MODEL_H_

#include <wfc/arrays.h>
#include <wfc/imodel.h>
#include <wfc/overlapping_types.h>

using Graphics = Array2D<std::vector<ColorIndex>>;

const size_t kUpscale = 4; // Upscale images before saving

using Propagator = Array3D<std::vector<PatternIndex>>;


struct OverlappingModelConfig
{
	PalettedImage sample_image;
	bool periodic_in;
	size_t symmetry;
	bool hasfoundation;
	int n;
	OutputProperties outputProperties;
};

struct PropagatorStatistics
{

	size_t longest_propagator;

	size_t sum_propagator;

	double average;

};

PropagatorStatistics analyze(const Propagator& propagator);

Propagator createPropagator(size_t numPatterns, int n, const std::vector<Pattern>& patterns);

struct OverlappingModelInternal
{
	// Index of pattern which is at the base of the image if the image has a base. Otherwise, kInvalidIndex
	std::experimental::optional<size_t> foundation; 

	int _n;
	// num_patterns X (2 * n - 1) X (2 * n - 1) X ???
	// list of other pattern indices that agree on this x/y offset (?)
	Propagator _propagator;
	std::vector<Pattern> _patterns;
	Palette _palette;
};

struct OverlappingComputedInfo
{

	OverlappingModelInternal internal;

	CommonParams commonParams; 
	
};

class OverlappingModel : public Model
{
public:
	OverlappingModel(const OverlappingComputedInfo& config);

	bool propagate(AlgorithmData& algorithmData) const override;

	bool on_boundary(const Index2D& index) const override
	{
		return !mCommonParams.mOutputProperties.periodic && (index.x + mInternal._n > mCommonParams.mOutputProperties.dimensions.width || index.y + mInternal._n > mCommonParams.mOutputProperties.dimensions.height);
	}

	Image image(const AlgorithmData& algorithmData) const override;

	Graphics graphics(const AlgorithmData& algorithmData) const;

	AlgorithmData initAlgorithmData() const override;
private:

	CommonParams mCommonParams;
	
	const OverlappingModelInternal& mInternal;
};

OverlappingComputedInfo fromConfig(const OverlappingModelConfig& config);

Image image_from_graphics(const Graphics& graphics, const Palette& palette);

Image upsample(const Image& image);

RGBA collapsePixel(const std::vector<ColorIndex>& tile_contributors, const Palette& palette);

RGBA averageContributors(const std::vector<ColorIndex>& contributors, const Palette& palette);

bool agrees(const Pattern& p1, const Pattern& p2, int dx, int dy, int n);

void modifyOutputForFoundation(const CommonParams& commonParams, const Model& model, size_t foundation, AlgorithmData& algorithmData);

#endif
