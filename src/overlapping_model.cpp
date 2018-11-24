#include <wfc/overlapping_model.h>

#include <cmath>

#include <emilib/irange.hpp>

using namespace emilib;

template <class Functor>
Pattern make_pattern(int n, Functor fun)
{
	Pattern result(n * n);
	for (auto dy : irange(n)) 
	{
		for (auto dx : irange(n)) 
		{
			result[dy * n + dx] = fun(dx, dy);
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
	Image result(graphics.size().width, graphics.size().height, {0, 0, 0, 0});

	for (const auto y : irange(graphics.size().height)) 
	{
		for (const auto x : irange(graphics.size().width)) 
		{
			const auto& tile_contributors = graphics.ref(x, y);
			result.ref(x, y) = collapsePixel(tile_contributors, palette);
		}
	}

	return result;
}


Pattern pattern_from_hash(const PatternHash hash, int n, size_t palette_size)
{
	size_t residue = hash;
	size_t power = std::pow(palette_size, n * n);
	Pattern result(n * n);

	for (size_t i = 0; i < result.size(); ++i)
	{
		power /= palette_size;
		size_t count = 0;

		while (residue >= power)
		{
			residue -= power;
			count++;
		}

		result[i] = static_cast<ColorIndex>(count);
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

	mCommonParams._foundation = kInvalidIndex;
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
				if (p1[x + config.n * y] != p2[x - dx + config.n * (y - dy)]) 
				{
					return false;
				}
			}
		}
		return true;
	};

	_propagator = Array3D<std::vector<PatternIndex>>(mCommonParams._num_patterns, 2 * config.n - 1, 2 * config.n - 1, {});

	// These are just used for printouts
	size_t longest_propagator = 0;
	size_t sum_propagator = 0;

	for (auto t : irange(mCommonParams._num_patterns)) 
	{
		for (auto x : irange<int>(2 * config.n - 1)) 
		{
			for (auto y : irange<int>(2 * config.n - 1)) 
			{
				auto& list = _propagator.ref(t, x, y);
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
	    (double)sum_propagator / _propagator.size(), longest_propagator, sum_propagator);
}

bool OverlappingModel::propagate(Output& output) const
{
	bool did_change = false;

	for (int x1 = 0; x1 < mCommonParams.mOutsideCommonParams._width; ++x1) 
	{
		for (int y1 = 0; y1 < mCommonParams.mOutsideCommonParams._height; ++y1) 
		{
			if (!output._changes.ref(x1, y1)) { continue; }
			output._changes.ref(x1, y1) = false;

			for (int dx = -_n + 1; dx < _n; ++dx) 
			{
				for (int dy = -_n + 1; dy < _n; ++dy) 
				{
					auto x2 = x1 + dx;
					auto y2 = y1 + dy;

					auto sx = x2;
					if      (sx <  0)      { sx += mCommonParams.mOutsideCommonParams._width; }
					else if (sx >= mCommonParams.mOutsideCommonParams._width) { sx -= mCommonParams.mOutsideCommonParams._width; }

					auto sy = y2;
					if      (sy <  0)       { sy += mCommonParams.mOutsideCommonParams._height; }
					else if (sy >= mCommonParams.mOutsideCommonParams._height) { sy -= mCommonParams.mOutsideCommonParams._height; }

					if (!mCommonParams.mOutsideCommonParams._periodic_out && (sx + _n > mCommonParams.mOutsideCommonParams._width || sy + _n > mCommonParams.mOutsideCommonParams._height)) 
					{
						continue;
					}

					for (int t2 = 0; t2 < mCommonParams._num_patterns; ++t2) 
					{
						if (!output._wave.ref(sx, sy, t2)) { continue; }

						bool can_pattern_fit = false;

						const auto& prop = _propagator.ref(t2, _n - 1 - dx, _n - 1 - dy);
						for (const auto& t3 : prop) 
						{
							if (output._wave.ref(x1, y1, t3)) 
							{
								can_pattern_fit = true;
								break;
							}
						}

						if (!can_pattern_fit) 
						{
							output._changes.ref(sx, sy) = true;
							output._wave.ref(sx, sy, t2) = false;
							did_change = true;
						}
					}
				}
			}
		}
	}

	return did_change;
}

Graphics OverlappingModel::graphics(const Output& output) const
{
	Graphics result(mCommonParams.mOutsideCommonParams._width, mCommonParams.mOutsideCommonParams._height, {});
	for (const auto y : irange(mCommonParams.mOutsideCommonParams._height)) 
	{
		for (const auto x : irange(mCommonParams.mOutsideCommonParams._width)) 
		{
			auto& tile_contributors = result.ref(x, y);

			for (int dy = 0; dy < _n; ++dy) 
			{
				for (int dx = 0; dx < _n; ++dx) 
				{
					int sx = x - dx;
					if (sx < 0) sx += mCommonParams.mOutsideCommonParams._width;

					int sy = y - dy;
					if (sy < 0) sy += mCommonParams.mOutsideCommonParams._height;

					if (on_boundary(sx, sy)) { continue; }

					for (int t = 0; t < mCommonParams._num_patterns; ++t) 
					{
						if (output._wave.ref(sx, sy, t)) 
						{
							tile_contributors.push_back(_patterns[t][dx + dy * _n]);
						}
					}
				}
			}
		}
	}
	return result;
}

Image OverlappingModel::image(const Output& output) const
{
	return upsample(image_from_graphics(graphics(output), _palette));
}

Image upsample(const Image& image)
{
	Image result(image.size().width * kUpscale, image.size().height * kUpscale, {});
	for (const auto y : irange(result.size().height)) 
	{
		for (const auto x : irange(result.size().width)) 
		{
			result.ref(x, y) = image.ref(x / kUpscale, y / kUpscale);
		}
	}
	return result;
}

// n = side of the pattern, e.g. 3.
PatternPrevalence extract_patterns(const PalettedImage& sample, int n, bool periodic_in, size_t symmetry, PatternHash* out_lowest_pattern)
{
	CHECK_LE_F(n, sample.width);
	CHECK_LE_F(n, sample.height);

	PatternPrevalence patterns;

	for (size_t y : irange(periodic_in ? sample.height : sample.height - n + 1)) 
	{
		for (size_t x : irange(periodic_in ? sample.width : sample.width - n + 1)) 
		{
			std::array<Pattern, 8> ps;
			ps[0] = patternFromSample(sample, n, x, y);
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
				if (out_lowest_pattern && y == sample.height - 1) 
				{
					*out_lowest_pattern = hash;
				}
			}
		}
	}

	return patterns;
}

Pattern patternFromSample(const PalettedImage& sample, int n, size_t x, size_t y)
{
	auto functor = [&] (size_t dx, size_t dy)
	{
		return sample.at_wrapped(x + dx, y + dy);
	};
	return make_pattern(n, functor);
}

Pattern rotate(const Pattern& p, int n)
{
	auto functor = [&](size_t x, size_t y)
	{ 
		return p[n - 1 - y + x * n]; 
	};
	return make_pattern(n, functor);
}

Pattern reflect(const Pattern& p, int n)
{
	auto functor = [&](size_t x, size_t y)
	{ 
		return p[n - 1 - x + y * n];
	};
	return make_pattern(n, functor);
}

PatternHash hash_from_pattern(const Pattern& pattern, size_t palette_size)
{
	CHECK_LT_F(std::pow((double)palette_size, (double)pattern.size()),
	           std::pow(2.0, sizeof(PatternHash) * 8),
	           "Too large palette (it is %lu) or too large pattern size (it's %.0f)",
	           palette_size, std::sqrt(pattern.size()));

	PatternHash result = 0;
	size_t power = 1;
	for (const auto i : irange(pattern.size()))
	{
		result += pattern[pattern.size() - 1 - i] * power;
		power *= palette_size;
	}
	return result;
}
