#include "point.h"

#pragma once

const unsigned short Blocked = 0xFFFF;
typedef unsigned short indext;
const int				mps = 128;

class areai {
	indext				floor[mps * mps];
	indext				roof[mps * mps];
public:
	void				clear();
	//static int			getx(indext i) { return i % mps; }
	//static int			gety(indext i) { return i / mps; }
	//static indext		geti(int x, int y) { return y * mps + x; }
	unsigned short		getfloor(indext i) const { return floor[i]; }
	unsigned short		getroof(indext i) const { return roof[i]; }
	void				setroof(indext i, int v) { roof[i] = v; }
	void				setfloor(indext i, int v) { floor[i] = v; }
	void				set(indext i, int v, int w, int h);
};
indext					h2i(point pt);
inline point			i2h(indext i) { return {(short)(i % (mps * 2)), (short)(i / (mps * 2))}; }
