#ifndef _WFC_RANGES_H_
#define _WFC_RANGES_H_

#include <wfc/arrays.h>

auto range2D(const Dimension2D& dimension)
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
};

template <class Functor>
void runForDimension(const Dimension2D& dimension, Functor functor)
{
	auto range = range2D(dimension);
	range(functor);
};


#endif
