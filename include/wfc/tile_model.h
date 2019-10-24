#pragma once

#include <wfc/arrays.h>
#include <wfc/imodel.h>
#include <wfc/rgba.h>

#include <functional>
#include <unordered_set>
#include <vector>

// Should be Array2D<RGBA>?
using Tile = std::vector<RGBA>;

struct TileModelInternal {

  CommonParams mCommonParams;

  // 4 X numPatterns X numPatterns
  Array3D<Bool> _propagator;

  std::vector<std::vector<RGBA>> _tiles;

  size_t _tile_size;
};

class TileModel : public Model {

public:
  TileModel(const TileModelInternal &internal);

  bool propagate(AlgorithmData &algorithmData) const override;

  bool on_boundary(const Index2D &index) const override;

  std::unique_ptr<Image> image(const AlgorithmData &algorithmData) const override;

  AlgorithmData initAlgorithmData() const override;

private:
  CommonParams mCommonParams;

  const TileModelInternal &mInternal;
};

Tile rotate(const Tile &in_tile, const size_t tile_size);

enum class Symmetry {
  None,
  X,
  L,
  T,
  I,
  // "Slash - /"
  S
};

using MapFunction = std::function<int(int)>;

struct MapFunctions {

  MapFunction a;

  MapFunction b;
};

struct SymmetryInfo {

  int cardinality;

  MapFunctions mapFunctions;
};

struct UniqueTile {

  std::string name;

  Symmetry symmetry;

  std::vector<Tile> tiles;

  double weight;
};

struct CopiedTile {

  std::string name;

  Symmetry symmetry;

  Tile tile;

  double weight;
};

SymmetryInfo convert(Symmetry symmetry);

struct Neighbor {

  std::string name;

  int value;
};

struct Neighbors {

  Neighbor left;

  Neighbor right;
};

struct TileModelConfig {

  size_t tileSize;

  std::unordered_set<std::string> subset;

  bool unique;

  std::vector<UniqueTile> uniqueTiles;

  std::vector<CopiedTile> copiedTiles;

  std::vector<Neighbors> neighbors;

  OutputProperties commonParam;
};

TileModelInternal fromConfig(const TileModelConfig &config);

std::vector<UniqueTile>
rotateConvert(const std::vector<CopiedTile> &copiedTiles, size_t tileSize);

int cardinalityForSymmetry(Symmetry symmetry);

void modifyOutputForFoundation(const CommonParams &commonParams,
                               const Model &model, size_t foundation,
                               AlgorithmData &algorithmData);
