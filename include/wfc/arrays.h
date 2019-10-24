#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>

struct Dimension2D {

  size_t width;

  size_t height;
};

struct Index2D {

  size_t x;

  size_t y;
};

template <class T> class Array2D {

public:
  Array2D() : mDimensions{0, 0} {}

  Array2D(const Dimension2D &dimension, T value = {})
      : mDimensions(dimension),
        mData(dimension.width * dimension.height, value) {}

  Array2D(std::initializer_list<std::initializer_list<T>> values) {
    mDimensions = {longestInitSize(values), values.size()};
    mData = std::vector<T>(mDimensions.width * mDimensions.height);

    size_t y = 0;
    for (auto yList : values) {
      size_t x = 0;
      for (auto value : yList) {
        operator[]({x, y}) = value;

        x++;
      }

      y++;
    }
  }

  typename std::vector<T>::reference operator[](const Index2D &index2D) {
    return mData[index(index2D.x, index2D.y)];
  }

  typename std::vector<T>::const_reference
  operator[](const Index2D &index2D) const {
    return mData[index(index2D.x, index2D.y)];
  }

  Dimension2D size() const { return mDimensions; }

  const T *data() const { return mData.data(); }

  bool operator==(const Array2D<T> &other) const {
    return (mDimensions == other.mDimensions) && (mData == other.mData);
  }

private:
  static size_t
  longestInitSize(std::initializer_list<std::initializer_list<T>> values) {
    size_t toReturn = 0;
    for (auto list : values) {
      toReturn = std::max(toReturn, list.size());
    }
    return toReturn;
  }

  size_t index(size_t x, size_t y) const { return y * mDimensions.width + x; }

  Dimension2D mDimensions;

  std::vector<T> mData;
};

struct Index3D {

  size_t x;

  size_t y;

  size_t z;
};

struct Dimension3D {

  size_t width;

  size_t height;

  size_t depth;
};

template <class T> class Array3D {

public:
  Array3D() : mDimensions{0, 0, 0} {}

  Array3D(Dimension3D dimension, T value = {})
      : mDimensions(dimension),
        mData(dimension.width * dimension.height * dimension.depth, value) {}

  typename std::vector<T>::reference operator[](const Index3D &index3D) {
    return mData[index(index3D.x, index3D.y, index3D.z)];
  }

  typename std::vector<T>::const_reference
  operator[](const Index3D &index3D) const {
    return mData[index(index3D.x, index3D.y, index3D.z)];
  }

  Dimension3D size() const { return mDimensions; }

  size_t volume() const { return mData.size(); }

private:
  size_t index(size_t x, size_t y, size_t z) const {
    return x * mDimensions.height * mDimensions.depth + y * mDimensions.depth +
           z;
  }

  Dimension3D mDimensions;

  std::vector<T> mData;
};

inline Index3D append(const Index2D &index2D, size_t z) {
  return {index2D.x, index2D.y, z};
}

inline Dimension3D append(const Dimension2D &dimension2D, size_t depth) {
  return {dimension2D.width, dimension2D.height, depth};
}

inline size_t area(const Dimension2D &dimension2D) {
  return dimension2D.width * dimension2D.height;
}

inline Index2D operator+(const Index2D &left, const Index2D &right) {
  return {left.x + right.x, left.y + right.y};
}

inline bool operator==(const Index2D &left, const Index2D &right) {
  return (left.x == right.x) && (left.y == right.y);
}

inline bool operator==(const Dimension2D &left, const Dimension2D &right) {
  return (left.width == right.width) && (left.height == right.height);
}

inline bool operator!=(const Index2D &left, const Index2D &right) {
  return (left.x != right.x) || (left.y != right.y);
}

inline bool operator!=(const Dimension2D &left, const Dimension2D &right) {
  return !(left == right);
}

inline std::ostream &operator<<(std::ostream &stream, const Index2D &index) {
  stream << "{" << index.x << ", " << index.y << "}";
  return stream;
}

template <class T>
inline std::ostream &operator<<(std::ostream &stream, const Array2D<T> &array) {
  Dimension2D dimension = array.size();
  stream << "{\n";
  for (int y = 0; y < dimension.height; ++y) {
    stream << "\t{ ";
    for (int x = 0; x < dimension.width; ++x) {
      Index2D index{x, y};
      stream << array[index];
      stream << ", ";
    }
    stream << "},\n";
  }
  stream << "}\n";
  return stream;
}

inline std::ostream &operator<<(std::ostream &stream,
                                const Array2D<uint8_t> &array) {
  Dimension2D dimension = array.size();
  stream << "{\n";
  for (size_t y = 0; y < dimension.height; ++y) {
    stream << "\t{ ";
    for (size_t x = 0; x < dimension.width; ++x) {
      Index2D index{x, y};
      stream << (int)array[index];
      stream << ", ";
    }
    stream << "},\n";
  }
  stream << "}\n";
  return stream;
}
