#include "point.h"

#pragma once

const unsigned short Blocked = 0xFFFF;
typedef unsigned short indext;
const int				mps = 128;

class areai {
	unsigned short		floor[mps * mps];
	unsigned short		roof[mps * mps];
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
	void				setwall(indext i, short unsigned v);
	void				write(const char* id) const;
};
