#ifndef _WFC_ARRAYS_H_
#define _WFC_ARRAYS_H_

#include <vector>
#include <cstdlib>

struct Dimension2D
{

    size_t width;

    size_t height;

};

struct Index2D
{

	size_t x;

	size_t y;

};

template <class T>
class Array2D
{

	public:

		Array2D() : 
            mDimensions{0, 0}
        {}

		Array2D(const Dimension2D& dimension, T value = {}) : 
            mDimensions(dimension), 
            mData(dimension.width * dimension.height, value)
        {}

		typename std::vector<T>::reference operator[](const Index2D& index2D)
		{
			return mData[index(index2D.x, index2D.y)];
		}

		typename std::vector<T>::const_reference operator[](const Index2D& index2D) const
		{
			return mData[index(index2D.x, index2D.y)];
		}

		typename std::vector<T>::reference ref(size_t x, size_t y)
		{
			return mData[index(x, y)];
		}

		typename std::vector<T>::const_reference ref(size_t x, size_t y) const
		{
			return mData[index(x, y)];
		}

		Dimension2D size() const
		{
			return mDimensions;
		}

		const T* data() const
		{
			return mData.data();
		}

	private:

		size_t index(size_t x, size_t y) const
		{
			return y * mDimensions.width + x;
		}

        Dimension2D mDimensions;

		std::vector<T> mData;

};

struct Index3D
{

	size_t x;

	size_t y;

	size_t z;

};

struct Dimension3D
{

    size_t width;

    size_t height;

    size_t depth;

};

template <class T>
class Array3D
{

	public: 

		Array3D() : 
            mDimensions{0, 0, 0}
        {}

		Array3D(size_t w, size_t h, size_t d, T value = {}) : 
			mDimensions{w, h, d},
			mData(w * h * d, value)
		{}

		typename std::vector<T>::reference operator[](const Index3D& index3D)
		{
			return mData[index(index3D.x, index3D.y, index3D.z)];
		}

		typename std::vector<T>::const_reference operator[](const Index3D& index3D) const
		{
			return mData[index(index3D.x, index3D.y, index3D.z)];
		}

		typename std::vector<T>::reference ref(size_t x, size_t y, size_t z)
		{
			return mData[index(x, y, z)];
		}

		typename std::vector<T>::const_reference ref(size_t x, size_t y, size_t z) const
		{
			return mData[index(x, y, z)];
		}

		size_t volume() const 
		{
			return mData.size();
		}

	private:

		size_t index(size_t x, size_t y, size_t z) const
		{
			return x * mDimensions.height * mDimensions.depth + y * mDimensions.depth + z; 
		}

		Dimension3D mDimensions;

		std::vector<T> mData;

};

inline Index3D append(const Index2D& index2D, size_t z)
{
	return { index2D.x, index2D.y, z };
}

inline Dimension3D append(const Dimension2D& dimension2D, size_t depth)
{
	return { dimension2D.width, dimension2D.height, depth };
}

#endif
