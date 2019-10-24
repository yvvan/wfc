#pragma once

#include <wfc/arrays.h>

// Had to add because offset can be negative
struct Offset2D {

  int x;

  int y;
};

//! \brief Defines a square range. The bottomLeft and upperRight
//! are included in the range.
struct SquareRange {

  Offset2D bottomLeft;

  Offset2D upperRight;
};

inline auto range2D(const SquareRange &range) {
  return [=](auto consumingFcn) {
    // Order is weird, but can/should be changed.
    for (int x = range.bottomLeft.x; x <= range.upperRight.x; ++x) {
      for (int y = range.bottomLeft.y; y <= range.upperRight.y; ++y) {
        consumingFcn(Offset2D{x, y});
      }
    }
  };
}

inline auto range2D(const Dimension2D &dimension) {
  // TODO: Implement in terms of SquareRange

  return [=](auto consumingFcn) {
    for (size_t y = 0; y < dimension.height; ++y) {
      for (size_t x = 0; x < dimension.width; ++x) {
        consumingFcn(Index2D{x, y});
      }
    }
  };
}

template <class Functor>
void runForDimension(const Dimension2D &dimension, Functor functor) {
  auto range = range2D(dimension);
  range(functor);
}

namespace BreakRange {

inline auto range2D(const Dimension2D &dimension) {
  return [=](auto consumingFcn) {
    for (size_t y = 0; y < dimension.height; ++y) {
      for (size_t x = 0; x < dimension.width; ++x) {
        if (consumingFcn(Index2D{x, y})) {
          return true;
        }
      }
    }
    return false;
  };
}

template <class Functor>
void runForDimension(const Dimension2D &dimension, Functor functor) {
  auto range = BreakRange::range2D(dimension);
  range(functor);
}

} // namespace BreakRange
