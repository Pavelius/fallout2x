#include "dialog.h"
#include "main.h"
#include "draw.h"

using namespace draw;

point t2s(point v) {
	return {
		(short)(32 * v.y + 48 * v.x),
		(short)(24 * v.y - 12 * v.x)
	};
}

point s2t(point pt) {
	int x = pt.x + 40;
	int y = pt.y + 26;
	return{(short)((x - 4 * y / 3) / 64), (short)((x + 4 * y) / 128)};
}

point h2s(point v) {
	return{
		(short)(16 * (v.y + v.x * 2 - v.x / 2)),
		(short)(12 * (v.y - v.x / 2))
	};
}

point s2h(point pt) {
	int x1 = pt.x + 15;
	int y1 = pt.y + 7;
	int nx;
	if(x1 < 0)
		nx = (x1 + 1) / 16 - 1;
	else
		nx = x1 / 16;
	int ny;
	if(y1 < 0)
		ny = (y1 + 1) / 12 - 1;
	else
		ny = y1 / 12;
	if(iabs(nx) % 2 != iabs(ny) % 2)
		nx--;
	long xhBase = 16 * nx;
	long yhBase = 12 * ny;
	int x = (4 * yhBase - 3 * xhBase) / 96;
	int y = yhBase / 12 - x / 2;
	return{(short)-x, (short)y};
}

static void marker() {
	auto push_caret = caret;
	caret.x -= 4; line(caret.x + 8, caret.y);
	caret = push_caret;
	caret.y -= 4; line(caret.x, caret.y + 8);
	caret = push_caret;
}

void sceneryi::paint() const {
	auto rs = gres(res::SCENERY);
	image(rs, rs->ganim(frame, current_tick / 200), 0);
}

void walli::paint() const {
	auto rs = gres(res::WALLS);
	image(rs, rs->ganim(frame, current_tick / 200), 0);
}

void animable::paint() const {
	auto rs = gres(getlook());
	if(!rs)
		return;
	image(rs, frame, flags);
}

void tilei::paint() const {
	auto rs = gres(res::TILES);
	image(rs, rs->ganim(frame, current_tick / 200), 0);
}

static void paint_drawable(const drawable* p) {
	if(bsdata<sceneryi>::have(p->data))
		((sceneryi*)p->data)->paint();
	else if(bsdata<walli>::have(p->data))
		((walli*)p->data)->paint();
	else if(bsdata<character>::have(p->data))
		((character*)p->data)->paint();
}

static int getorder(const drawable* p) {
	if(bsdata<sceneryi>::have(p->data))
		return ((sceneryi*)p->data)->is(Flat) ? 0 : 1;
	return 1;
}

static drawable** select_drawables(drawable** ps, drawable* const* pe) {
	for(auto& e : bsdata<character>()) {
		if(ps < pe)
			*ps++ = &e;
	}
	return ps;
}

void initialize_adventure() {
	drawable::paint = paint_drawable;
	drawable::getorder = getorder;
	drawable::select = select_drawables;
}

void update_animation();

static void open_dialog() {
	opendialog((const char*)hot.param);
}

static void adventure_hotkey() {
	switch(hot.key) {
	case 'I': execute(open_dialog, (long)"CharacterInvertory"); break;
	default: break;
	}
}