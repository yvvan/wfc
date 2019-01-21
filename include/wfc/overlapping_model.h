#ifndef _WFC_OVERLAPPING_MODEL_H_
#define _WFC_OVERLAPPING_MODEL_H_

#include <wfc/arrays.h>
#include <wfc/imodel.h>

#include <unordered_map>

using ColorIndex = uint8_t; // tile index or color index. If you have more than 255, don't.
using Palette = std::vector<RGBA>;
using Pattern = Array2D<ColorIndex>;

using Graphics = Array2D<std::vector<ColorIndex>>;
using PatternIndex = uint16_t;
const size_t kUpscale = 4; // Upscale images before saving

using PatternHash = uint64_t; // Another representation of a Pattern.

struct HashedPattern
{

	Pattern pattern;

	uint64_t hash;

};

struct PatternHasher
{

	inline std::size_t operator()(const HashedPattern& hashedPattern) const
	{
		return hashedPattern.hash;
	}
	
};


inline bool operator==(const HashedPattern& left, const HashedPattern& right)
{
	return left.pattern == right.pattern;
}

using PatternPrevalence = std::unordered_map<PatternHash, size_t>;

using Propagator = Array3D<std::vector<PatternIndex>>;

Index2D wrapAroundIndex(const Index2D& index, const Dimension2D& dimension);

struct PalettedImage
{
	Array2D<ColorIndex> data; 
	Palette palette;
};

const PatternHash kInvalidHash = -1;


struct OverlappingModelConfig
{
	PalettedImage sample_image;
	bool periodic_in;
	size_t symmetry;
	bool has_foundation;
	int n;
	OutsideCommonParams commonParam;
};

struct PropagatorStatistics
{

	size_t longest_propagator;

	size_t sum_propagator;

	double average;

};

PropagatorStatistics analyze(const Propagator& propagator);

Propagator createPropagator(size_t numPatterns, int n, const std::vector<Pattern>& patterns);

// n = side of the pattern, e.g. 3.
PatternPrevalence extract_patterns(const PalettedImage& sample, int n, bool periodic_in, size_t symmetry, PatternHash* out_lowest_pattern);

struct OverlappingModelInternal
{
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

	bool propagate(Output& output) const override;

	bool on_boundary(const Index2D& index) const override
	{
		return !mCommonParams.mOutsideCommonParams._periodic_out && (index.x + mInternal._n > mCommonParams.mOutsideCommonParams.dimension.width || index.y + mInternal._n > mCommonParams.mOutsideCommonParams.dimension.height);
	}

	Image image(const Output& output) const override;

	Graphics graphics(const Output& output) const;

private:

	CommonParams mCommonParams;
	
	const OverlappingModelInternal& mInternal;
};

OverlappingComputedInfo fromConfig(const OverlappingModelConfig& config);

Pattern pattern_from_hash(const PatternHash hash, int n, size_t palette_size);

Image image_from_graphics(const Graphics& graphics, const Palette& palette);

Image upsample(const Image& image);

Pattern patternFromSample(const PalettedImage& sample, int n, const Index2D& index);

Pattern rotate(const Pattern& p, int n);

Pattern reflect(const Pattern& p, int n);

PatternHash hash_from_pattern(const Pattern& pattern, size_t palette_size);

RGBA collapsePixel(const std::vector<ColorIndex>& tile_contributors, const Palette& palette);

std::array<Pattern, 8> generatePatterns(const PalettedImage& sample, int n, const Index2D& index);

RGBA averageContributors(const std::vector<ColorIndex>& contributors, const Palette& palette);

bool agrees(const Pattern& p1, const Pattern& p2, int dx, int dy, int n);

struct PatternInfo
{

	std::vector<Pattern> patterns;

	std::vector<double> patternWeight;

	std::experimental::optional<size_t> foundation;

};

PatternInfo calculatePatternInfo(const PalettedImage& image, bool hasFoundation, bool periodicIn, bool symmetry, int n);

#endif
