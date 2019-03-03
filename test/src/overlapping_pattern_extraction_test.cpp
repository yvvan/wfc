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

void evenCheckerboardTest(int sizeFactor)
{
	const size_t size = sizeFactor * 2;
	const int n = 2;

	auto sample = checkerBoard(size);

	Pattern pattern({ n, n });
	pattern[{0, 0 }] = 0;
	pattern[{0, 1 }] = 1;
	pattern[{1, 0 }] = 1;
	pattern[{1, 1 }] = 0;

	Dimension2D dimension{ size, size };
	Array2D<PatternIdentifier> grid(dimension);

	PatternTransformProperties normalTransform = { .rotations = 0, .reflected = false };
	PatternTransformProperties reflectedTransform = { .rotations = 0, .reflected = true };

	int normalEnumeratedTransform = enumerateTransformProperties(normalTransform);
	int reflectedEnumeratedTransform = enumerateTransformProperties(reflectedTransform);

	auto fillGrid = [&] (const Index2D& index)
	{
		if (sample.data[index] == 0)
		{
			grid[index] =
			{
				.patternIndex = 0,
				.enumeratedTransform = normalEnumeratedTransform
			};
		}
		else
		{
			grid[index] =
			{
				.patternIndex = 0,
				.enumeratedTransform = reflectedEnumeratedTransform
			};
		}
	};

	runForDimension(dimension, fillGrid);

	int gridArea = size * size;
	ImagePatternProperties expectedProperties = 
	{
		.patterns = 
		{
			{
				.pattern = pattern,
				.occurrence = {{ gridArea / 2, gridArea / 2 }}
			}
		},
		.grid = grid
	};

	ImagePatternProperties properties = extractPatternsFromImage(sample, n);

	//std::cout << "Sample:\n" << sample.data;

	ASSERT_TRUE(imagePropertiesEquivalent(properties, expectedProperties));
}

TEST(OverlappingExtractionTest, test1)
{
	for (int i = 1; i < 4; i++)
	{
		evenCheckerboardTest(i);
	}
}
