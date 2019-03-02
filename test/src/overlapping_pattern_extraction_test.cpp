#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>

#include <wfc/ranges.h>

#include <wfc/overlapping_types.h>
#include <wfc/overlapping_pattern_extraction.h>

constexpr RGBA white = { 255, 255, 255 };
constexpr RGBA black = { 0, 0, 0 };

// Makes a black/white checkerboard pattern of the desired size where the bottom left
// corner
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

bool patternsEquivalent(const PatternOccurrence& left, const PatternOccurrence& right)
{
	return true;
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

TEST(OverlappingExtractionTest, test1)
{
	const size_t size = 4;
	const int n = 2;

	auto sample = checkerBoard(size);

	Pattern pattern({ n, n });
	pattern[{0, 0 }] = 0;
	pattern[{0, 1 }] = 1;
	pattern[{1, 0 }] = 1;
	pattern[{1, 1 }] = 0;
	Array2D<PatternIdentifier> grid({ size, size });;

	ImagePatternProperties expectedProperties = 
	{
		.patterns = 
		{
			{
				.pattern = pattern,
				.occurrence = {{ size * size }}
			}
		},
		.grid = grid
	};

	ImagePatternProperties properties = extractPatternsFromImage(sample, n);

	//std::cout << "Sample:\n" << sample.data;

	ASSERT_EQ(properties.patterns.size(), 1);
}
