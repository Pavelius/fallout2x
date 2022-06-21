#include "dialog.h"
#include "main.h"
#include "draw.h"

using namespace draw;

indext current_hexagon;
static rect play_area = {0, 0, 640, 480 - 99};

void editor_hotkey();
void paint_drawables();
void paint_drawables();
void redraw_select_tool();

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

void cancel_hotkey() {
	if(hot.key == KeyEscape)
		execute(buttoncancel);
}

static void addpoint() {
	auto p = (point*)hot.object;
	p->x += (short)hot.param;
	p->y += (short)hot.param2;
}

static void scrollmap(int x, int y, int cicle) {
	const int w = 640;
	const int h = 480;
	const int s = 4;
	const int dx = 16;
	const int dy = 12;
	rect rc = {};
	if(x == 0) {
		rc.x1 = s + 1;
		rc.x2 = w - s - 1;
	} else {
		rc.x1 = (x < 0) ? 0 : w - s;
		rc.x2 = rc.x1 + s;
	}
	if(y == 0) {
		rc.y1 = s + 1;
		rc.y2 = h - s - 1;
	} else {
		rc.y1 = (y < 0) ? 0 : h - s;
		rc.y2 = rc.y1 + s;
	}
	if(hot.mouse.in(rc)) {
		current_hexagon = Blocked;
		cursor.set(res::INTRFACE, cicle);
		if(hot.key == InputTimer)
			execute(addpoint, x * dx, y * dy, &camera);
	}
}

void redraw_floor() {
	auto ps = gres(res::TILES);
	if(!ps)
		return;
	auto tm = current_tick;
	rect rc = {-tile_width, -tile_height, 640 + tile_width, 480 + tile_height};
	for(short y = 0; y < 100; y++) {
		for(short x = 0; x < 100; x++) {
			caret = t2s({x, y}) - camera;
			caret.x += 8;
			caret.y += 26;
			if(caret.in(rc)) {
				auto tv = bsdata<tilei>::elements[loc.getfloor(t2i({x, y}))].frame;
				if(tv > 1)
					draw::image(ps, ps->ganim(tv, tm), 0);
			}
		}
	}
}

void control_map() {
	const int dx = 16;
	const int dy = 12;
	switch(hot.key) {
	case KeyLeft: execute(addpoint, -dx, 0, &camera); break;
	case KeyRight: execute(addpoint, dx, 0, &camera); break;
	case KeyUp: execute(addpoint, 0, -dy, &camera); break;
	case KeyDown: execute(addpoint, 0, dy, &camera); break;
	}
	scrollmap(-1, -1, 270);
	scrollmap(0, -1, 271);
	scrollmap(1, -1, 272);
	scrollmap(1, 0, 273);
	scrollmap(1, 1, 274);
	scrollmap(0, 1, 275);
	scrollmap(-1, 1, 276);
	scrollmap(-1, 0, 277);
}

void set_hexagon_position() {
	current_hexagon = Blocked;
	if(hot.mouse.in(play_area))
		current_hexagon = h2i(s2h(hot.mouse + camera));
}

void redraw_hexagon() {
	if(current_hexagon == Blocked)
		return;
	cursor.clear();
	auto pt = h2s(i2h(current_hexagon)) - camera;
	image(pt.x - 15, pt.y - 7, gres(res::INTRFACE), 1, 0);
}

void sceneryi::paint() const {
	auto rs = gres(res::SCENERY);
	image(rs, rs->ganim(frame, current_tick / 200), 0);
}

void walli::paint() const {
	auto rs = gres(res::WALLS);
	image(rs, rs->ganim(frame, current_tick / 200), 0);
}

static void marker() {
	auto push_caret = caret;
	caret.x -= 4; line(caret.x + 8, caret.y);
	caret = push_caret;
	caret.y -= 4; line(caret.x, caret.y + 8);
	caret = push_caret;
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

void adventure() {
	update_animation();
	set_hexagon_position();
	control_map();
	redraw_floor();
	redraw_hexagon();
	paint_drawables();
	adventure_hotkey();
}