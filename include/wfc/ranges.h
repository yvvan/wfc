#ifndef _WFC_RANGES_H_
#define _WFC_RANGES_H_

#include <wfc/arrays.h>

//! \brief Defines a square range. The bottomLeft and upperRight
//! are included in the range.
struct SquareRange
{

	Index2D bottomLeft;

	Index2D upperRight;

};

inline auto range2D(const SquareRange& range)
{
	return [=] (auto consumingFcn)
	{
		for (int x = range.bottomLeft.x; x <= range.upperRight.x; ++x) 
		{
			for (int y = range.bottomLeft.y; y <= range.upperRight.y; ++y) 
			{
				consumingFcn(Index2D{ x, y });
			}
		}
	};
}

inline auto range2D(const Dimension2D& dimension)
{
	return [=] (auto consumingFcn)
	{
		for (size_t y = 0; y < dimension.height; ++y)
		{
			for (size_t x = 0; x < dimension.width; ++x) 
			{
				consumingFcn(Index2D{x, y});
			}
		}
	};
}

template <class Functor>
void runForDimension(const Dimension2D& dimension, Functor functor)
{
	auto range = range2D(dimension);
	range(functor);
}

namespace BreakRange
{

	inline auto range2D(const Dimension2D& dimension)
	{
		return [=] (auto consumingFcn)
		{
			for (size_t y = 0; y < dimension.height; ++y)
			{
				for (size_t x = 0; x < dimension.width; ++x) 
				{
					if (consumingFcn(Index2D{x, y}))
					{
						return true;
					}
				}
			}
			return false;
		};
	}

	template <class Functor>
	void runForDimension(const Dimension2D& dimension, Functor functor)
	{
		auto range = BreakRange::range2D(dimension);
		range(functor);
	}

}

#endif
