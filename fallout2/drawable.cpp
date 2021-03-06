#include "crt.h"
#include "draw.h"
#include "drawable.h"

using namespace draw;

BSDATAC(drawable, 2048)
static adat<drawable*, 1024> drawables;
drawable::fnevent drawable::paint;
drawable::fnget drawable::getorder;
drawable::fnget drawable::ishilite;
drawable::fnselect drawable::select;

static int compare(const void* v1, const void* v2) {
	auto p1 = *((drawable**)v1);
	auto p2 = *((drawable**)v2);
	auto o1 = drawable::getorder(p1);
	auto o2 = drawable::getorder(p2);
	if(o1 != o2)
		return o1 - o2;
	if(p1->position.y != p2->position.y)
		return p1->position.y - p2->position.y;
	return p1->position.x - p2->position.x;
}

static void prepare_drawables() {
	rect rc = {camera.x - 128, camera.y - 128, camera.x + width + 128, camera.y + height + 128};
	auto pe = drawables.endof();
	auto ps = drawables.data;
	for(auto& e : bsdata<drawable>()) {
		if(!e.position.in(rc))
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	if(drawable::select)
		ps = drawable::select(ps, pe);
	drawables.count = ps - drawables.data;
}

static void sort_drawables() {
	qsort(drawables.data, drawables.count, sizeof(drawables.data[0]), compare);
}

void paint_drawables() {
	const int hs = 64;
	rectpush push;
	prepare_drawables();
	sort_drawables();
	for(auto p : drawables) {
		caret = p->position - camera;
		drawable::paint(p);
		if(drawable::ishilite) {
			rect rc = {caret.x - hs, caret.y - hs, caret.x + hs, caret.y + hs};
			if(hot.mouse.in(rc)) {
				if(drawable::ishilite(p)) {
					hilite_position = caret;
					hilite_object = p;
				}
			}
		}
	}
}

drawable* drawable::add(point pt, const void* object) {
	auto p = bsdata<drawable>::add();
	p->position = pt;
	p->data = object;
	return p;
}

drawable* drawable::find(const void* object) {
	for(auto& e : bsdata<drawable>()) {
		if(e.data == object)
			return &e;
	}
	return 0;
}

drawable* drawable::find(const point pt) {
	for(auto& e : bsdata<drawable>()) {
		if(e.position == pt)
			return &e;
	}
	return 0;
}

drawable* drawable::findadd(const point pt, const void* object) {
	for(auto& e : bsdata<drawable>()) {
		if(e.position == pt && e.data == object)
			return &e;
	}
	return add(pt, object);
}

void drawable::clear() {
	memset(this, 0, sizeof(*this));
}