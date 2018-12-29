#include <wfc/overlapping_model.h>

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

RGBA collapsePixel(const std::vector<ColorIndex>& tile_contributors, const Palette& palette)
{
	if (tile_contributors.empty()) 
	{
		// No contributors, so set to 0
		return {0, 0, 0, 255};
	} 
	else if (tile_contributors.size() == 1) 
	{
		// One contributor, so use that
		return palette[tile_contributors[0]];
	} 
	else 
	{
		// Multiple contributors, so average them
		return averageContributors(tile_contributors, palette);
	}
}

RGBA averageContributors(const std::vector<ColorIndex>& contributors, const Palette& palette)
{
	size_t r = 0;
	size_t g = 0;
	size_t b = 0;
	size_t a = 0;
	for (const auto tile : contributors) 
	{
		r += palette[tile].r;
		g += palette[tile].g;
		b += palette[tile].b;
		a += palette[tile].a;
	}
	r /= contributors.size();
	g /= contributors.size();
	b /= contributors.size();
	a /= contributors.size();
	return {(uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a};
}

Image image_from_graphics(const Graphics& graphics, const Palette& palette)
{
	Image result(graphics.size(), {0, 0, 0, 0});

	auto rangeFcn = [&] (const Index2D& index)
	{
		const auto& tile_contributors = graphics[index];
		result[index] = collapsePixel(tile_contributors, palette);
	};

	runForDimension(graphics.size(), rangeFcn);

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

bool agrees(const Pattern& p1, const Pattern& p2, int dx, int dy, int n) 
{
	int xmin = dx < 0 ? 0 : dx, xmax = dx < 0 ? dx + n : n;
	int ymin = dy < 0 ? 0 : dy, ymax = dy < 0 ? dy + n : n;
	for (int y = ymin; y < ymax; ++y) 
	{
		for (int x = xmin; x < xmax; ++x) 
		{
			if (p1[{ x, y }] != p2[{ (x - dx), (y - dy) }]) 
			{
				return false;
			}
		}
	}
	return true;
};

OverlappingModel::OverlappingModel(OverlappingModelConfig config)
{
	LOG_F(INFO, "palette size: %lu", config.sample_image.palette.size());

	PatternHash foundation = kInvalidHash;
	PatternHash* foundationPtr = (config.has_foundation) ? &foundation : nullptr;
	const auto hashed_patterns = extract_patterns(config.sample_image, config.n, config.periodic_in, config.symmetry, foundationPtr);

	LOG_F(INFO, "Found %lu unique patterns in sample image", hashed_patterns.size());

	mCommonParams.mOutsideCommonParams = config.commonParam;
	mCommonParams._num_patterns = hashed_patterns.size();
	_n            = config.n;
	_palette      = config.sample_image.palette;

	mCommonParams._foundation = std::experimental::optional<size_t>();
	for (const auto& it : hashed_patterns) 
	{
		if (it.first == foundation) 
		{
			mCommonParams._foundation = _patterns.size();
		}

		_patterns.push_back(pattern_from_hash(it.first, config.n, _palette.size()));
		mCommonParams._pattern_weight.push_back(it.second);
	}

	_propagator = Array3D<std::vector<PatternIndex>>({ mCommonParams._num_patterns, 2 * config.n - 1, 2 * config.n - 1}, {});

	// These are just used for printouts
	size_t longest_propagator = 0;
	size_t sum_propagator = 0;

	for (auto t : irange(mCommonParams._num_patterns)) 
	{
		for (auto x : irange<int>(2 * config.n - 1)) 
		{
			for (auto y : irange<int>(2 * config.n - 1)) 
			{
				Index3D index3D{ t, x, y };

				auto& list = _propagator[index3D];
				for (auto t2 : irange(mCommonParams._num_patterns)) 
				{
					if (agrees(_patterns[t], _patterns[t2], x - config.n + 1, y - config.n + 1, config.n)) 
					{
						list.push_back(t2);
					}
				}

				longest_propagator = std::max(longest_propagator, list.size());
				sum_propagator += list.size();
			}
		}
	}

	LOG_F(INFO, "propagator length: mean/max/sum: %.1f, %lu, %lu",
	    (double)sum_propagator / _propagator.volume(), longest_propagator, sum_propagator);
}

bool OverlappingModel::propagate(Output& output) const
{
	bool did_change = false;
	Dimension2D dimension = mCommonParams.mOutsideCommonParams.dimension;

	// This whole set of nested loops looks very similar to the one in graphics()
	auto rangeFcn = [&] (const Index2D& index)
	{
		if (!output._changes[index])
		{
			return; 
		}

		output._changes[index] = false;

		int rangeLimit = _n - 1;

		SquareRange range
		{
			.bottomLeft = { -rangeLimit, -rangeLimit },
			.upperRight = { rangeLimit, rangeLimit }
		};

		auto rangeIterator = range2D(range);

		auto rangeFcn = [&] (const Offset2D& offset)
		{
			auto sx = (int)index.x + (int)offset.x;
			auto sy = (int)index.y + (int)offset.y;

			// Do wrap around (always-positive modulus)
			if      (sx <  0)      { sx += dimension.width; }
			else if (sx >= dimension.width) { sx -= dimension.width; }

			if      (sy <  0)       { sy += dimension.height; }
			else if (sy >= dimension.height) { sy -= dimension.height; }

			Index2D sIndex{ sx, sy };

			if (on_boundary(sIndex)) 
			{
				return;
			}

			for (int t2 = 0; t2 < mCommonParams._num_patterns; ++t2) 
			{
				Index3D sPatternIndex = append(sIndex, t2);
				if (!output._wave[sPatternIndex])
				{
					continue;
				}

				// This part below seems to be the only thing fundamentally diff from graphics() algorithm:

				bool can_pattern_fit = false;

				Index3D shiftedIndex{ t2, rangeLimit - offset.x, rangeLimit - offset.y };
				const auto& prop = _propagator[shiftedIndex];
				for (const auto& t3 : prop) 
				{
					if (output._wave[append(index, t3)]) 
					{
						can_pattern_fit = true;
						break;
					}
				}

				if (!can_pattern_fit) 
				{
					output._changes[sIndex] = true;
					output._wave[sPatternIndex] = false;
					did_change = true;
				}
			}
		};

		rangeIterator(rangeFcn);
	};

	runForDimension(dimension, rangeFcn);

	return did_change;
}

Graphics OverlappingModel::graphics(const Output& output) const
{
	Dimension2D dimension = mCommonParams.mOutsideCommonParams.dimension;

	Graphics result(dimension, {});


	auto rangeFcn = [&] (const Index2D& index)
	{
		auto& tile_contributors = result[index];

		for (int dy = 0; dy < _n; ++dy) 
		{
			for (int dx = 0; dx < _n; ++dx) 
			{
				int sx = index.x - dx;
				if (sx < 0) sx += dimension.width;

				int sy = index.y - dy;
				if (sy < 0) sy += dimension.height;

				if (on_boundary({ sx, sy })) { continue; }

				for (int t = 0; t < mCommonParams._num_patterns; ++t) 
				{
					Index3D index3D{ sx, sy, t };
					if (output._wave[index3D]) 
					{
						tile_contributors.push_back(_patterns[t][{ dx, dy }]);
					}
				}
			}
		}
	};

	runForDimension(dimension, rangeFcn);

	return result;
}

Image OverlappingModel::image(const Output& output) const
{
	return upsample(image_from_graphics(graphics(output), _palette));
}

Image upsample(const Image& image)
{
	Image result({ image.size().width * kUpscale, image.size().height * kUpscale }, {});

	auto rangeFcn = [&] (const Index2D& index)
	{
		Index2D scaled
		{
			index.x / kUpscale, 
			index.y / kUpscale 
		};

		result[index] = image[scaled];
	};

	runForDimension(result.size(), rangeFcn);

	return result;
}

// n = side of the pattern, e.g. 3.
PatternPrevalence extract_patterns(const PalettedImage& sample, int n, bool periodic_in, size_t symmetry, PatternHash* out_lowest_pattern)
{
	Dimension2D imageDimension = sample.data.size();

	CHECK_LE_F(n, imageDimension.width);
	CHECK_LE_F(n, imageDimension.height);

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

std::array<Pattern, 8> generatePatterns(const PalettedImage& sample, int n, const Index2D& index)
{
	std::array<Pattern, 8> toReturn;
	toReturn[0] = patternFromSample(sample, n, index);
	toReturn[1] = reflect(toReturn[0], n);
	toReturn[2] = rotate(toReturn[0], n);
	toReturn[3] = reflect(toReturn[2], n);
	toReturn[4] = rotate(toReturn[2], n);
	toReturn[5] = reflect(toReturn[4], n);
	toReturn[6] = rotate(toReturn[4], n);
	toReturn[7] = reflect(toReturn[6], n);
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

Index2D wrapAroundIndex(const Index2D& index, const Dimension2D& dimension)
{
	return { (index.x % dimension.width), (index.y % dimension.height) };
}
