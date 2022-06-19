#include "main.h"

const int chance_alternate_tile = 10;

bool terrain::have(short v) const {
	if(isalternate(v))
		return true;
	for(auto i : blocks) {
		if(i == v)
			return true;
	}
	return false;
}

short terrain::random() const {
	if(alternate[0] && d100() < chance_alternate_tile) {
		adat<short, 4> source;
		for(auto v : alternate) {
			if(v)
				source.add();
		}
		return source.data[rand() % source.getcount()];
	}
	return blocks[Center];
}

bool terrain::isalternate(short v) const {
	return alternate[0] == v
		|| alternate[1] == v
		|| alternate[2] == v
		|| alternate[3] == v;
}

bool terrain::iscentral(short v) const {
	return blocks[Center] == v
		|| isalternate(v);
}

const terrain* terrain::find(short v) {
	for(auto& e : bsdata<terrain>()) {
		if(e.have(v))
			return &e;
	}
	return 0;
}

static short checked(short t1, short t) {
	return t1 ? t1 : t;
}

void terrain::correct(indext i) {
	static direction_s all[] = {Up, Right, Down, Left};
	unsigned char c = 0;
	auto t = loc.getfloor(i);
	auto p = find(t);
	if(!p)
		return;
	for(auto d : all) {
		c <<= 1;
		auto i1 = loc.tot(i, d);
		if(i1 == Blocked)
			continue;
		auto v = loc.getfloor(i1);
		if(p->have(v))
			c |= 1;
	}
	loc.setfloor(i, p->correct(t, c));
}

void terrain::correctaround(indext i) {
	static direction_s all[] = {Up, RightUp, Right, RightDown, Down, LeftDown, Left, LeftUp};
	for(auto d : all) {
		auto i1 = loc.tot(i, d);
		if(i1 == Blocked)
			continue;
		correct(i);
	}
}

void terrain::correct() {
	for(short y = 0; y < 100; y++)
		for(short x = 0; x < 100; x++)
			correct(t2i({x, y}));
}

short terrain::correct(short t, unsigned char c) const {
	const unsigned char CUp = 8;
	const unsigned char CRight = 4;
	const unsigned char CDown = 2;
	const unsigned char CLeft = 1;
	switch(c) {
	case 0:
		return t;
	case CUp | CRight | CDown:
		return checked(blocks[Left], t);
	case CUp | CLeft | CDown:
		return checked(blocks[Right], t);
	case CLeft | CRight | CDown:
		return checked(blocks[Up], t);
	case CLeft | CRight | CUp:
		return checked(blocks[Down], t);
	case CDown | CLeft:
		return checked(blocks[RightUp], t);
	case CUp | CLeft:
		return checked(blocks[RightDown], t);
	case CUp | CRight:
		return checked(blocks[LeftDown], t);
	case CDown | CRight:
		return checked(blocks[LeftUp], t);
	default:
		if(iscentral(t))
			return t;
		return random();
	}
}