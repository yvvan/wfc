#include <wfc/algorithm_data.h>

AlgorithmData initialOutput(const Dimension2D &outputDimensions,
                            size_t numPatterns) {
  Dimension3D waveDimension = append(outputDimensions, numPatterns);
  return {Array3D<Bool>(waveDimension, true),
          Array2D<Bool>(outputDimensions, false)};
}
