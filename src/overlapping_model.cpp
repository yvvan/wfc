#include <wfc/overlapping_model.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>

#include <emilib/irange.hpp>

using namespace emilib;

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
	mCommonParams.mOutsideCommonParams = config.commonParam;
	mCommonParams._num_patterns = config.hashed_patterns.size();
	_n            = config.n;
	_palette      = config.palette;

	mCommonParams._foundation = kInvalidIndex;
	for (const auto& it : config.hashed_patterns) 
	{
		if (it.first == config.foundation_pattern) 
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
				list.shrink_to_fit();
				longest_propagator = std::max(longest_propagator, list.size());
				sum_propagator += list.size();
			}
		}
	}

	LOG_F(INFO, "propagator length: mean/max/sum: %.1f, %lu, %lu",
	    (double)sum_propagator / _propagator.size(), longest_propagator, sum_propagator);
}

bool OverlappingModel::propagate(Output* output) const
{
	bool did_change = false;

	for (int x1 = 0; x1 < mCommonParams.mOutsideCommonParams._width; ++x1) 
	{
		for (int y1 = 0; y1 < mCommonParams.mOutsideCommonParams._height; ++y1) 
		{
			if (!output->_changes.ref(x1, y1)) { continue; }
			output->_changes.ref(x1, y1) = false;

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
						if (!output->_wave.ref(sx, sy, t2)) { continue; }

						bool can_pattern_fit = false;

						const auto& prop = _propagator.ref(t2, _n - 1 - dx, _n - 1 - dy);
						for (const auto& t3 : prop) 
						{
							if (output->_wave.ref(x1, y1, t3)) 
							{
								can_pattern_fit = true;
								break;
							}
						}

						if (!can_pattern_fit) 
						{
							output->_changes.ref(sx, sy) = true;
							output->_wave.ref(sx, sy, t2) = false;
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
			auto& tile_constributors = result.ref(x, y);

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
							tile_constributors.push_back(_patterns[t][dx + dy * _n]);
						}
					}
				}
			}
		}
	}
	return result;
}
