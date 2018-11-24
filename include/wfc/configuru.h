#ifndef _WFC_CONFIGURU_H_
#define _WFC_CONFIGURU_H_

#include <functional>

// emilib
#include <configuru.hpp>
#include <emilib/irange.hpp>

#include <wfc/overlapping_model.h>
#include <wfc/tile_model.h>
#include <wfc/imodel.h>
#include <wfc/common.h>

using Tile = std::vector<RGBA>;
using TileLoader = std::function<Tile(const std::string& tile_name)>;

using namespace emilib;

const size_t MAX_COLORS = 1 << (sizeof(ColorIndex) * 8);

Tile rotate(const Tile& in_tile, const size_t tile_size);

struct TileModelConfig
{

	const configuru::Config config; 
	
	std::string subset_name;
	
	OutsideCommonParams commonParam;
	
	const TileLoader tile_loader;

};

TileModelInternal fromConfig(const TileModelConfig& config);

PalettedImage load_paletted_image(const std::string& path);

OverlappingModelConfig extractOverlappingConfig(const std::string& image_dir, const configuru::Config& config);

Tile loadTile(const std::string& subdir, const std::string& image_dir, const std::string& tile_name);

TileModelConfig extractConfig(const std::string& image_dir, const configuru::Config& topConfig);

struct GeneralConfig
{
	size_t limit;
	size_t screenshots;

	const std::string name;
};

struct ConfigActions
{

	std::function<void(const GeneralConfig&, const OverlappingModelConfig&)> overlappingAction;

	std::function<void(const GeneralConfig&, const TileModelInternal&)> tileAction;

};

void run_config_file(const std::string& path, ConfigActions actions);

#endif
