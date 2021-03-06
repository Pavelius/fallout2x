#include "area.h"
#include "crt.h"

areai loc;

void areai::clear() {
	memset(this, 0, sizeof(*this));
}

indext h2i(point h) {
	if((unsigned)h.x >= mps * 2 || (unsigned)h.y >= mps * 2)
		return Blocked;
	return h.y * mps * 2 + h.x;
}

void areai::set(indext i, short unsigned v, short w, short h) {
	for(auto y = 0; y < h; y++) {
		auto pi = i;
		for(auto x = 0; x < w; x++)
			floor[i++] = v;
		i = pi + mps;
	}
}

void areai::set(indext i, short unsigned v, short w, short h, int random) {
	if(!random)
		return;
	for(auto y = 0; y < h; y++) {
		auto pi = i;
		for(auto x = 0; x < w; x++)
			floor[i++] = v + rand() % random;
		i = pi + mps;
	}
}

int	areai::reapeated(int x0, int x1, int y, bool need_roof) const {
	auto p = (need_roof ? roof : floor) + y * mps;
	auto result = 1;
	auto v = p[x0];
	for(auto x = x0; x < x1 - 1; x++) {
		if(p[x + 1] == (v + 1)) {
			result++;
			v++;
		} else
			break;
	}
	return result;
}

short areai::find(short v, short x0, short x1, short y, bool need_roof) const {
	auto p = (need_roof ? roof : floor) + y * mps;
	for(auto x = x0; x < x1; x++) {
		if(p[x] == v)
			return x;
	}
	return -1;
}

indext areai::tot(indext i, int d) {
	point t = i2t(i);
	switch(d) {
	case Up:
		if(t.y == 0)
			return Blocked;
		t.y--;
		break;
	case Down:
		if(t.y == mps - 1)
			return Blocked;
		t.y++;
		break;
	case Right:
		if(t.x == mps - 1)
			return Blocked;
		t.x++;
		break;
	case Left:
		if(t.x == 0)
			return Blocked;
		t.x--;
		break;
	default:
		return Blocked;
	}
	return t2i(t);
}

//void areai::set(indext i, int v, int w, int h) {
//	auto x0 = getx(i), y0 = gety(i);
//	auto x1 = x0 + w, y1 = y0 + h;
//	for(auto y = y0; y < y1; y++) {
//		for(auto x = x0; x < x1; x++)
//			setfloor(geti(x, y), v);
//	}
//}