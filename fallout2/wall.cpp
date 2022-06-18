#include "draw.h"
#include "main.h"
#include "my_initialize_list.h"

namespace {
enum direction : unsigned char {
	ST, RT, RD, RD2, RU, RS, DD, DDS
};
typedef std::initializer_list<direction> directiona;
typedef std::initializer_list<short> indexa;
struct wallgroup {
	typedef void(*fnproc)(point& h);
	const char*		id;
	indexa			indecies;
	directiona		elements;
	fnproc			pbegin;
};
struct wallcomplex {
	const char*		id;
	short			start;
	directiona		elements;
};
static void oddh(point& h) {
	h.x = ((short unsigned)h.x & 0xFFFE) + 1;
	h.y = ((short unsigned)h.y & 0xFFFE) + 1;
}
static bool show_index = true;
}

BSDATA(wallgroup) = {
	{"WallH", {1, 2, 4, 6, 8, 23, 96, 98, 100, 102, 130}, {ST, RU}, oddh},
	{"WallV", {10, 12, 14, 16, 108, 110, 112, 114, 120}, {ST, DD}},
	{"DoorH3", {39, 42}, {ST, RU, RS, RT}, oddh},
	{"DoorV3", {104, 131}, {ST, DD, DDS, DD}},
	{"WindowH", {50}, {ST, RU, RD, RU, RD, RU}},
};
BSDATAF(wallgroup)

BSDATA(wallcomplex) = {
	{"Bar", 467, {ST, DD, DD, DD, DD, RT, RD, RU, RD, RU}},
	{"Bar", 477, {ST, RT, RT, RT, RT, RT, RD2, DD, DD, RT}},
	{"HanharR", 251, {ST, DD, DD, DD, DD, DD, DD, DD, DD, DD, DD, RT, RT, DD, RU, DD, RT}},
	{"HanharL", 268, {ST, DD, RU, DD, DD, DD, DD, DD, DD, DD, DD, DD, DD}},
};
BSDATAF(wallcomplex)

walli* walli::findindex(short i) {
	return bsdata<walli>::elements + (i - 1);
}

short walli::next(short i) {
	switch(i) {
	case 1: return 3;
	case 18: return 21;
	case 48: return 46;
	case 105: return 97;
	case 124: return 129;
	case 132: return 129;
	case 130: return 129;
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

static bool have(const std::initializer_list<short>& source, short value) {
	for(auto v : source) {
		if(v == value)
			return true;
	}
	return false;
}

static void adds(point pt, short v) {
	auto pd = drawable::find(pt);
	auto p = bsdata<walli>::elements + (v - 1);
	if(!pd)
		pd = drawable::add(pt, p);
	pd->data = p;
	pd->frame = p->frame;
	pd->frame_stop = pd->frame;
}

static void next(point& h, direction d) {
	switch(d) {
	case ST:
		break;
	case RT: case RS:
		h.x++;
		break;
	case RD:
		h.y++;
		h.x++;
		break;
	case RD2:
		h.y+=2;
		break;
	case RU:
		h.y--;
		h.x++;
		break;
	case DD: case DDS:
		h.y++;
		break;
	default:
		break;
	}
}

static void place(point h0, short& v, const directiona& elements) {
	point h = {};
	auto s0 = h2s(h0);
	for(auto d : elements) {
		next(h, d);
		if(d == RS || d == DDS)
			continue;
		adds(s0 + h2s(h), v);
		v = walli::next(v);
	}
}

static void textac(const char* format) {
	auto push_caret = draw::caret;
	draw::caret.x -= draw::textw(format) / 2;
	draw::caret.y -= draw::texth() / 2;
	draw::text(format);
	draw::caret = push_caret;
}

static void textac(point h) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i,%2i", h.x, h.y);
	textac(temp);
}

static void paint(point h0, short& v, const directiona& elements) {
	point h = {};
	for(auto d : elements) {
		next(h, d);
		if(d == RS || d == DDS)
			continue;
		auto push_caret = draw::caret;
		draw::caret = h2s(h) + draw::caret;
		bsdata<walli>::elements[v - 1].paint();
		if(draw::hot.key == 'I')
			draw::execute(draw::cbsetbool, show_index ? 0 : 1, 0, &show_index);
		if(show_index)
			textac(h);
		draw::caret = push_caret;
		v = walli::next(v);
	}
}

static wallgroup* findgroup(short id) {
	for(auto& e : bsdata<wallgroup>()) {
		if(have(e.indecies, id))
			return &e;
	}
	return 0;
}

static wallcomplex* findblock(short id) {
	for(auto& e : bsdata<wallcomplex>()) {
		if(e.start == id)
			return &e;
	}
	return 0;
}

void walli::set(point h, short v) {
	auto pg = findgroup(v);
	if(pg) {
		if(pg->pbegin)
			pg->pbegin(h);
		place(h, v, pg->elements);
		return;
	}
	auto pb = findblock(v);
	if(pb) {
		auto v = pb->start;
		place(h, v, pb->elements);
		return;
	}
	adds(h2s(h), v);
}

void paint_editor(point h, const walli* p) {
	auto pg = findgroup(p->index);
	if(pg) {
		auto v = p->index;
		paint(h, v, pg->elements);
		return;
	}
	auto pb = findblock(p->index);
	if(pb) {
		auto v = pb->start;
		paint(h, v, pb->elements);
		return;
	}
	auto push_caret = draw::caret;
	draw::caret = h2s(h) - draw::camera;
	p->paint();
	push_caret = push_caret;
}

size_t filter_walls(void** pbegin) {
	auto ps = pbegin;
	for(auto& we : bsdata<wallgroup>()) {
		for(auto id : we.indecies)
			*ps++ = (void*)(bsdata<walli>::elements + (id - 1));
	}
	for(auto& we : bsdata<wallcomplex>())
		*ps++ = (void*)(bsdata<walli>::elements + (we.start - 1));
	return ps - pbegin;
}