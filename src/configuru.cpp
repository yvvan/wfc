#include <wfc/configuru.h>

#include <stb_image.h>

#include <algorithm>
#include <sstream>
#include <unordered_set>

std::vector<Neighbors> loadNeighbors(const configuru::Config &config) {
  std::vector<Neighbors> toReturn;
  for (const auto &neighbor : config["neighbors"].as_array()) {
    const auto left = neighbor["left"];
    const auto right = neighbor["right"];
//    CHECK_EQ_F(left.array_size(), 2u);
//    CHECK_EQ_F(right.array_size(), 2u);

    Neighbors newNeighbors = {{
                                  left[0].as_string(),
                                  left[1].get<int>(),
                              },
                              {
                                  right[0].as_string(),
                                  right[1].get<int>(),
                              }};
    toReturn.push_back(newNeighbors);
  }
  return toReturn;
}

Symmetry readSymmetry(const configuru::Config &tile) {
  std::string sym = tile.get_or("symmetry", "X");
  if (sym == "L") {
    return Symmetry::L;
  } else if (sym == "T") {
    return Symmetry::T;
  } else if (sym == "I") {
    return Symmetry::I;
  } else if (sym == "\\") {
    return Symmetry::S;
  } else if (sym == "X") {
    return Symmetry::X;
  } else {
    return {};
  }
}

template <class Functor>
void forTileData(const configuru::Config &config,
                 const std::unordered_set<std::string> &subset,
                 Functor functor) {
  for (const configuru::Config &tile : config["tiles"].as_array()) {
    const std::string tile_name = tile["name"].as_string();

    if (!subset.empty() && subset.count(tile_name) == 0) {
      continue;
    }

    // Seems symmetry can be deduced based on tile and need to be explicitly
    // specified in a .cfg file
    Symmetry symmetry = readSymmetry(tile);

    if (symmetry == Symmetry::None) {
      continue;
    }

    double weight = tile.get_or("weight", 1.0);

    functor(tile_name, symmetry, weight);
  }
}

std::vector<CopiedTile>
loadCopied(const configuru::Config &config, const TileLoader &tileLoader,
           const std::unordered_set<std::string> &subset, size_t tileSize) {
  std::vector<CopiedTile> toReturn;

  auto functor = [&](const std::string &tile_name, Symmetry symmetry,
                     double weight) {
    // Load once, then rotate the reqd number of times
    const Tile bitmap = tileLoader(tile_name);
    //CHECK_EQ_F(bitmap.size(), tileSize * tileSize);

    CopiedTile nextTile = {tile_name, symmetry, bitmap, weight};

    toReturn.push_back(nextTile);
  };

  forTileData(config, subset, functor);

  return toReturn;
}

std::vector<UniqueTile>
loadUnique(const configuru::Config &config, const TileLoader &tileLoader,
           const std::unordered_set<std::string> &subset, size_t tileSize) {
  std::vector<UniqueTile> toReturn;

  auto functor = [&](const std::string &tile_name, Symmetry symmetry,
                     double weight) {
    int cardinality = cardinalityForSymmetry(symmetry);

    UniqueTile nextTile = {tile_name, symmetry, {}, weight};

    // This is only used for summer group.
    // In that case, all the rotations are unique and are given using enumerated
    // images: cliff 1.bmp, cliff 2.bmp, cliff 3.bmp, etc.
    for (int t = 0; t < cardinality; ++t) {
      std::stringstream stream;
      stream << tile_name << " " << t;
      const Tile bitmap = tileLoader(stream.str());
      //CHECK_EQ_F(bitmap.size(), tileSize * tileSize);
      nextTile.tiles.push_back(bitmap);
    }

    toReturn.push_back(nextTile);
  };

  forTileData(config, subset, functor);
  return toReturn;
}

PalettedImage load_paletted_image(const std::string &path) {
  //ERROR_CONTEXT("loading sample image", path.c_str());
  int width, height, comp;
  RGBA *rgba = reinterpret_cast<RGBA *>(
      stbi_load(path.c_str(), &width, &height, &comp, 4));
  assert(rgba);
  //CHECK_NOTNULL_F(rgba);
  auto num_pixels = width * height;

  // Fix issues with stbi_load:
  if (comp == 1) {
    // input was greyscale - set alpha:
    for (int i = 0; i < num_pixels; ++i) {
      rgba[i].a = rgba[i].r;
    }
  } else {
    for (int i = 0; i < num_pixels; ++i) {
      if (comp == 3) {
        rgba[i].a = 255;
      }
      if (rgba[i].a == 0) {
        rgba[i] = RGBA{0, 0, 0, 0};
      }
    }
  }

  std::vector<RGBA> palette;

  Array2D<ColorIndex> data(
      {static_cast<size_t>(width), static_cast<size_t>(height)});

  auto pixel_idx = 0;
  for (size_t dy = 0; dy < height; ++dy) {
    for (size_t dx = 0; dx < width; ++dx) {
      const RGBA color = rgba[pixel_idx];
      const auto color_idx =
          std::find(palette.begin(), palette.end(), color) - palette.begin();
      if (color_idx == palette.size()) {
        //CHECK_LT_F(palette.size(), MAX_COLORS, "Too many colors in image");
        palette.push_back(color);
      }
      data[{dx, dy}] = color_idx;

      ++pixel_idx;
    }
  }

  stbi_image_free(rgba);

  return PalettedImage{data, palette};
}

OverlappingModelConfig
extractOverlappingConfig(const std::string &image_dir,
                         const configuru::Config &config) {
  const auto image_filename = config["image"].as_string();
  const auto in_path = image_dir + image_filename;

  return {load_paletted_image(in_path),
          config.get_or("periodic_in", true),
          (size_t)config.get_or("symmetry", 8),
          config.get_or("foundation", false),
          config.get_or("n", 3),
          {{(size_t)config.get_or("width", 48),
            (size_t)config.get_or("height", 48)},
           config.get_or("periodic_out", true)}};
}

Tile loadTile(const std::string &subdir, const std::string &image_dir,
              const std::string &tile_name) {
  std::stringstream stream;
  stream << image_dir << subdir << "/" << tile_name << ".bmp";
  const std::string path = stream.str();
  int width, height, comp;
  RGBA *rgba = reinterpret_cast<RGBA *>(
      stbi_load(path.c_str(), &width, &height, &comp, 4));
  assert(rgba);
  //CHECK_NOTNULL_F(rgba);
  const auto num_pixels = width * height;
  Tile tile(rgba, rgba + num_pixels);
  stbi_image_free(rgba);
  return tile;
}

std::unordered_set<std::string> loadSubsets(const configuru::Config &config,
                                            const std::string &subset_name) {
  std::unordered_set<std::string> toReturn;
  for (const auto &tile_name : config["subsets"][subset_name].as_array()) {
    toReturn.insert(tile_name.as_string());
  }
  return toReturn;
}

TileModelConfig extractConfig(const std::string &image_dir,
                              const configuru::Config &topConfig) {
  const std::string subdir = topConfig["subdir"].as_string();

  const auto root_dir = image_dir + subdir + "/";

  auto config = configuru::parse_file(root_dir + "data.cfg", configuru::CFG);

  // This is usually not specified (therefore, false)
  const bool uniqueFlag = config.get_or("unique", false);

  std::string subset_name = topConfig.get_or("subset", std::string());

  std::unordered_set<std::string> subset;
  if (subset_name != "") {
    subset = loadSubsets(config, subset_name);
  }

  TileLoader tile_loader = [image_dir, subdir](const std::string &tile_name) {
    return loadTile(subdir, image_dir, tile_name);
  };

  size_t tileSize = config.get_or("tile_size", 16);

  std::vector<UniqueTile> unique;
  std::vector<CopiedTile> copied;
  if (uniqueFlag) {
    unique = loadUnique(config, tile_loader, subset, tileSize);
  } else {
    copied = loadCopied(config, tile_loader, subset, tileSize);
  }

  auto neighbors = loadNeighbors(config);

  return {
      tileSize,
      subset,
      uniqueFlag,
      unique,
      copied,
      neighbors,
      {{(size_t)topConfig.get_or("width", 48),
        (size_t)topConfig.get_or("height", 48)},
       topConfig.get_or("periodic", false)},
  };
}

GeneralConfig importGeneralConfig(const std::string &name,
                                  const configuru::Config &config) {
  size_t importedLimit = (size_t)config.get_or("limit", 0);
  size_t actualLimit = 0;
  if (importedLimit != 0) {
    actualLimit = importedLimit;
  }

  return {actualLimit, (size_t)config.get_or("numOutput", 2), name};
}

void run_config_file(const std::string &path, ConfigActions actions) {
  //LOG_F(INFO, "Running all samples in %s", path.c_str());
  const auto samples = configuru::parse_file(path, configuru::CFG);
  const auto image_dir = samples["image_dir"].as_string();

  if (samples.count("overlapping")) {
    for (const auto &p : samples["overlapping"].as_object()) {
      //LOG_SCOPE_F(INFO, "%s", p.key().c_str());

      std::cout << "key = " << p.key() << "\n";
      const auto &config = p.value();

      GeneralConfig generalConfig = importGeneralConfig(p.key(), config);

      OverlappingModelConfig overlappingModelConfig =
          extractOverlappingConfig(image_dir, config);
      actions.overlappingAction(generalConfig, overlappingModelConfig);

      p.value().check_dangling();
    }
  }

  if (samples.count("tiled")) {
    for (const auto &p : samples["tiled"].as_object()) {
      //LOG_SCOPE_F(INFO, "Tiled %s", p.key().c_str());

      const auto &config = p.value();

      GeneralConfig generalConfig = importGeneralConfig(p.key(), config);

      TileModelConfig tileModelConfig = extractConfig(image_dir, config);

      actions.tileAction(generalConfig, tileModelConfig);
    }
  }
}
