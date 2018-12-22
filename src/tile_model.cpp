#include <wfc/tile_model.h>

#include <emilib/irange.hpp>

using namespace emilib;

TileModel::TileModel(const TileModelInternal& internal) :
	mInternal(internal)
{
	// Needed because other functions access from base class
	// TODO: Remove
	mCommonParams = mInternal.mCommonParams;
}

bool TileModel::on_boundary(const Index2D& index) const
{
	return false;
}

bool TileModel::propagate(Output& output) const
{
	bool did_change = false;

	Dimension2D dimension = mCommonParams.mOutsideCommonParams.dimension;

	for (int x2 = 0; x2 < dimension.width; ++x2) 
	{
		for (int y2 = 0; y2 < dimension.height; ++y2) 
		{
			for (int d = 0; d < 4; ++d) 
			{
				int x1 = x2, y1 = y2;

				// Looks like this might be wrap-around algorithm? 
				if (d == 0) 
				{
					if (x2 == 0) 
					{
						if (!mCommonParams.mOutsideCommonParams._periodic_out) { continue; }
						x1 = dimension.width - 1;
					} 
					else 
					{
						x1 = x2 - 1;
					}
				} 
				else if (d == 1) 
				{
					if (y2 == dimension.height - 1) 
					{
						if (!mCommonParams.mOutsideCommonParams._periodic_out) { continue; }
						y1 = 0;
					}
					else 
					{
						y1 = y2 + 1;
					}
				} 
				else if (d == 2) 
				{
					if (x2 == dimension.width - 1) 
					{
						if (!mCommonParams.mOutsideCommonParams._periodic_out) { continue; }
						x1 = 0;
					} 
					else 
					{
						x1 = x2 + 1;
					}
				}
				else 
				{
					if (y2 == 0) 
					{
						if (!mCommonParams.mOutsideCommonParams._periodic_out) { continue; }
						y1 = dimension.height - 1;
					} 
					else 
					{
						y1 = y2 - 1;
					}
				}

				if (!output._changes[ {x1, y1 } ]) { continue; }

				for (int t2 = 0; t2 < mCommonParams._num_patterns; ++t2) 
				{
					if (output._wave[ { x2, y2, t2} ]) 
					{
						bool b = false;
						for (int t1 = 0; t1 < mCommonParams._num_patterns && !b; ++t1) 
						{
							if (output._wave[ { x1, y1, t1 } ]) 
							{
								b = mInternal._propagator[ { d, t1, t2 } ];
							}
						}
						if (!b) 
						{
							output._wave[ { x2, y2, t2 } ] = false;
							output._changes[ { x2, y2 } ] = true;
							did_change = true;
						}
					}
				}
			}
		}
	}

	return did_change;
}

Image TileModel::image(const Output& output) const
{
	Dimension2D dimension = mCommonParams.mOutsideCommonParams.dimension;
	Image result({ dimension.width * mInternal._tile_size, dimension.height * mInternal._tile_size } , {});

	for (int x = 0; x < dimension.width; ++x) 
	{
		for (int y = 0; y < dimension.height; ++y) 
		{
			double sum = 0;
			for (const auto t : irange(mCommonParams._num_patterns)) 
			{
				if (output._wave[ { x, y, t } ] ) 
				{
					sum += mCommonParams._pattern_weight[t];
				}
			}

			for (int yt = 0; yt < mInternal._tile_size; ++yt) 
			{
				for (int xt = 0; xt < mInternal._tile_size; ++xt) 
				{
					if (sum == 0) 
					{
						result[ { x * mInternal._tile_size + xt, y * mInternal._tile_size + yt } ]  = RGBA{0, 0, 0, 255};
					} 
					else 
					{
						double r = 0, g = 0, b = 0, a = 0;
						for (int t = 0; t < mCommonParams._num_patterns; ++t) 
						{
							if (output._wave[ { x, y, t } ]) 
							{
								RGBA c = mInternal._tiles[t][xt + yt * mInternal._tile_size];
								r += (double)c.r * mCommonParams._pattern_weight[t] / sum;
								g += (double)c.g * mCommonParams._pattern_weight[t] / sum;
								b += (double)c.b * mCommonParams._pattern_weight[t] / sum;
								a += (double)c.a * mCommonParams._pattern_weight[t] / sum;
							}
						}

						result[ { x * mInternal._tile_size + xt, y * mInternal._tile_size + yt } ] =
						           RGBA{(uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a};
					}
				}
			}
		}
	}

	return result;
}
