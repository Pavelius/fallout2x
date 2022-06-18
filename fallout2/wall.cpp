#include "main.h"

static short horiz_wall[] = {1, 2, 4, 6, 8, 23};
static short vert_wall[] = {10, 12, 14, 16};
static short horiz_door3[] = {39, 42};
static short horiz_window6[] = {50};

const walli* walli::last;

BSDATA(walli::doori) = {
	{38, 3},
	{41, 3},
	{44, 3, true},
	{47, 3, true},
};

walli* walli::findindex(short i) {
	return bsdata<walli>::elements + (i - 1);
}

short walli::next(short i) {
	switch(i) {
	case 1: return 3;
	case 18: return 21;
	case 48: return 46;
	default: return i + 1;
	}
}

const walli* walli::next() const {
	return bsdata<walli>::elements + next(index) - 1;
}

static bool have(slice<short> source, short value) {
	for(auto v : source) {
		if(v == value)
			return true;
	}
	return false;
}

static void add(point h, short v) {
	auto pt = h2s(h);
	auto pd = drawable::find(pt);
	auto p = bsdata<walli>::elements + (v - 1);
	if(!pd)
		pd = drawable::add(pt, p);
	pd->data = p;
	pd->frame = p->frame;
	pd->frame_stop = pd->frame;
}

static point nexth(point h) {
	h.x++; return h;
}

static point nextv(point h) {
	h.y++; return h;
}

static point oddh(point h) {
	h.x = ((short unsigned)h.x & 0xFFFE) + 1;
	h.y = ((short unsigned)h.y & 0xFFFE) + 1;
	return h;
}

void walli::set(point h, short v) {
	if(have(horiz_wall, v)) {
		h = oddh(h);
		add(h, v);
		add(nexth(h), next(v));
	} else if(have(vert_wall, v)) {
		add(h, v);
		add(nextv(h), next(v));
	} else
		add(h, v);
}