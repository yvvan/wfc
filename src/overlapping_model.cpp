#include <wfc/overlapping_model.h>

#include <cmath>
#include <iostream>

#include <emilib/irange.hpp>

#include <wfc/ranges.h>

using namespace emilib;

template <class Functor>
Pattern make_pattern(int n, Functor fun)
{
	Pattern result({ n, n });
	for (auto dy : irange(n)) 
	{
		for (auto dx : irange(n)) 
		{
			result[{dx, dy}] = fun({ dx, dy });
		}
	}
	return result;
};

RGBA collapsePixel(const std::vector<ColorIndex>& tile_contributors, const Palette& palette)
{
	RGBA toReturn;

	if (tile_contributors.empty()) 
	{
		// No contributors, so set to 0
		toReturn = {0, 0, 0, 255};
	} 
	else if (tile_contributors.size() == 1) 
	{
		// One contributor, so use that
		toReturn = palette[tile_contributors[0]];
	} 
	else 
	{
		// Multiple contributors, so average them
		size_t r = 0;
		size_t g = 0;
		size_t b = 0;
		size_t a = 0;
		for (const auto tile : tile_contributors) 
		{
			r += palette[tile].r;
			g += palette[tile].g;
			b += palette[tile].b;
			a += palette[tile].a;
		}
		r /= tile_contributors.size();
		g /= tile_contributors.size();
		b /= tile_contributors.size();
		a /= tile_contributors.size();
		toReturn = {(uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a};
	}

	return toReturn;
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

OverlappingModel::OverlappingModel(OverlappingModelConfig config)
{
	LOG_F(INFO, "palette size: %lu", config.sample_image.palette.size());

	PatternHash foundation = kInvalidHash;
	const auto hashed_patterns = extract_patterns(config.sample_image, config.n, config.periodic_in, config.symmetry, config.has_foundation ? &foundation : nullptr);
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

	const auto agrees = [&](const Pattern& p1, const Pattern& p2, int dx, int dy) 
	{
		int xmin = dx < 0 ? 0 : dx, xmax = dx < 0 ? dx + config.n : config.n;
		int ymin = dy < 0 ? 0 : dy, ymax = dy < 0 ? dy + config.n : config.n;
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
					if (agrees(_patterns[t], _patterns[t2], x - config.n + 1, y - config.n + 1)) 
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
	Dimension2D dimension{mCommonParams.mOutsideCommonParams._width, mCommonParams.mOutsideCommonParams._height};

	// This whole set of nested loops looks very similar to the one in graphics()
	auto rangeFcn = [&] (const Index2D& index)
	{
		int x1 = index.x;
		int y1 = index.y;

		if (!output._changes[index])
		{
			return; 
		}

		output._changes[index] = false;

		for (int dx = -_n + 1; dx < _n; ++dx) 
		{
			for (int dy = -_n + 1; dy < _n; ++dy) 
			{
				auto x2 = x1 + dx;
				auto y2 = y1 + dy;

				// Do wrap around
				auto sx = x2;
				if      (sx <  0)      { sx += dimension.width; }
				else if (sx >= dimension.width) { sx -= dimension.width; }

				auto sy = y2;
				if      (sy <  0)       { sy += dimension.height; }
				else if (sy >= dimension.height) { sy -= dimension.height; }

				// Same as on_boundary?
				if (!mCommonParams.mOutsideCommonParams._periodic_out && (sx + _n > dimension.width || sy + _n > dimension.height)) 
				{
					continue;
				}

				Index2D sIndex{ sx, sy };

				for (int t2 = 0; t2 < mCommonParams._num_patterns; ++t2) 
				{
					Index3D sPatternIndex = append(sIndex, t2);
					if (!output._wave[sPatternIndex])
					{
						continue;
					}

					// This part below seems to be the only thing fundamentally diff from graphics() algorithm:

					bool can_pattern_fit = false;

					Index3D shiftedIndex { t2, _n - 1 - dx, _n - 1 - dy };
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
			}
		}
	};

	runForDimension(dimension, rangeFcn);

	return did_change;
}

Graphics OverlappingModel::graphics(const Output& output) const
{
	Dimension2D dimension{mCommonParams.mOutsideCommonParams._width, mCommonParams.mOutsideCommonParams._height};

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

				if (on_boundary(sx, sy)) { continue; }

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
	CHECK_LE_F(n, sample.width);
	CHECK_LE_F(n, sample.height);

	PatternPrevalence patterns;

	Dimension2D dimension
	{
		.width = periodic_in ? sample.width : sample.width - n + 1,
		.height = periodic_in ? sample.height : sample.height - n + 1
	};

	auto rangeFcn = [&] (const Index2D& index)
	{
		std::array<Pattern, 8> ps;
		ps[0] = patternFromSample(sample, n, index);
		ps[1] = reflect(ps[0], n);
		ps[2] = rotate(ps[0], n);
		ps[3] = reflect(ps[2], n);
		ps[4] = rotate(ps[2], n);
		ps[5] = reflect(ps[4], n);
		ps[6] = rotate(ps[4], n);
		ps[7] = reflect(ps[6], n);

		for (int k = 0; k < symmetry; ++k) 
		{
			auto hash = hash_from_pattern(ps[k], sample.palette.size());
			patterns[hash] += 1;
			if (out_lowest_pattern && index.y == sample.height - 1) 
			{
				*out_lowest_pattern = hash;
			}
		}
	};

	runForDimension(dimension, rangeFcn);

	return patterns;
}

Pattern patternFromSample(const PalettedImage& sample, int n, const Index2D& index)
{
	auto functor = [&] (size_t dx, size_t dy)
	{
		return sample.at_wrapped(index.x + dx, index.y + dy);
	};
	return make_pattern(n, functor);
}

Pattern rotate(const Pattern& p, int n)
{
	auto functor = [&](size_t x, size_t y)
	{ 
		return p[{ (n - 1 - y), x }]; 
	};
	return make_pattern(n, functor);
}

Pattern reflect(const Pattern& p, int n)
{
	auto functor = [&](size_t x, size_t y)
	{ 
		return p[{ (n - 1 - x), y }];
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
