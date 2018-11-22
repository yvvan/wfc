#ifndef _WFC_TILE_MODEL_H_
#define _WFC_TILE_MODEL_H_

#include <wfc/imodel.h>
#include <wfc/arrays.h>
#include <wfc/rgba.h>

#include <vector>

struct TileModelInternal
{
	CommonParams mCommonParams;

	Array3D<Bool>                  _propagator; // 4 X _num_patterns X _num_patterns
	std::vector<std::vector<RGBA>> _tiles;
	size_t                         _tile_size;
};

class TileModel : public Model
{
public:
	TileModel(TileModelInternal internal);

	bool propagate(Output* output) const override;

	bool on_boundary(int x, int y) const override
	{
		return false;
	}

	Image image(const Output& output) const override;

private:
	TileModelInternal mInternal;
};

#endif
