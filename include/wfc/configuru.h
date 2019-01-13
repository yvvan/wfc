#ifndef _WFC_CONFIGURU_H_
#define _WFC_CONFIGURU_H_

#include <functional>
#include <unordered_set>
#include <experimental/optional>

// emilib
#include <configuru.hpp>
#include <emilib/irange.hpp>

#include <wfc/overlapping_model.h>
#include <wfc/tile_model.h>
#include <wfc/imodel.h>

using TileLoader = std::function<Tile(const std::string& tile_name)>;

using namespace emilib;

const size_t MAX_COLORS = 1 << (sizeof(ColorIndex) * 8);

PalettedImage load_paletted_image(const std::string& path);

OverlappingModelConfig extractOverlappingConfig(const std::string& image_dir, const configuru::Config& config);

Tile loadTile(const std::string& subdir, const std::string& image_dir, const std::string& tile_name);

TileModelConfig extractConfig(const std::string& image_dir, const configuru::Config& topConfig);

std::vector<UniqueTile> loadUnique(const configuru::Config& config, const TileLoader& tileLoader, const std::unordered_set<std::string>& subset, size_t tileSize);

std::vector<CopiedTile> loadCopied(const configuru::Config& config, const TileLoader& tileLoader, const std::unordered_set<std::string>& subset, size_t tileSize);

struct GeneralConfig
{
	std::experimental::optional<size_t> limit;
	size_t numOutput;

	const std::string name;
};

struct ConfigActions
{

	std::function<void(const GeneralConfig&, const OverlappingModelConfig&)> overlappingAction;

	std::function<void(const GeneralConfig&, const TileModelConfig&)> tileAction;

};

void run_config_file(const std::string& path, ConfigActions actions);

#endif
