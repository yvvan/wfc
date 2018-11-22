#ifndef _WFC_ARRAYS_H_
#define _WFC_ARRAYS_H_

#include <vector>

struct Dimension2D
{

    size_t width;

    size_t height;

};

template <class T>
class Array2D
{

	public:

		Array2D() : 
            mDimensions{0, 0}
        {}

		Array2D(size_t w, size_t h, T value = {}) : 
            mDimensions{w, h}, 
            mData(w * h, value)
        {}

		T& ref(size_t x, size_t y)
		{
			return mData[index(x, y)];
		}

		const T& ref(size_t x, size_t y) const
		{
			return mData[index(x, y)];
		}

		size_t   width()  const { return mDimensions.width;       }
		size_t   height() const { return mDimensions.height;      }
		const T* data()   const { return mData.data(); }

	private:

		size_t index(size_t x, size_t y) const
		{
			return y * mDimensions.width + x;
		}

        Dimension2D mDimensions;

		std::vector<T> mData;

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

		T& ref(size_t x, size_t y, size_t z)
		{
			return mData[index(x, y, z)];
		}

		const T& ref(size_t x, size_t y, size_t z) const
		{
			return mData[index(x, y, z)];
		}

		size_t size() const 
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

#endif
