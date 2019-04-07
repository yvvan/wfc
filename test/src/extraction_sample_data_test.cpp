#include <gtest/gtest.h>

#include <wfc/overlapping_types.h>
#include <wfc/overlapping_pattern_extraction.h>
#include <wfc/pattern_properties_comparison.h>

// This test contains an example data
TEST(ExtractionSampleData, test1)
{
	const int n = 2;

	Array2D<ColorIndex> image = 
	{
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 1, 1, 1, 1, 2, 2, 1, 1, 2, 1, 2, 1, 1, 2, 2, },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 1, 2, 2, 2, 0, 1, 1, 1, 1, 2, 1, 2, 1, 1, 2, 2, 2, 2, 1, 2, },
		{ 0, 1, 1, 1, 2, 1, 1, 2, 1, 2, 1, 1, 2, 2, 2, 2, 0, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, },
		{ 0, 1, 1, 2, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 0, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 0, 2, 2, 2, 2, },
		{ 0, 1, 2, 0, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 1, 2, 1, 2, 2, 2, 2, 0, 2, 0, 2, 2, 0, },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 1, 1, 1, 1, 1, 1, 1, },
		{ 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 1, },
		{ 2, 2, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 2, 2, 1, 2, 1, 1, 2, 2, 0, 1, 1, 1, 1, 1, 1, 1, },
		{ 2, 1, 1, 1, 1, 1, 2, 2, 0, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 1, 1, 2, 1, 2, 0, 1, 1, 2, 1, 2, 1, 1, },
		{ 2, 2, 1, 2, 2, 2, 2, 2, 0, 1, 1, 2, 1, 2, 1, 2, 2, 1, 1, 2, 2, 1, 2, 2, 0, 1, 2, 2, 2, 1, 1, 2, },
		{ 2, 1, 1, 1, 2, 2, 2, 2, 0, 1, 1, 1, 2, 1, 2, 1, 1, 2, 2, 1, 2, 2, 2, 2, 0, 2, 1, 2, 1, 1, 2, 2, },
		{ 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, },
		{ 0, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, },
		{ 0, 1, 1, 1, 1, 1, 2, 1, 2, 2, 1, 1, 1, 1, 2, 2, 0, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 2, },
		{ 0, 1, 2, 1, 1, 2, 1, 2, 2, 1, 1, 2, 2, 2, 2, 2, 0, 1, 1, 1, 2, 1, 1, 2, 2, 1, 2, 2, 1, 1, 2, 2, },
		{ 0, 1, 1, 1, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 0, 1, 1, 1, 2, 1, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, },
		{ 0, 1, 2, 2, 2, 1, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 0, 1, 1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, },
		{ 0, 1, 1, 1, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 0, 2, 2, 2, 1, 1, 2, 0, 0, 0, 2, 2, 2, 2, 2, 2, },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{ 1, 1, 1, 1, 1, 1, 1, 2, 0, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 2, 1, 1, 1, 1, 1, 1, },
		{ 1, 2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 0, 1, 1, 1, 1, 1, 1, 1, },
		{ 1, 1, 2, 2, 2, 1, 2, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 2, 2, 0, 1, 1, 1, 1, 1, 1, 2, },
		{ 1, 1, 1, 1, 2, 2, 2, 2, 0, 1, 1, 1, 1, 2, 1, 2, 2, 1, 2, 1, 2, 2, 2, 2, 0, 1, 1, 1, 2, 1, 2, 1, },
		{ 1, 1, 2, 1, 2, 2, 2, 2, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 2, 2, 1, 2, 0, 0, 1, 2, 2, 1, 2, 1, 2, },
		{ 2, 1, 1, 2, 1, 2, 2, 2, 0, 1, 1, 2, 2, 2, 2, 0, 0, 2, 2, 0, 2, 2, 0, 2, 0, 1, 2, 2, 1, 2, 2, 0, },
		{ 2, 2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 2, 2, 2, 0, 0, 2, 2, 0, 2, 2, 2, 2, 2, 0, 2, 2, 1, 2, 2, 2, 0, },
	};

	PalettedImage sample
	{
		.data = image,
		.palette = { {}, {}, {} }
	};

	std::vector<PatternOccurrence> patterns = 
	{
		{
			.pattern =
			{
				{ 0, 0, },
				{ 0, 1, },
			},
			.occurrence =
			{
				{ 3, 4, 0, 4, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 0, 0, },
				{ 1, 1, },
			},
			.occurrence =
			{
				{ 94, 37, 3, 1, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 0, 0, },
				{ 1, 2, },
			},
			.occurrence =
			{
				{ 6, 5, 5, 6, },
				{ 7, 5, 9, 2, },
			}
		},
		{
			.pattern =
			{
				{ 0, 0, },
				{ 0, 2, },
			},
			.occurrence =
			{
				{ 6, 13, 22, 5, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 1, 1, },
				{ 1, 1, },
			},
			.occurrence =
			{
				{ 170, 0, 0, 0, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 1, 2, },
				{ 1, 1, },
			},
			.occurrence =
			{
				{ 20, 20, 39, 62, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 2, 1, },
				{ 1, 2, },
			},
			.occurrence =
			{
				{ 27, 27, 0, 0, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 1, 1, },
				{ 2, 2, },
			},
			.occurrence =
			{
				{ 38, 18, 8, 4, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 2, 2, },
				{ 2, 2, },
			},
			.occurrence =
			{
				{ 78, 0, 0, 0, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 2, 0, },
				{ 2, 0, },
			},
			.occurrence =
			{
				{ 37, 5, 4, 74, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 1, 2, },
				{ 2, 2, },
			},
			.occurrence =
			{
				{ 52, 18, 15, 33, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 2, 2, },
				{ 2, 0, },
			},
			.occurrence =
			{
				{ 11, 1, 3, 6, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 1, 2, },
				{ 2, 0, },
			},
			.occurrence =
			{
				{ 4, 0, 0, 1, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 2, 1, },
				{ 0, 1, },
			},
			.occurrence =
			{
				{ 1, 0, 0, 0, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 2, 0, },
				{ 0, 2, },
			},
			.occurrence =
			{
				{ 3, 1, 0, 0, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 0, 0, },
				{ 0, 0, },
			},
			.occurrence =
			{
				{ 5, 0, 0, 0, },
				{ 0, 0, 0, 0, },
			}
		},
		{
			.pattern =
			{
				{ 2, 1, },
				{ 2, 0, },
			},
			.occurrence =
			{
				{ 1, 0, 0, 0, },
				{ 1, 0, 0, 0, },
			}
		}

	};

	Array2D<PatternIdentifier> grid = 
	{
		{ 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 1}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 1}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {3, 0}
			} 
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {3, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 12 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 13 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 1}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 14 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 14 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}
		},
		{ 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 1}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 1}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 1}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 1}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 1}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 15 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 1}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {3, 1}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}
		},
		{ 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}
		}, 
		{ 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 1}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 1}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}
		},
		{ 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 1}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 1}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {3, 0}
			}
		},
		{ 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {3, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {3, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 1}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 1}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 15 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 1}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 15 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 15 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}
		},
		{ 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 1}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {0, 1}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {3, 1}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}
		},
		{ 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}
		},
		{ 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {1, 0}
			}
		},
		{ 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}
		},
		{ 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 4 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 12 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 12 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 14 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 12 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 12 ,
				.enumeratedTransform = {3, 0}
			}
		},
		{ 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 7 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 16 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 16 ,
				.enumeratedTransform = {0, 1}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 1 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 5 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 14 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 11 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {1, 1}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 6 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 10 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 8 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {2, 0}
			}
		},
		{ 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 0 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 1}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 15 ,
				.enumeratedTransform = {0, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 2 ,
				.enumeratedTransform = {2, 1}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 9 ,
				.enumeratedTransform = {3, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {2, 0}
			}, 
			{
				.patternIndex = 3 ,
				.enumeratedTransform = {1, 0}
			}
		}
	};

	ImagePatternProperties expectedProperties{ .patterns = patterns, .grid = grid };

	ImagePatternProperties properties = extractPatternsFromImage(sample, n);

	ASSERT_TRUE(imagePropertiesEquivalent(properties, expectedProperties));
	ASSERT_TRUE(imageGridEquivalent(properties, expectedProperties));

}
