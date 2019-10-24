#include <wfc/tile_model.h>

#include <array>
#include <cassert>
#include <unordered_map>

TileModel::TileModel(const TileModelInternal &internal) : mInternal(internal) {
  // Needed because other functions access from base class
  // TODO: Remove
  mCommonParams = mInternal.mCommonParams;
}

bool TileModel::on_boundary(const Index2D &index) const { return false; }

bool TileModel::propagate(AlgorithmData &algorithmData) const {
  bool did_change = false;

  Dimension2D dimension = mCommonParams.mOutputProperties.dimensions;

  for (size_t x2 = 0; x2 < dimension.width; ++x2) {
    for (size_t y2 = 0; y2 < dimension.height; ++y2) {
      for (size_t d = 0; d < 4; ++d) {
        size_t x1 = x2, y1 = y2;

        // Looks like this might be wrap-around algorithm?
        if (d == 0) {
          if (x2 == 0) {
            if (!mCommonParams.mOutputProperties.periodic) {
              continue;
            }
            x1 = dimension.width - 1;
          } else {
            x1 = x2 - 1;
          }
        } else if (d == 1) {
          if (y2 == dimension.height - 1) {
            if (!mCommonParams.mOutputProperties.periodic) {
              continue;
            }
            y1 = 0;
          } else {
            y1 = y2 + 1;
          }
        } else if (d == 2) {
          if (x2 == dimension.width - 1) {
            if (!mCommonParams.mOutputProperties.periodic) {
              continue;
            }
            x1 = 0;
          } else {
            x1 = x2 + 1;
          }
        } else {
          if (y2 == 0) {
            if (!mCommonParams.mOutputProperties.periodic) {
              continue;
            }
            y1 = dimension.height - 1;
          } else {
            y1 = y2 - 1;
          }
        }

        if (!algorithmData._changes[{x1, y1}]) {
          continue;
        }

        for (size_t t2 = 0; t2 < mCommonParams.numPatterns; ++t2) {
          if (algorithmData._wave[{x2, y2, t2}]) {
            bool b = false;
            for (size_t t1 = 0; t1 < mCommonParams.numPatterns && !b; ++t1) {
              if (algorithmData._wave[{x1, y1, t1}]) {
                b = mInternal._propagator[{d, t1, t2}];
              }
            }
            if (!b) {
              algorithmData._wave[{x2, y2, t2}] = false;
              algorithmData._changes[{x2, y2}] = true;
              did_change = true;
            }
          }
        }
      }
    }
  }

  return did_change;
}

std::unique_ptr<Image>
TileModel::image(const AlgorithmData &algorithmData) const {
  Dimension2D dimension = mCommonParams.mOutputProperties.dimensions;
  auto result = std::make_unique<Image>(
      Dimension2D{dimension.width * mInternal._tile_size,
                  dimension.height * mInternal._tile_size},
      RGBA{});

  for (size_t x = 0; x < dimension.width; ++x) {
    for (size_t y = 0; y < dimension.height; ++y) {
      double sum = 0;
      for (size_t t = 0; t < mCommonParams.numPatterns; ++t) {
        if (algorithmData._wave[{x, y, t}]) {
          sum += mCommonParams.patternWeights[t];
        }
      }

      for (size_t yt = 0; yt < mInternal._tile_size; ++yt) {
        for (size_t xt = 0; xt < mInternal._tile_size; ++xt) {
          if (sum == 0) {
            (*result)[{x * mInternal._tile_size + xt,
                       y * mInternal._tile_size + yt}] = RGBA{0, 0, 0, 255};
          } else {
            double r = 0, g = 0, b = 0, a = 0;
            for (size_t t = 0; t < mCommonParams.numPatterns; ++t) {
              if (algorithmData._wave[{x, y, t}]) {
                RGBA c = mInternal._tiles[t][xt + yt * mInternal._tile_size];
                r += (double)c.r * mCommonParams.patternWeights[t] / sum;
                g += (double)c.g * mCommonParams.patternWeights[t] / sum;
                b += (double)c.b * mCommonParams.patternWeights[t] / sum;
                a += (double)c.a * mCommonParams.patternWeights[t] / sum;
              }
            }

            (*result)[{x * mInternal._tile_size + xt,
                       y * mInternal._tile_size + yt}] =
                RGBA{(uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a};
          }
        }
      }
    }
  }

  return result;
}

Tile rotate(const Tile &in_tile, const size_t tile_size) {
  // CHECK_EQ_F(in_tile.size(), tile_size * tile_size);
  Tile out_tile;
  for (size_t y = 0; y < tile_size; ++y) {
    for (size_t x = 0; x < tile_size; ++x) {
      out_tile.push_back(in_tile[tile_size - 1 - y + x * tile_size]);
    }
  }
  return out_tile;
}

int cardinalityForSymmetry(Symmetry symmetry) {
  int toReturn = 0;
  switch (symmetry) {

  case Symmetry::L:
    toReturn = 4;
    break;

  case Symmetry::T:
    toReturn = 4;
    break;

  case Symmetry::I:
    toReturn = 2;
    break;

  case Symmetry::S:
    toReturn = 2;
    break;

  case Symmetry::X:
    toReturn = 1;
    break;
  case Symmetry::None:
    break;
  }
  return toReturn;
}

MapFunctions mapFunctions(Symmetry symmetry) {
  MapFunctions toReturn;
  switch (symmetry) {

  case Symmetry::L:
    toReturn = {[](int i) { return (i + 1) % 4; },
                [](int i) { return i % 2 == 0 ? i + 1 : i - 1; }};
    break;

  case Symmetry::T:
    toReturn = {[](int i) { return (i + 1) % 4; },
                [](int i) { return i % 2 == 0 ? i : 4 - i; }};
    break;

  case Symmetry::I:
    toReturn = {[](int i) { return 1 - i; }, [](int i) { return i; }};
    break;

  case Symmetry::S:
    toReturn = {[](int i) { return 1 - i; }, [](int i) { return 1 - i; }};
    break;

  case Symmetry::X:
    toReturn = {[](int i) { return i; }, [](int i) { return i; }};
    break;
  case Symmetry::None:
    assert(false);
    break;
  }
  return toReturn;
}

AlgorithmData TileModel::initAlgorithmData() const {
  return initialOutput(mCommonParams.mOutputProperties.dimensions,
                       mCommonParams.numPatterns);
}

SymmetryInfo convert(Symmetry symmetry) {
  return {cardinalityForSymmetry(symmetry), mapFunctions(symmetry)};
}

std::vector<UniqueTile>
rotateConvert(const std::vector<CopiedTile> &copiedTiles, size_t tileSize) {
  std::vector<UniqueTile> toReturn;
  for (const auto &copiedTile : copiedTiles) {
    UniqueTile nextTile = {
        copiedTile.name, copiedTile.symmetry, {}, copiedTile.weight};

    Tile currentTile = copiedTile.tile;
    nextTile.tiles.push_back(currentTile);

    int cardinality = cardinalityForSymmetry(copiedTile.symmetry);

    for (int t = 0; t < cardinality - 1; ++t) {
      currentTile = rotate(currentTile, tileSize);
      nextTile.tiles.push_back(currentTile);
    }

    toReturn.push_back(nextTile);
  }
  return toReturn;
}

// TODO: Seems like part of the algorithm is here - should be moved to
// algorithm-specific files. This file is just meant for loading config with
// configuru.
TileModelInternal fromConfig(const TileModelConfig &config) {
  TileModelInternal toReturn;

  toReturn.mCommonParams.mOutputProperties = config.commonParam;

  toReturn._tile_size = config.tileSize;

  const bool unique = config.unique;

  const std::unordered_set<std::string> &subset = config.subset;

  std::vector<std::array<size_t, 8>> action;
  std::unordered_map<std::string, size_t> first_occurrence;

  std::vector<UniqueTile> loadedTiles;
  if (unique) {
    loadedTiles = config.uniqueTiles;
  } else {
    const auto &loadedCopiedTiles = config.copiedTiles;
    loadedTiles = rotateConvert(loadedCopiedTiles, toReturn._tile_size);
  }

  if (loadedTiles.empty()) {

  }

  const auto &neighbors = config.neighbors;

  for (const auto &tile : loadedTiles) {
    SymmetryInfo symmetryInfo = convert(tile.symmetry);
    const auto &a = symmetryInfo.mapFunctions.a;
    const auto &b = symmetryInfo.mapFunctions.b;
    int cardinality = symmetryInfo.cardinality;

    const size_t num_patterns_so_far = action.size();
    first_occurrence[tile.name] = num_patterns_so_far;

    for (const auto &tileImage : tile.tiles) {
      toReturn._tiles.push_back(tileImage);
    }

    for (int t = 0; t < cardinality; ++t) {
      std::array<size_t, 8> map;

      map[0] = t;
      map[1] = a(t);
      map[2] = a(a(t));
      map[3] = a(a(a(t)));
      map[4] = b(t);
      map[5] = b(a(t));
      map[6] = b(a(a(t)));
      map[7] = b(a(a(a(t))));

      for (int s = 0; s < 8; ++s) {
        map[s] += num_patterns_so_far;
      }

      action.push_back(map);
    }

    double weight = tile.weight;
    for (int t = 0; t < cardinality; ++t) {
      toReturn.mCommonParams.patternWeights.push_back(weight);
    }
  }

  toReturn.mCommonParams.numPatterns = action.size();

  toReturn._propagator = Array3D<Bool>({4, toReturn.mCommonParams.numPatterns,
                                        toReturn.mCommonParams.numPatterns},
                                       false);

  for (const auto &neighbor : neighbors) {
    const auto &left = neighbor.left;
    const auto &right = neighbor.right;

    if (!subset.empty() &&
        (subset.count(left.name) == 0 || subset.count(right.name) == 0)) {
      continue;
    }

    size_t L = action[first_occurrence[left.name]][left.value];
    size_t R = action[first_occurrence[right.name]][right.value];
    size_t D = action[L][1];
    size_t U = action[R][1];

    toReturn._propagator[{0, L, R}] = true;
    toReturn._propagator[{0, action[L][6], action[R][6]}] = true;
    toReturn._propagator[{0, action[R][4], action[L][4]}] = true;
    toReturn._propagator[{0, action[R][2], action[L][2]}] = true;

    toReturn._propagator[{2, D, U}] = true;
    toReturn._propagator[{1, action[U][6], action[D][6]}] = true;
    toReturn._propagator[{1, action[D][4], action[U][4]}] = true;
    toReturn._propagator[{1, action[U][2], action[D][2]}] = true;
  }

  // Accounts for some inherent symmetry(?)
  for (size_t t1 = 0; t1 < toReturn.mCommonParams.numPatterns; ++t1) {
    for (size_t t2 = 0; t2 < toReturn.mCommonParams.numPatterns; ++t2) {
      toReturn._propagator[{2, t1, t2}] = toReturn._propagator[{0, t2, t1}];
      toReturn._propagator[{3, t1, t2}] = toReturn._propagator[{1, t2, t1}];
    }
  }
  return toReturn;
}
