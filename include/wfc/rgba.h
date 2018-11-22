#ifndef _WFC_RGBA_H_
#define _WFC_RGBA_H_

struct RGBA
{
	uint8_t r, g, b, a;
};

bool operator==(RGBA x, RGBA y)
{
	return x.r == y.r 
		&& x.g == y.g 
		&& x.b == y.b 
		&& x.a == y.a;
}

#endif
