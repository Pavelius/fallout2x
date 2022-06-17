#include "draw.h"
#include "drawable.h"
#include "main.h"

using namespace draw;

BSDATAC(drawable, 2048)
static adat<drawable*, 1024> drawables;

static int compare(const void* v1, const void* v2) {
	auto p1 = *((drawable**)v1);
	auto p2 = *((drawable**)v2);
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
	drawables.count = ps - drawables.data;
}

static void sort_drawables() {
	qsort(drawables.data, drawables.count, sizeof(drawables.data[0]), compare);
}

static void cleanup_drawables() {
}

void paint_drawables() {
	rectpush push;
	prepare_drawables();
	sort_drawables();
	for(auto p : drawables) {
		caret = p->position - camera;
		p->paint();
	}
}

drawable* drawable::add(point pt, void* object) {
	auto p = bsdata<drawable>::add();
	p->position = pt;
	p->data = object;
	p->flags = 0;
	p->frame = 0;
	p->frame_stop = 0;
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