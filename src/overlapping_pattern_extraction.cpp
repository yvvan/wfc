#include <wfc/overlapping_pattern_extraction.h>

#include <cmath>
#include <iostream>

#include <emilib/irange.hpp>

#include <wfc/ranges.h>

using namespace emilib;

template <class Functor>
Pattern make_pattern(int n, Functor fun)
{
	Dimension2D dimension{ n, n };

	Pattern result(dimension);

	auto rangeFcn = [&] (const Index2D& index)
	{
		result[index] = fun(index);
	};

	runForDimension(dimension, rangeFcn);

	return result;
}

PatternInfo calculatePatternInfo(const PalettedImage& image, bool hasFoundation, bool periodicIn, bool symmetry, int n)
{
	PatternInfo toReturn = {};

	PatternHash foundation = kInvalidHash;
	PatternHash* foundationPtr = (hasFoundation) ? &foundation : nullptr;
	const auto hashed_patterns = extract_patterns(image, n, periodicIn, symmetry, foundationPtr);

	for (const auto& it : hashed_patterns) 
	{
		if (it.first == foundation) 
		{
			// size() = the current index. This should be more explicit.
			// This is also a really roundabout way of setting the foundation
			toReturn.foundation = toReturn.patterns.size();
		}

		toReturn.patterns.push_back(pattern_from_hash(it.first, n, image.palette.size()));
		toReturn.patternWeight.push_back(it.second);
	}
	return toReturn;
}

// n = side of the pattern, e.g. 3.
PatternPrevalence extract_patterns(const PalettedImage& sample, int n, bool periodic_in, size_t symmetry, PatternHash* out_lowest_pattern)
{
	Dimension2D imageDimension = sample.data.size();

	CHECK_LE_F(n, imageDimension.width);
	CHECK_LE_F(n, imageDimension.height);

	std::unordered_map<HashedPattern, size_t, PatternHasher> newPatterns;

	PatternPrevalence patterns;

	Dimension2D dimension;
	if (periodic_in)
	{
		dimension = imageDimension;
	}
	else
	{
		dimension = 
		{
			.width = imageDimension.width - n + 1,
			.height = imageDimension.height - n + 1
		};
	}

	auto rangeFcn = [&] (const Index2D& index)
	{
		std::array<Pattern, 8> ps = generatePatterns(sample, n, index);

		for (int k = 0; k < symmetry; ++k) 
		{
			auto hash = hash_from_pattern(ps[k], sample.palette.size());
			patterns[hash] += 1;

			if (out_lowest_pattern && index.y == imageDimension.height - 1) 
			{
				*out_lowest_pattern = hash;
			}
		}
	};

	runForDimension(dimension, rangeFcn);

	return patterns;
}

PatternHash hash_from_pattern(const Pattern& pattern, size_t palette_size)
{
	CHECK_LT_F(std::pow((double)palette_size, (double)area(pattern.size())),
	           std::pow(2.0, sizeof(PatternHash) * 8),
	           "Too large palette (it is %lu) or too large pattern size (it's %.0f)",
	           palette_size, std::sqrt(area(pattern.size())));

	PatternHash result = 0;
	size_t power = 1;

	Dimension2D dim = pattern.size();

	for (int y = dim.height - 1; y >= 0; --y)
	{
		for (int x = dim.width - 1; x >= 0; --x) 
		{
			result += pattern[{ x, y }] * power;
			power *= palette_size;
		}
	}
	return result;
}

Pattern pattern_from_hash(const PatternHash hash, int n, size_t palette_size)
{
	size_t residue = hash;
	size_t power = std::pow(palette_size, n * n);
	Pattern result({ n, n });

	for (auto y : irange(n)) 
	{
		for (auto x : irange(n)) 
		{
			power /= palette_size;
			size_t count = 0;

			while (residue >= power)
			{
				residue -= power;
				count++;
			}

			result[{x, y}] = static_cast<ColorIndex>(count);
		}
	}

	return result;
}

std::array<Pattern, 8> generatePatterns(const PalettedImage& sample, int n, const Index2D& index)
{
	std::array<Pattern, 8> toReturn;

	auto currentPattern = patternFromSample(sample, n, index);

	for (int i = 0; i < 4; ++i)
	{
		toReturn[i] = currentPattern;
		toReturn[i + 1] = reflect(currentPattern, n);

		currentPattern = rotate(currentPattern, n);
	}

	return toReturn;
}

Pattern patternFromSample(const PalettedImage& sample, int n, const Index2D& imageIndex)
{
	auto functor = [&] (const Index2D& patternIndex)
	{
		Index2D shiftedIndex = patternIndex + imageIndex;
		Dimension2D dimension = sample.data.size();
		return sample.data[wrapAroundIndex(shiftedIndex, dimension)];
	};
	return make_pattern(n, functor);
}

Pattern rotate(const Pattern& p, int n)
{
	auto functor = [&] (const Index2D& patternIndex)
	{ 
		return p[{ (n - 1 - patternIndex.y), patternIndex.x }]; 
	};
	return make_pattern(n, functor);
}

Pattern reflect(const Pattern& p, int n)
{
	auto functor = [&] (const Index2D& patternIndex)
	{ 
		return p[{ (n - 1 - patternIndex.x), patternIndex.y }];
	};
	return make_pattern(n, functor);
}

Index2D wrapAroundIndex(const Index2D& index, const Dimension2D& dimension)
{
	return { (index.x % dimension.width), (index.y % dimension.height) };
}

