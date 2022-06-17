#include "main.h"
#include "draw.h"

using namespace draw;

static bool show_tile_index = false;
static indext current_hexagon;
static rect play_area = {0, 0, 640, 480 - 99};

void paint_drawables();

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

static void cancel_hotkey() {
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

void list_input(int& origin, int perpage, int perline, int maximum) {
	auto maximum_row = (maximum / perline) * perline;
	if(origin + perline > maximum_row)
		origin = maximum_row;
	switch(hot.key) {
	case KeyUp:
	case MouseWheelUp:
		if(origin)
			execute(cbsetint, origin - perline, 0, &origin);
		break;
	case MouseWheelDown:
	case KeyDown:
		if(origin < maximum_row)
			execute(cbsetint, origin + perline, 0, &origin);
		break;
	case KeyHome:
		if(origin)
			execute(cbsetint, 0, 0, &origin);
		break;
	case KeyEnd:
		if(origin != maximum_row)
			execute(cbsetint, maximum_row, 0, &origin);
		break;
	case KeyPageUp:
		if(origin)
			execute(cbsetint, origin - perpage, 0, &origin);
		break;
	case KeyPageDown:
		execute(cbsetint, origin + perpage, 0, &origin);
		break;
	}
}

static void textac(const char* format) {
	auto push_caret = caret;
	caret.x -= textw(format) / 2;
	caret.y -= texth() / 2;
	text(format);
	caret = push_caret;
}

static void redraw_floor() {
	auto ps = gres(res::TILES);
	if(!ps)
		return;
	auto tm = current_tick;
	rect rc = {-tile_width, -tile_height, 640 + tile_width, 480 + tile_height};
	int xx = 8 - camera.x;
	int yy = 26 - camera.y;
	for(auto y = 0; y < 100; y++) {
		auto prev_xx = xx;
		auto prev_yy = yy;
		for(auto x = 0; x < 100; x++) {
			point pt = {(short)xx, (short)yy};
			if(pt.in(rc)) {
				auto tv = 2; //loc.getfloor(loc.geti(x, y));
				if(tv > 1)
					draw::image(xx, yy, ps, ps->ganim(tv, tm), 0);
			}
			xx += 48;
			yy -= 12;
		}
		xx = prev_xx + 32;
		yy = prev_yy + 24;
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

static void set_hexagon_position() {
	current_hexagon = Blocked;
	if(hot.mouse.in(play_area))
		current_hexagon = h2i(s2h(hot.mouse + camera));
}

static void redraw_hexagon() {
	if(current_hexagon == Blocked)
		return;
	cursor.clear();
	auto pt = h2s(i2h(current_hexagon)) - camera;
	image(pt.x - 15, pt.y - 7, gres(res::INTRFACE), 1, 0);
}

static void place_tool() {
	if(current_hexagon == Blocked)
		return;
	auto pt = h2s(i2h(current_hexagon));
	if(bsdata<scenery>::have(hot.object)) {
		auto p = (scenery*)hot.object;
		auto pd = drawable::find(pt);
		if(!pd)
			pd = drawable::add(pt, p);
		pd->data = p;
		pd->frame = p->frame;
		pd->frame_stop = pd->frame;
	}
}

static void redraw_select_tool() {
	if(current_hexagon == Blocked)
		return;
	auto pt = h2s(i2h(current_hexagon)) - camera;
	if(scenery::last) {
		auto rs = gres(res::SCENERY);
		image(pt.x, pt.y, gres(res::SCENERY), rs->ganim(scenery::last->frame, current_tick / 200), 0);
		if(hot.key == MouseLeft && hot.pressed)
			execute(place_tool, 0, 0, scenery::last);
	}
}

static void scenery_list() {
	static int origin;
	rectpush push;
	width = 128; height = 128;
	auto dx = push.width / (width + metrics::padding);
	auto dy = push.height / (height + metrics::padding);
	auto maximum = bsdata<scenery>::source.getcount();
	list_input(origin, dx * dy, dx, maximum);
	for(size_t i = origin; i < maximum; i++) {
		if(caret.x + width > push.width) {
			caret.y += height + metrics::padding;
			caret.x = push.caret.x;
		}
		if(caret.y + height > push.height)
			break;
		bsdata<scenery>::elements[i].painted();
		caret.x += width + metrics::padding;
	}
}

static void horizline() {
	auto push_caret = caret;
	line(caret.x + width, caret.y);
	caret = push_caret;
}

static void status_text() {
	char temp[2048]; stringbuilder sb(temp); sb.clear();
	if(bsdata<scenery>::have(hilite_object))
		((scenery*)hilite_object)->getinfo(sb);
	if(temp[0])
		texta(temp, AlignCenterCenter);
}

static void status_bar() {
	auto push_fore = fore;
	height = texth() * 4;
	width = width - 1;
	caret.x = 0;
	caret.y = getheight() - height - 1;
	fore = getcolor(ColorInfo);
	rectf();
	fore = getcolor(ColorButton);
	rectb();
	fore = getcolor(ColorText);
	status_text();
	fore = push_fore;
}

static void scenery_scene() {
	fore = getcolor(ColorDisable);
	rectf();
	caret.x += metrics::padding;
	caret.y += metrics::padding;
	fore = getcolor(ColorCheck);
	scenery_list();
	status_bar();
}

static void tile_list() {
	char temp[260]; stringbuilder sb(temp);
	const auto max_width = 16;
	auto need_break = false;
	auto pr = gres(res::TILES);
	auto count = pr->cicles;
	auto push_caret = caret;
	for(auto y = 0; true; y++) {
		if(need_break)
			break;
		for(int x = 0; x < max_width; x++) {
			unsigned tv = y * max_width + x;
			if(!tv)
				continue;
			if(tv >= count) {
				need_break = true;
				break;
			}
			caret = t2s({(short)x, (short)y}) - camera;
			auto cicle = y * max_width + x;
			image(caret.x, caret.y + tile_height / 2, pr, pr->ganim(cicle, current_tick / 200), 0);
			if(show_tile_index) {
				sb.clear(); sb.add("%1i", tv);
				textac(temp);
			}
			//if(hilite == tv)
			//	image(pz.x, pz.y + tile_height / 2, ps, ps->ganim(222, tm), 0);
		}
	}
	caret = push_caret;
}

static void tile_hotkey() {
	switch(hot.key) {
	case 'I': execute(cbsetbool, show_tile_index ? 0 : 1, 0, &show_tile_index); break;
	}
}

static void tile_scene() {
	fore = getcolor(ColorDisable);
	rectf();
	fore = getcolor(ColorText);
	tile_list();
	control_map();
	tile_hotkey();
	cancel_hotkey();
}

void choose_scenery() {
	scene(scenery_scene);
	scenery::last = (scenery*)getresult();
}

void choose_tile() {
	auto push_camera = camera;
	scene(tile_scene);
	camera = push_camera;
}

static void press_hotkey() {
	switch(hot.key) {
	case 'S':
		choose_scenery();
		break;
	case 'T':
		choose_tile();
		break;
	}
}

void scenery::paint() const {
	auto rs = gres(res::SCENERY);
	image(gres(res::SCENERY), rs->ganim(frame, current_tick / 200), 0);
}

void drawable::paint() const {
	if(bsdata<scenery>::have(data))
		((scenery*)data)->paint();
}

void adventure() {
	set_hexagon_position();
	control_map();
	redraw_floor();
	redraw_hexagon();
	paint_drawables();
	redraw_select_tool();
	press_hotkey();
}