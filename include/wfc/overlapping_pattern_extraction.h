#ifndef _OVERLAPPING_PATTERN_EXTRACTION_H_
#define _OVERLAPPING_PATTERN_EXTRACTION_H_

#include <wfc/overlapping_types.h>

#include <experimental/optional>
#include <unordered_map>
#include <vector>

using PatternHash = uint64_t; // Another representation of a Pattern.
const PatternHash kInvalidHash = -1;

using PatternPrevalence = std::unordered_map<PatternHash, size_t>;

Pattern pattern_from_hash(const PatternHash hash, int n, size_t palette_size);

PatternHash hash_from_pattern(const Pattern& pattern, size_t palette_size);

struct PatternInfo
{

	std::vector<Pattern> patterns;

	std::vector<double> patternWeight;

	std::experimental::optional<size_t> foundation;

};

PatternInfo calculatePatternInfo(const PalettedImage& image, bool hasFoundation, bool periodicIn, bool symmetry, int n);

// n = side of the pattern, e.g. 3.
PatternPrevalence extract_patterns(const PalettedImage& sample, int n, bool periodic_in, size_t symmetry, PatternHash* out_lowest_pattern);

std::array<Pattern, 8> generatePatterns(const PalettedImage& sample, int n, const Index2D& index);

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


Pattern patternFromSample(const PalettedImage& sample, int n, const Index2D& index);

Pattern rotate(const Pattern& p, int n);

Pattern reflect(const Pattern& p, int n);

Index2D wrapAroundIndex(const Index2D& index, const Dimension2D& dimension);


#endif
