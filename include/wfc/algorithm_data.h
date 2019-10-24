#pragma once

#include <wfc/arrays.h>

// To avoid problems with vector<bool>
using Bool = uint8_t;

// Data used during tiling algorithm operation
struct AlgorithmData {
  // _width X _height X num_patterns
  // _wave.get(x, y, t) == is the pattern t possible at x, y?
  // Starts off true everywhere.
  Array3D<Bool> _wave;
  Array2D<Bool> _changes; // _width X _height. Starts off false everywhere.
};

AlgorithmData initialOutput(const Dimension2D &outputDimensions,
                            size_t numPatterns);
