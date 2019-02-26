#include <gtest/gtest.h>

#include <iostream>

#include <wfc/ranges.h>

#include <wfc/overlapping_types.h>
#include <wfc/overlapping_pattern_extraction.h>

constexpr RGBA white = { 255, 255, 255 };
constexpr RGBA black = { 0, 0, 0 };

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

TEST(OverlappingExtractionTest, test1)
{
	const size_t size = 10;
	const int n = 2;

	auto sample = checkerBoard(size);

	ImagePatternProperties properties = extractPatternsFromImage(sample, n);

	ASSERT_EQ(properties.patterns.size(), 1);
}
