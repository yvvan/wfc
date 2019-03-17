#include <wfc/overlapping_pattern_extraction.h>

#include <cmath>
#include <iostream>

#include <wfc/ranges.h>

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

PatternInfo calculatePatternInfo(const PalettedImage& image, bool hasFoundation, bool periodicIn, size_t symmetry, int n)
{
	PatternInfo toReturn = {};

	PatternHash foundation = kInvalidHash;
	PatternHash* foundationPtr = (hasFoundation) ? &foundation : nullptr;
	const auto hashed_patterns = extract_patterns(image, n, periodicIn, symmetry, foundationPtr);

	for (const auto& it : hashed_patterns) 
	{
		if (it.first.hash == foundation) 
		{
			// size() = the current index. This should be more explicit.
			// This is also a really roundabout way of setting the foundation
			toReturn.foundation = toReturn.patterns.size();
		}

		WeightedPattern newItem
		{
			.pattern = it.first.pattern,
			.weight = it.second
		};
		toReturn.patterns.push_back(newItem);
	}
	return toReturn;
}

// n = side of the pattern, e.g. 3.
PatternPrevalence extract_patterns(const PalettedImage& sample, int n, bool periodic_in, size_t symmetry, PatternHash* out_lowest_pattern)
{
	Dimension2D imageDimension = sample.data.size();

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
			HashedPattern hashedPattern
			{
				.pattern = ps[k],
				.hash = hash_from_pattern(ps[k], sample.palette.size())
			};
			
			patterns[hashedPattern] += 1;

			if (out_lowest_pattern && index.y == imageDimension.height - 1) 
			{
				*out_lowest_pattern = hashedPattern.hash;
			}
		}
	};

	runForDimension(dimension, rangeFcn);

	return patterns;
}

// A map from hashed pattern to the index in vector
using PatternMap = std::unordered_map<HashedPattern, size_t, PatternHasher>;

PatternTransformProperties convertTransformProperties(int enumerated)
{
	return 
	{
		.rotations = enumerated / 2,
		.reflected = ((enumerated % 2) == 1)
	};
};

auto runOverPatterns(const PalettedImage& sample, int n, const Index2D& index)
{
	return [&sample, n, index] (auto consumerFcn)
	{
		auto currentPattern = patternFromSample(sample, n, index);

		for (int i = 0; i < 4; ++i)
		{
			int baseIndex = 2 * i;

			EnumeratedPattern toConsume
			{
				.pattern = currentPattern,
				.enumeratedTransform = baseIndex 
			};
			if (consumerFcn(toConsume))
			{
				return true;
			}

			toConsume = 
			{
				.pattern = reflect(currentPattern, n),
				.enumeratedTransform = baseIndex + 1 
			};
			if (consumerFcn(toConsume))
			{
				return true;
			}

			currentPattern = rotate(currentPattern, n);
		}
	};
}

ImagePatternProperties extractPatternsFromImage(const PalettedImage& sample, int n)
{
	ImagePatternProperties toReturn;
	toReturn.grid = Array2D<PatternIdentifier>(sample.data.size());

	PatternMap patternMap;

	int count = 0;

	auto rangeFcn = [&] (const Index2D& index)
	{
		std::array<Pattern, 8> ps = generatePatterns(sample, n, index);

		auto hashedValue = patternMap.end();
		int transformEnumeration = 0;

		auto consumerFcn = [&] (const EnumeratedPattern& enumeratedPattern)
		{
			HashedPattern hashedPattern
			{
				.pattern = enumeratedPattern.pattern,
				.hash = hash_from_pattern(enumeratedPattern.pattern, sample.palette.size())
			};

			hashedValue = patternMap.find(hashedPattern);
			transformEnumeration = enumeratedPattern.enumeratedTransform;

			if (hashedValue != patternMap.end())
			{
				return true;
			}
			else
			{
				return false;
			}
		};

		int patternEnumeration = 0;
		for (const auto& pattern : ps)
		{
			if (consumerFcn({ pattern, patternEnumeration }))
			{
				break;
			}
			patternEnumeration++;
		}

		if (hashedValue != patternMap.end())
		{
			PatternIdentifier identifier
			{
				.patternIndex = hashedValue->second,
				.enumeratedTransform = transformEnumeration
			};
			toReturn.grid[index] = identifier;

			toReturn.patterns[hashedValue->second].occurrence[transformEnumeration]++;
		}
		else
		{
			toReturn.patterns.push_back({ ps[0], {} });
			toReturn.patterns.back().occurrence[0] = 1;

			PatternIdentifier identifier
			{
				.patternIndex = toReturn.patterns.size() - 1,
				.enumeratedTransform = 0 
			};
			toReturn.grid[index] = identifier;

			HashedPattern hashedPattern
			{
				.pattern = ps[0],
				.hash = hash_from_pattern(ps[0], sample.palette.size())
			};
			patternMap[hashedPattern] = identifier.patternIndex;
		}
	};

	Dimension2D imageDimension = sample.data.size();
	runForDimension(imageDimension, rangeFcn);

	return toReturn;
}

PatternHash hash_from_pattern(const Pattern& pattern, size_t palette_size)
{
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

std::array<Pattern, 8> generatePatterns(const PalettedImage& sample, int n, const Index2D& index)
{
	std::array<Pattern, 8> toReturn;
	auto consumerFcn = [&toReturn] (const EnumeratedPattern& enumeratedPattern)
	{
		toReturn[enumeratedPattern.enumeratedTransform] = enumeratedPattern.pattern;
		return false;
	};

	auto iterable = runOverPatterns(sample, n, index);

	iterable(consumerFcn);

	return toReturn;
}

Pattern patternFromSample(const PalettedImage& sample, int n, const Index2D& imageIndex)
{
	auto functor = [&] (const Index2D& patternIndex)
	{
		Index2D shiftedIndex = patternIndex + imageIndex;
		Dimension2D dimension = sample.data.size();
		auto index = wrapAroundIndex(shiftedIndex, dimension);
		return sample.data[index];
	};
	return make_pattern(n, functor);
}

Pattern rotate(const Pattern& p, int n)
{
	auto functor = [&] (const Index2D& patternIndex)
	{ 
		return p[{ patternIndex.y, (n - 1) - patternIndex.x }]; 
	};
	return make_pattern(n, functor);
}

Pattern reflect(const Pattern& p, int n)
{
	auto functor = [&] (const Index2D& patternIndex)
	{ 
		return p[{ (n - 1) - patternIndex.x, patternIndex.y }];
	};
	return make_pattern(n, functor);
}

Index2D wrapAroundIndex(const Index2D& index, const Dimension2D& dimension)
{
	return { (index.x % dimension.width), (index.y % dimension.height) };
}

PatternTransformProperties denumerateTransformProperties(int enumeratedTransform)
{
	return 
	{
		.rotations = enumeratedTransform / 2,
		.reflected = ((enumeratedTransform % 2) == 1)
	};
}

int enumerateTransformProperties(const PatternTransformProperties& transformProperties)
{
	return (transformProperties.rotations * 2) + (transformProperties.reflected ? 1 : 0);
}

Pattern createPattern(const Pattern& base, const PatternTransformProperties& transformProperties)
{
	Pattern toReturn = base;

	int n = base.size().width;

	for (int i = 0; i < transformProperties.rotations; i++)
	{
		toReturn = rotate(toReturn, n);	
	}

	if (transformProperties.reflected)
	{
		toReturn = reflect(toReturn, n);
	}

	return toReturn;
}
