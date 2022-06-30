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
	if(!this->text)
		return;
	rectpush push;
	auto push_fore = fore;
	width = height = 200;
	caret.x -= width / 2;
	caret.y -= height / 2;
	fore = color(255, 255, 255);
	texta(text, AlignCenterCenter);
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

static floatstring* findstring(const void* data) {
	for(auto& e : bsdata<floatstring>()) {
		if(e.data == data)
			return &e;
	}
	return 0;
}

void addstring(point position, const char* string, unsigned long delay, void* data) {
	floatstring* p = 0;
	if(data)
		p = findstring(data);
	if(!p)
		p = bsdata<floatstring>::add();
	p->position = position;
	p->text = string;
	p->stop = current_tick + delay;
	p->data = data;
}