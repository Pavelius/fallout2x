#include "crt.h"
#include "draw.h"
#include "floatstring.h"

BSDATAC(floatstring, 128)

using namespace draw;

extern unsigned long current_tick;

void floatstring::clear() {
	memset(this, 0, sizeof(*this));
}

void floatstring::paint() const {
	auto push_fore = fore;
	fore = color(255, 255, 255);
	caret.x -= textw(text) / 2;
	draw::text(text);
	fore = push_fore;
}

static void cleanup() {
	auto pb = bsdata<floatstring>::begin();
	auto p = bsdata<floatstring>::end() - 1;
	while(p >= pb) {
		if(*p)
			break;
		p--;
	}
	bsdata<floatstring>::source.count = p - pb + 1;
}

void update_floatstring() {
	for(auto& e : bsdata<floatstring>()) {
		if(!e)
			continue;
		if(e.stop < current_tick)
			e.clear();
	}
	cleanup();
}

void addstring(point position, const char* string, unsigned long delay) {
	auto p = bsdata<floatstring>::add();
	p->position = position;
	p->text = string;
	p->stop = current_tick + delay;
}