#pragma once

#include <vector>

#include <wfc/algorithm_data.h>
#include <wfc/arrays.h>
#include <wfc/rgba.h>

// Properties of algorithmData image.
struct OutputProperties {

  Dimension2D dimensions;

  bool periodic;
};

struct CommonParams {

  OutputProperties mOutputProperties;

  size_t numPatterns;

  // The weight of each pattern (e.g. how often that pattern occurs in the
  // sample image).
  std::vector<double> patternWeights;
};

using Image = Array2D<RGBA>;

class Model {

public:
  virtual bool propagate(AlgorithmData &algorithmData) const = 0;

  virtual bool on_boundary(const Index2D &index) const = 0;

  virtual std::unique_ptr<Image> image(const AlgorithmData &algorithmData) const = 0;

  virtual AlgorithmData initAlgorithmData() const = 0;
};
