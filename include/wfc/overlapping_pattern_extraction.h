#ifndef _OVERLAPPING_PATTERN_EXTRACTION_H_
#define _OVERLAPPING_PATTERN_EXTRACTION_H_

#include <wfc/overlapping_types.h>

#include <experimental/optional>
#include <unordered_map>
#include <vector>

using PatternHash = uint64_t; // Another representation of a Pattern.
const PatternHash kInvalidHash = -1;

// A pattern combined with its hash. This is to make patterns suitable for storage in hashtables
// by allowing their lookup without having to recalculate hash values. The class is immutable
// to ensure that the hash is not accidentally modified.
struct HashedPattern
{

	const Pattern pattern;

	const PatternHash hash;

};

// Hashing functor which just forwards the hash of the stored HashedPattern member.
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

using PatternPrevalence = std::unordered_map<HashedPattern, size_t, PatternHasher>;

PatternHash hash_from_pattern(const Pattern& pattern, size_t palette_size);

struct WeightedPattern
{

	Pattern pattern;

	double weight;

};

struct PatternInfo
{

	std::vector<WeightedPattern> patterns;

	std::experimental::optional<size_t> foundation;

};

// A pattern coupled with the count of its occurrence in the image.
// Occurrence has 8 values, representing all the different ways that patterns
// can be transformed (2 flips * 4 rotations).
struct PatternOccurrence
{

	Pattern pattern;

	Array2D<int> occurrence;

};

// Description of the transformation of a pattern.
// Any pattern may be rotated up to 4 times, with each rotation having a corresponding
// reflected version. Sometimes a transformation will not change the properties of a
// pattern, due to symmetry.
struct PatternTransformProperties
{

	int rotations;

	bool reflected;

};

// Refers to a pattern and a transform on that pattern.
// Instead of storing the full pattern and the full transform information, a slightly
// different form is stored. An index is used to represent the pattern, where the index
// points to an element in a vector. An int is used to represent the transform, instead of
// storing the full transform information. There is a one-to-one function between the enumerated
// transform, and full transform information.
struct PatternIdentifier
{

	size_t patternIndex;

	Index2D enumeratedTransform;

};

// The extracted patterns found in an image.
// There are two components. The first is a vector containing the patterns themselves.
// The second is an Array2D which maps each point in the image to one of the patterns
// in this vector. Each element in the Array2D contains an index pointing to one of the
// patterns in the vector, as well as transform information required to transform the pattern
// to the pattern found in the image.
struct ImagePatternProperties
{

	std::vector<PatternOccurrence> patterns;

	Array2D<PatternIdentifier> grid;

};

struct EnumeratedPattern
{
	Pattern pattern;
	
	Index2D enumeratedTransform;
};

PatternTransformProperties denumerateTransformProperties(Index2D enumeratedTransform);

Index2D enumerateTransformProperties(const PatternTransformProperties& transformProperties);

PatternInfo calculatePatternInfo(const PalettedImage& image, bool hasFoundation, bool periodicIn, size_t symmetry, int n);

// n = side of the pattern, e.g. 3.
PatternPrevalence extract_patterns(const PalettedImage& sample, int n, bool periodic_in, size_t symmetry, PatternHash* out_lowest_pattern);

ImagePatternProperties extractPatternsFromImage(const PalettedImage& sample, int n);

Array2D<Pattern> generatePatterns(const PalettedImage& sample, int n, const Index2D& index);

Pattern patternFromSample(const PalettedImage& sample, int n, const Index2D& index);

Pattern rotate(const Pattern& p, int n);

Pattern reflect(const Pattern& p, int n);

Index2D wrapAroundIndex(const Index2D& index, const Dimension2D& dimension);

Pattern createPattern(const Pattern& base, const PatternTransformProperties& transformProperties);

#endif
