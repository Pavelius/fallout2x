#include "point.h"

#pragma once

const unsigned short Blocked = 0xFFFF;
typedef unsigned short indext;
const int				mps = 128;

enum direction_s : unsigned char {
	Center, Up, RightUp, Right, RightDown, Down, LeftDown, Left, LeftUp,
};

class areai {
	unsigned short		floor[mps * mps];
	unsigned short		roof[mps * mps];
	unsigned char		blocks[mps * mps / 8];
public:
	void				clear();
	unsigned short		getfloor(indext i) const { return floor[i]; }
	unsigned short		getroof(indext i) const { return roof[i]; }
	short				find(short v, short x0, short x1, short y, bool need_roof) const;
	void				read(const char* id);
	int					reapeated(int x0, int x1, int y, bool need_roof) const;
	void				setroof(indext i, int v) { roof[i] = v; }
	void				setfloor(indext i, int v) { floor[i] = v; }
	void				set(indext i, short unsigned v, short w, short h);
	void				set(indext i, short unsigned v, short w, short h, int random);
	static indext		tot(indext i, int d);
	void				write(const char* id) const;
};

inline point			h2t(point v) { return {(short)(v.x / 2), (short)(v.y / 2)}; }
inline point			i2t(indext i) { return {(short)(i % mps), (short)(i / mps)}; }
inline point			i2h(indext i) { return {(short)(i % (mps * 2)), (short)(i / (mps * 2))}; }
inline indext			t2i(point v) { return v.y * mps + v.x; }
inline point			t2h(point v) { return {(short)(v.x * 2), (short)(v.y * 2)}; }
