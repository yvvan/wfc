#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>

#include <wfc/ranges.h>

#include <wfc/overlapping_types.h>
#include <wfc/overlapping_pattern_extraction.h>

constexpr RGBA white = { 255, 255, 255 };
constexpr RGBA black = { 0, 0, 0 };

// Makes a black/white checkerboard pattern of the desired size where the bottom left
// corner is black.
PalettedImage checkerBoard(size_t width)
{
	Dimension2D dimension{ width, width }; 

	Array2D<ColorIndex> grid(dimension);

	auto functor = [&] (const Index2D& index)
	{
		bool gridVal = ((index.x + index.y ) % 2);
		grid[index] = gridVal ? 1 : 0;
		return false;
	};

	runForDimension(dimension, functor);

	return 
	{
		.data = grid,
		.palette = { white, black }
	};
}

struct FlattenedPatternOccurence
{
	Pattern pattern;

	int occurrence;
};

struct TransformOccurrence
{

	PatternTransformProperties transform;

	int occurrence;

};

std::array<int, 8> convert(const std::vector<TransformOccurrence>& toConvert)
{
	std::array<int, 8> toReturn = {};
	for (const auto& transformOccurence : toConvert)
	{
		int index = enumerateTransformProperties(transformOccurence.transform);
		toReturn[index] = transformOccurence.occurrence;
	}
	return toReturn;
}

bool operator==(const FlattenedPatternOccurence& left, const FlattenedPatternOccurence& right)
{
	return (left.pattern == right.pattern && left.occurrence == right.occurrence);
}

std::array<FlattenedPatternOccurence, 8> flattenPatternOccurrence(const PatternOccurrence& input)
{
	std::array<FlattenedPatternOccurence, 8> toReturn;
	for (int i = 0; i < 8; i++)
	{
		PatternTransformProperties transformProperties = denumerateTransformProperties(i);

		toReturn[i] =
		{
			.pattern = createPattern(input.pattern, transformProperties),
			.occurrence = input.occurrence[i]
		};
	}
	return toReturn;
}

bool patternsEquivalent(const PatternOccurrence& left, const PatternOccurrence& right)
{
	auto flattenedLeft = flattenPatternOccurrence(left);
	auto flattenedRight = flattenPatternOccurrence(right);

	// Don't have to check for size as usual for is_permutation as they're both array size 8.
	return std::is_permutation(flattenedLeft.begin(), flattenedLeft.end(), flattenedRight.begin());
}

bool imagePropertiesEquivalent(const ImagePatternProperties& left, const ImagePatternProperties& right)
{
	// Pattern
	if (left.patterns.size() != right.patterns.size())
	{
		return false;
	}

	bool patternsMatch = std::is_permutation(left.patterns.begin(), left.patterns.end(), right.patterns.begin(), patternsEquivalent);
	if (!patternsMatch)
	{
		return false;
	}

	// Map
	return true;
}

bool enumeratedPatternsEquivalent(const EnumeratedPattern& left, const EnumeratedPattern& right)
{
	PatternTransformProperties leftTransform = denumerateTransformProperties(left.enumeratedTransform);
	PatternTransformProperties rightTransform = denumerateTransformProperties(right.enumeratedTransform);

	Pattern leftTransformedPattern = createPattern(left.pattern, leftTransform);
	Pattern rightTransformedPattern = createPattern(right.pattern, rightTransform);

	return (leftTransformedPattern == rightTransformedPattern);
}

bool imageGridEquivalent(const ImagePatternProperties& left, const ImagePatternProperties& right)
{
	if (left.grid.size() != right.grid.size())
	{
		return false;
	}

	Dimension2D dimension = left.grid.size();

	bool toReturn = true;

	auto functor = [&] (const Index2D& index)
	{
		const PatternIdentifier& leftIdentifier = left.grid[index];
		const PatternIdentifier& rightIdentifier = right.grid[index];

		EnumeratedPattern enumeratedLeft =
		{
			.pattern = left.patterns[leftIdentifier.patternIndex].pattern,
			.enumeratedTransform = leftIdentifier.enumeratedTransform
		};

		EnumeratedPattern enumeratedRight =
		{
			.pattern = right.patterns[rightIdentifier.patternIndex].pattern,
			.enumeratedTransform = rightIdentifier.enumeratedTransform
		};

		bool matching = enumeratedPatternsEquivalent(enumeratedLeft, enumeratedRight);

		if (!matching)
		{
			toReturn = false;
		}

		// If any not matching - break out of the loop
		return !matching;
	};

	BreakRange::runForDimension(dimension, functor);

	return toReturn;
}

ImagePatternProperties expectedEvenCheckerboardProperties(int sizeFactor)
{
	const size_t size = sizeFactor * 2;

	Pattern pattern
	{
		{ 0, 1 },
		{ 1, 0 }
	};

	Dimension2D dimension{ size, size };
	Array2D<PatternIdentifier> grid(dimension);

	PatternTransformProperties normalTransform = { .rotations = 0, .reflected = false };
	PatternTransformProperties reflectedTransform = { .rotations = 0, .reflected = true };

	int normalEnumeratedTransform = enumerateTransformProperties(normalTransform);
	int reflectedEnumeratedTransform = enumerateTransformProperties(reflectedTransform);

	auto fillGrid = [&] (const Index2D& index)
	{
		PatternIdentifier indexIdentifier;

		// Only one pattern for even checkerboards, so all elements will have this index
		indexIdentifier.patternIndex = 0;
		if ((index.x + index.y ) % 2)
		{
			indexIdentifier.enumeratedTransform = reflectedEnumeratedTransform;
		}
		else
		{
			indexIdentifier.enumeratedTransform = normalEnumeratedTransform;
		}

		grid[index] = indexIdentifier;
	};

	runForDimension(dimension, fillGrid);

	int gridArea = size * size;
	return ImagePatternProperties
	{
		.patterns = 
		{
			{
				.pattern = pattern,
				.occurrence = convert(
				{
					{
						.transform = { .rotations = 0, .reflected = false },
						.occurrence = gridArea / 2
					},
					{
						.transform = { .rotations = 0, .reflected = true },
						.occurrence = gridArea / 2
					}
				})
			}
		},
		.grid = grid
	};
}

void evenCheckerboardTest(int sizeFactor)
{
	const size_t size = sizeFactor * 2;
	const int n = 2;

	auto sample = checkerBoard(size);
	ImagePatternProperties expectedProperties = expectedEvenCheckerboardProperties(sizeFactor);

	ImagePatternProperties properties = extractPatternsFromImage(sample, n);

	ASSERT_TRUE(imagePropertiesEquivalent(properties, expectedProperties));
	ASSERT_TRUE(imageGridEquivalent(properties, expectedProperties));
	
}

TEST(OverlappingExtractionTest, test1)
{
	for (int i = 1; i <= 6; i++)
	{
		evenCheckerboardTest(i);
	}
}

ImagePatternProperties expectedOddCheckerboardProperties(int sizeFactor)
{
	const size_t size = (sizeFactor * 2) - 1;

	Pattern pattern
	{
		{ 0, 1 },
		{ 1, 0 }
	};

	Pattern sidePattern
	{
		{ 1, 0 },
		{ 1, 0 }
	};

	Pattern topPattern
	{
		{ 0, 0 },
		{ 0, 0 }
	};

	Dimension2D dimension{ size, size };
	Array2D<PatternIdentifier> grid(dimension);

	PatternTransformProperties normalTransform = { .rotations = 0, .reflected = false };
	PatternTransformProperties reflectedTransform = { .rotations = 0, .reflected = true };

	int normalEnumeratedTransform = enumerateTransformProperties(normalTransform);
	int reflectedEnumeratedTransform = enumerateTransformProperties(reflectedTransform);

	auto fillGrid = [&] (const Index2D& index)
	{
		PatternIdentifier indexIdentifier;

		// Only one pattern for even checkerboards, so all elements will have this index
		indexIdentifier.patternIndex = 0;
		if ((index.x + index.y ) % 2)
		{
			indexIdentifier.enumeratedTransform = reflectedEnumeratedTransform;
		}
		else
		{
			indexIdentifier.enumeratedTransform = normalEnumeratedTransform;
		}

		grid[index] = indexIdentifier;
	};

	Dimension2D reducedDimension{ size - 1, size - 1 };
	runForDimension(reducedDimension, fillGrid);

	for (size_t x = 0; x < size - 1; ++x)
	{
		const size_t y = size - 1;
		PatternIdentifier indexIdentifier;

		indexIdentifier.patternIndex = 1;
		if (x % 2)
		{
			indexIdentifier.enumeratedTransform = reflectedEnumeratedTransform;
		}
		else
		{
			indexIdentifier.enumeratedTransform = normalEnumeratedTransform;
		}
		grid[{ x, y }] = indexIdentifier;
	}

	PatternTransformProperties rotatedNormalTransform = { .rotations = 1, .reflected = false };
	PatternTransformProperties rotatedReflectedTransform = { .rotations = 3, .reflected = false };

	int rotatedNormalEnumeratedTransform = enumerateTransformProperties(rotatedNormalTransform);
	int rotatedReflectedEnumeratedTransform = enumerateTransformProperties(rotatedReflectedTransform);

	for (size_t y = 0; y < size - 1; ++y)
	{
		const size_t x = size - 1;
		PatternIdentifier indexIdentifier;

		indexIdentifier.patternIndex = 1;
		if (y % 2)
		{
			indexIdentifier.enumeratedTransform = rotatedNormalEnumeratedTransform;
		}
		else
		{
			indexIdentifier.enumeratedTransform = rotatedReflectedEnumeratedTransform;
		}
		grid[{ x, y }] = indexIdentifier;
	}


	grid[{ size - 1, size - 1 }] =
	{
		.patternIndex = 2,
		.enumeratedTransform = 0
	};


	int subCheckerboardArea = (size - 1) * (size - 1);

	int subCheckerboardOccurrence = subCheckerboardArea / 2;
	int sideSubOccurrences = static_cast<int>((size - 1) / 2);

	return ImagePatternProperties
	{
		.patterns =
		{
			{
				.pattern = pattern,
				.occurrence = convert(
				{
					{
						.transform = { .rotations = 0, .reflected = false },
						.occurrence = subCheckerboardOccurrence
					},
					{
						.transform = { .rotations = 0, .reflected = true },
						.occurrence = subCheckerboardOccurrence
					}
				})
			},
			{
				.pattern = sidePattern,
				.occurrence = convert(
				{
					{
						.transform = { .rotations = 0, .reflected = false },
						.occurrence = sideSubOccurrences
					},
					{
						.transform = { .rotations = 1, .reflected = false },
						.occurrence = sideSubOccurrences
					},
					{
						.transform = { .rotations = 2, .reflected = false },
						.occurrence = sideSubOccurrences
					},
					{
						.transform = { .rotations = 3, .reflected = false },
						.occurrence = sideSubOccurrences
					}
				})
			},
			{
				.pattern = topPattern,
				.occurrence = convert(
				{
					{
						.transform = { .rotations = 0, .reflected = false },
						.occurrence = 1
					}
				})
			}
		},
		.grid = grid
	};
}

void oddCheckerboardTest(int sizeFactor)
{
	const size_t size = (sizeFactor * 2) - 1;
	const int n = 2;

	auto sample = checkerBoard(size);
	ImagePatternProperties expectedProperties = expectedOddCheckerboardProperties(sizeFactor);

	ImagePatternProperties properties = extractPatternsFromImage(sample, n);

	ASSERT_TRUE(imagePropertiesEquivalent(properties, expectedProperties));
	ASSERT_TRUE(imageGridEquivalent(properties, expectedProperties));
}

TEST(OverlappingExtractionTest, test2)
{
	for (int i = 2; i <= 6; i++)
	{
		oddCheckerboardTest(i);
	}
}
