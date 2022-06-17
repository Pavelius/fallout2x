#include "area.h"
#include "crt.h"

void areai::clear() {
	memset(this, 0, sizeof(*this));
}

indext h2i(point h) {
	if((unsigned)h.x >= mps * 2 || (unsigned)h.y >= mps * 2)
		return Blocked;
	return h.y * mps * 2 + h.x;
}

//void areai::set(indext i, int v, int w, int h) {
//	auto x0 = getx(i), y0 = gety(i);
//	auto x1 = x0 + w, y1 = y0 + h;
//	for(auto y = y0; y < y1; y++) {
//		for(auto x = x0; x < x1; x++)
//			setfloor(geti(x, y), v);
//	}
//}