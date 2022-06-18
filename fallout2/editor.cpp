#include "draw.h"
#include "main.h"

using namespace draw;

typedef size_t(*fnfilter)(void** result);

static array* choose_source;
const void* current_tool;
static bool show_tile_index = false;
static int list_origin;

extern indext current_hexagon;

void cancel_hotkey();
void control_map();
size_t filter_walls(void** pbegin);
void paint_drawables();
void paint_editor(point h, const walli* p);
void redraw_floor();
void redraw_hexagon();

void add_object(point h, const void* p, short frame) {
	auto pt = h2s(h);
	auto pd = drawable::find(pt);
	if(!pd)
		pd = drawable::add(pt, p);
	pd->data = p;
	pd->frame = frame;
	pd->frame_stop = pd->frame;
}

static void textac(const char* format) {
	auto push_caret = caret;
	caret.x -= textw(format) / 2;
	caret.y -= texth() / 2;
	text(format);
	caret = push_caret;
}

static point horz(point h) {
	h.x++;
	return h;
}

static point vert(point h) {
	h.y++;
	return h;
}

static void place_tool() {
	if(current_hexagon == Blocked)
		return;
	auto h = i2h(current_hexagon);
	if(bsdata<sceneryi>::have(hot.object))
		add_object(h, hot.object, ((sceneryi*)hot.object)->frame);
	if(bsdata<walli>::have(hot.object))
		walli::set(h, ((walli*)hot.object)->index);
	else if(bsdata<tilei>::have(hot.object)) {
		auto p = (tilei*)hot.object;
		auto i = t2i(h2t(i2h(current_hexagon)));
		loc.setfloor(i, getbsi(p));
	}
}

static void paint_tool() {
	if(bsdata<sceneryi>::have(current_tool))
		((sceneryi*)current_tool)->paint();
	else if(bsdata<walli>::have(current_tool))
		paint_editor(i2h(current_hexagon), (walli*)current_tool);
	else if(bsdata<tilei>::have(current_tool)) {
		caret = t2s(h2t(i2h(current_hexagon))) - camera;
		caret.x += 8;
		caret.y += 26;
		((tilei*)current_tool)->paint();
	}
}

void redraw_select_tool() {
	if(current_hexagon == Blocked)
		return;
	auto push_caret = caret;
	caret = h2s(i2h(current_hexagon)) - camera;
	if(current_tool) {
		paint_tool();
		if(hot.key == MouseLeft && hot.pressed)
			execute(place_tool, 0, 0, current_tool);
	}
	caret = push_caret;
}

static void tile_hotkey() {
	switch(hot.key) {
	case 'I': execute(cbsetbool, show_tile_index ? 0 : 1, 0, &show_tile_index); break;
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

static void choose_list_element(const void* object) {
	auto push_clipping = clipping;
	clipping.set(caret.x, caret.y, caret.x + width, caret.y + height);
	auto push_caret = caret;
	caret.x += width / 2;
	caret.y += height - 32;
	if(bsdata<sceneryi>::have(object))
		((sceneryi*)object)->paint();
	else if(bsdata<walli>::have(object))
		((walli*)object)->paint();
	caret = push_caret;
	clipping = push_clipping;
	if(ishilite()) {
		rectb();
		hilite_object = object;
		if(hot.key == MouseLeft && !hot.pressed)
			execute(buttonparam, (long)object);
	}
	caret.y += height - 32;
	texta(((nameable*)object)->getname(), AlignCenter);
	caret = push_caret;
}

static fnfilter choose_filter, choose_filter_value;

static void choose_list() {
	static void* source_data[1024];
	rectpush push;
	width = 128; height = 128;
	auto dx = push.width / (width + metrics::padding);
	auto dy = push.height / (height + metrics::padding);
	size_t maximum = 0;
	if(choose_filter)
		maximum = choose_filter(source_data);
	else
		maximum = choose_source->getcount();
	list_input(list_origin, dx * dy, dx, maximum);
	for(size_t i = list_origin; i < maximum; i++) {
		if(caret.x + width > push.width) {
			caret.y += height + metrics::padding;
			caret.x = push.caret.x;
		}
		if(caret.y + height > push.height)
			break;
		if(choose_filter)
			choose_list_element(source_data[i]);
		else
			choose_list_element(choose_source->ptr(i));
		caret.x += width + metrics::padding;
	}
}

static void status_text() {
	char temp[2048]; stringbuilder sb(temp); sb.clear();
	if(bsdata<sceneryi>::have(hilite_object))
		((sceneryi*)hilite_object)->getinfoed(sb);
	else if(bsdata<walli>::have(hilite_object))
		((walli*)hilite_object)->getinfoed(sb);
	if(temp[0])
		texta(temp, AlignCenterCenter);
}

static void status_window() {
	auto push_fore = fore;
	height = texth() * 4 + 2;
	width = width - 1;
	caret.x = 0;
	caret.y = getheight() - height - 1;
	fore = getcolor(ColorInfo);
	rectf();
	fore = getcolor(ColorButton);
	rectb();
	fore = push_fore;
}

static void choose_filter_set() {
	if(choose_filter)
		choose_filter = 0;
	else
		choose_filter = choose_filter_value;
}

static void choose_hotkey() {
	switch(hot.key) {
	case 'F': execute(choose_filter_set); break;
	default: break;
	}
}

static void common_scene() {
	auto push_fore = fore;
	fore = getcolor(ColorDisable);
	rectf();
	caret.x += metrics::padding;
	caret.y += metrics::padding;
	fore = getcolor(ColorCheck);
	choose_list();
	fore = push_fore;
	status_window();
	status_text();
	choose_hotkey();
	cancel_hotkey();
}

static void tile_list() {
	char temp[260]; stringbuilder sb(temp);
	const auto max_width = 16;
	auto need_break = false;
	auto pr = gres(res::TILES);
	auto count = bsdata<tilei>::source.count;
	auto push_caret = caret;
	auto px = hot.mouse + camera; px.y -= tile_height / 2;
	auto pt = s2t(px);
	auto hilite = 0;
	if(pt.x < 0 || pt.y<0 || pt.x > max_width)
		hilite = 0;
	else
		hilite = pt.y * max_width + pt.x;
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
			auto& ei = bsdata<tilei>::elements[tv];
			image(caret.x, caret.y + tile_height / 2, pr, pr->ganim(ei.frame, current_tick / 200), 0);
			if(show_tile_index) {
				sb.clear(); sb.add("%1i", tv);
				textac(temp);
			}
		}
	}
	if(hilite) {
		short x = hilite % max_width;
		short y = hilite / max_width;
		caret = t2s({x, y}) - camera;
		image(caret.x, caret.y + tile_height / 2, pr, pr->ganim(222, current_tick / 200), 0);
		if(hot.key == MouseLeft && !hot.pressed)
			execute(buttonparam, (long)(bsdata<tilei>::elements + hilite));
	}
	caret = push_caret;
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

static void choose_scenery() {
	static int origin;
	list_origin = origin;
	choose_source = bsdata<sceneryi>::source_ptr;
	scene(common_scene);
	if(getresult()) {
		origin = list_origin;
		current_tool = (void*)getresult();
	}
}

//static void choose_wallblock() {
//	static int origin;
//	list_origin = origin;
//	choose_source = bsdata<wallblock>::source_ptr;
//	scene(common_scene);
//	if(getresult()) {
//		origin = list_origin;
//		current_tool = (void*)getresult();
//	}
//}

static void choose_wall() {
	static int origin;
	list_origin = origin;
	choose_filter_value = filter_walls;
	choose_source = bsdata<walli>::source_ptr;
	scene(common_scene);
	if(getresult()) {
		origin = list_origin;
		current_tool = (walli*)getresult();
	}
}

static void choose_tile() {
	static point tile_camera;
	auto push_camera = camera;
	camera = tile_camera;
	scene(tile_scene);
	tile_camera = camera;
	camera = push_camera;
	if(getresult()) {
		tilei::last = (tilei*)getresult();
		current_tool = tilei::last;
	}
}

static void save_map() {
	loc.write("test");
}

static void read_map() {
	loc.read("test");
}

static void delete_object() {
	if(current_hexagon == Blocked)
		return;
	auto p = drawable::find(h2s(i2h(current_hexagon)));
	if(p)
		p->clear();
}

void editor_hotkey() {
	switch(hot.key) {
	case KeyDelete: execute(delete_object); break;
	case 'S': execute(choose_scenery); break;
	case Ctrl + 'S': execute(save_map); break;
	case Ctrl + 'R': execute(read_map); break;
	case 'T': execute(choose_tile); break;
	case 'W': execute(choose_wall); break;
	}
}

static void status_text_main() {
	char temp[2048]; stringbuilder sb(temp); sb.clear();
	if(current_hexagon != Blocked) {
		auto h = i2h(current_hexagon);
		sb.add("(%1i %2i %3i)", h.x, h.y, current_hexagon);
	}
	sb.adds("Drawables %1i", bsdata<drawable>::source.getcount());
	if(temp[0])
		texta(temp, AlignCenterCenter);
}

static void set_hexagon_position() {
	static rect play_area = {0, 0, width, height - texth() * 4 - 2};
	current_hexagon = Blocked;
	if(hot.mouse.in(play_area))
		current_hexagon = h2i(s2h(hot.mouse + camera));
}

void editor() {
	set_hexagon_position();
	control_map();
	redraw_floor();
	redraw_hexagon();
	paint_drawables();
	redraw_select_tool();
	status_window();
	status_text_main();
	editor_hotkey();
}