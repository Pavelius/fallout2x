#include "draw.h"
#include "io_stream.h"
#include "main.h"

using namespace draw;

typedef size_t(*fnfilter)(void** result);

static array* choose_source;
const void* current_tool;
static bool show_tile_index = false;
static int list_origin;
static point border[2];

extern indext current_hexagon;

void control_map();
size_t filter_walls(void** pbegin);
void paint_drawables();
void paint_editor(point h, const walli* p);
void redraw_floor();
void redraw_hexagon();

static void cancel_hotkey() {
	if(hot.key == KeyEscape)
		execute(buttoncancel);
}

static void place_editor(point bt, const tilegroup* p) {
	auto index = p->start;
	for(auto& e : *p) {
		point t = bt + e.shift;
		for(auto i = 0; i < e.count; i++) {
			loc.setfloor(t2i(t), index++);
			t.x++;
		}
	}
}

static void paint_editor(const tilegroup* p) {
	auto push_caret = caret;
	auto pr = gres(res::TILES);
	auto index = p->start;
	for(auto& e : *p) {
		caret = push_caret + t2s(e.shift);
		for(auto i = 0; i < e.count; i++) {
			image(pr, bsdata<tilei>::elements[index++].frame, 0);
			caret.x += 48;
			caret.y -= 12;
		}
	}
	caret = push_caret;
}

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
	} else if(bsdata<terrain>::have(hot.object)) {
		auto p = (terrain*)hot.object;
		auto i = t2i(h2t(i2h(current_hexagon)));
		loc.setfloor(i, p->blocks[Center]);
		terrain::correct();
	} else if(bsdata<tilegroup>::have(hot.object)) {
		auto p = (tilegroup*)hot.object;
		place_editor(h2t(i2h(current_hexagon)), p);
	}
}

static void paint_tool() {
	if(bsdata<sceneryi>::have(current_tool))
		((sceneryi*)current_tool)->paint();
	else if(bsdata<walli>::have(current_tool))
		paint_editor(i2h(current_hexagon), (walli*)current_tool);
	else if(bsdata<terrain>::have(current_tool)) {
		caret = t2s(h2t(i2h(current_hexagon))) - camera;
		caret.x += 8;
		caret.y += 26;
		bsdata<tilei>::elements[((terrain*)current_tool)->blocks[Center]].paint();
	} else if(bsdata<tilegroup>::have(current_tool)) {
		caret = t2s(h2t(i2h(current_hexagon))) - camera;
		caret.x += 8;
		caret.y += 26;
		paint_editor((tilegroup*)current_tool);
	} else if(bsdata<tilei>::have(current_tool)) {
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
	else if(bsdata<terrain>::have(object))
		bsdata<tilei>::elements[((terrain*)object)->blocks[Center]].paint();
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
	else if(bsdata<tilegroup>::have(hilite_object))
		((tilegroup*)hilite_object)->getinfoed(sb);
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

static void export_tilegroup() {
	auto p = (const tilegroup*)hot.object;
	const char* lf = "\n";
	io::file es("rules/export.txt", StreamWrite | StreamText);
	if(!es)
		return;
	es << "{";
	es << "\"" << "\"" << ", ";
	es << (int)p->start << ", ";
	es << (int)p->count << ", ";
	es << "{";
	auto need_block_coma = false;
	for(auto& ge : *p) {
		if(need_block_coma)
			es << ", ";
		es << "{";
		es << (int)ge.count << ", ";
		auto pt = ge.shift;
		es << "{" << pt.x << ", " << pt.y << "}";
		es << "}";
		need_block_coma = true;
	}
	es << "}";
	es << "}";
}

static void update_next_tile(tilegroup* p) {
	auto p1 = p + 1;
	if(p1 >= bsdata<tilegroup>::end())
		return;
	p1->start = p->getendid() + 1;
}

static void tilegroup_info(tilegroup* p) {
	static int current_line;
	if(current_line < 0)
		current_line = (int)p->count - 1;
	if(current_line >= (int)p->count)
		current_line = 0;
	rectpush push;
	auto push_fore = fore;
	char temp[512]; stringbuilder sb(temp);
	p->getinfolist(sb);
	caret.x = 2; caret.y = 2;
	auto index = p->start;
	auto line = 0;
	for(auto& e : *p) {
		if(line == current_line)
			fore = getcolor(ColorCheck);
		else
			fore = getcolor(ColorText);
		sb.clear();
		sb.addn("%1i-%2i (%3i,%4i) %6i", index, index + e.count - 1, e.shift.x, e.shift.y, line+1, e.count);
		text(temp); caret.y += texth();
		index += e.count;
		line++;
	}
	switch(hot.key) {
	case KeyUp: execute(cbsetint, current_line - 1, 0, &current_line); break;
	case KeyDown: execute(cbsetint, current_line + 1, 0, &current_line); break;
	case KeyHome: execute(cbsetsht, p->elements[current_line].shift.x - 1, 0, &p->elements[current_line].shift.x); break;
	case KeyEnd: execute(cbsetsht, p->elements[current_line].shift.x + 1, 0, &p->elements[current_line].shift.x); break;
	case KeyPageUp: execute(cbsetsht, p->elements[current_line].shift.y - 1, 0, &p->elements[current_line].shift.y); break;
	case KeyPageDown: execute(cbsetsht, p->elements[current_line].shift.y + 1, 0, &p->elements[current_line].shift.y); break;
	case KeyLeft:
		hot.key = 0;
		if(p->elements[current_line].count > 1) {
			p->elements[current_line].count -= 1;
			update_next_tile(p);
		}
		break;
	case KeyRight:
		hot.key = 0;
		p->elements[current_line].count += 1;
		update_next_tile(p);
		break;
	case KeyTab:
		hot.key = 0;
		if((unsigned)p->count >= sizeof(p->elements) / sizeof(p->elements[0]))
			break;
		p->elements[p->count] = p->elements[p->count - 1];
		p->elements[p->count].shift.y++;
		p->count++;
		update_next_tile(p);
		break;
	case Ctrl + 'S':
		execute(export_tilegroup, 0, 0, p);
		break;
	case KeyDelete:
		hot.key = 0;
		if(p->count > 1) {
			p->count--;
			update_next_tile(p);
		}
		break;
	case 'Q':
		hot.key = 0;
		p->start++;
		update_next_tile(p);
		break;
	case 'A':
		hot.key = 0;
		p->start--;
		update_next_tile(p);
		break;
	default:
		break;
	}
}

static void tilegroup_list() {
	static int index;
	if(index > (int)bsdata<tilegroup>::source.getcount() - 1)
		index = (int)bsdata<tilegroup>::source.getcount() - 1;
	if(index < 0)
		index = 0;
	auto p = bsdata<tilegroup>::elements + index;
	hilite_object = p;
	auto push_caret = caret;
	caret = point{120, 480 / 2} - camera;
	paint_editor(p);
	switch(hot.key) {
	case '-':
		execute(cbsetint, index - 1, 0, &index);
		break;
	case '+':
		execute(cbsetint, index + 1, 0, &index);
		break;
	case KeySpace:
	case KeyEnter:
		execute(buttonparam, (long)p);
		break;
	}
	caret = push_caret;
	tilegroup_info(p);
}

static void textbl(const char* format) {
	auto push_caret = caret;
	caret.x = 2;
	caret.y = getheight() - texth() - 2;
	text(format);
	caret = push_caret;
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

static void choose_terrain() {
	static int origin;
	list_origin = origin;
	choose_source = bsdata<terrain>::source_ptr;
	scene(common_scene);
	if(getresult()) {
		origin = list_origin;
		current_tool = (void*)getresult();
	}
}

static void choose_wall() {
	static int origin;
	list_origin = origin;
	auto push_filter = choose_filter_value;
	choose_filter_value = filter_walls;
	choose_filter = 0;
	choose_source = bsdata<walli>::source_ptr;
	scene(common_scene);
	choose_filter_value = push_filter;
	if(getresult()) {
		origin = list_origin;
		current_tool = (walli*)getresult();
	}
}

static void tilegroup_scene() {
	fore = getcolor(ColorDisable);
	rectf();
	fore = getcolor(ColorText);
	tilegroup_list();
	control_map();
	cancel_hotkey();
}

static void choose_tilegroup() {
	static point loc_camera;
	auto push_camera = camera;
	camera = loc_camera;
	scene(tilegroup_scene);
	if(getresult())
		current_tool = (tilegroup*)getresult();
	loc_camera = camera;
	camera = push_camera;
}

static void choose_tile() {
	static point tile_camera;
	auto push_camera = camera;
	camera = tile_camera;
	scene(tile_scene);
	tile_camera = camera;
	camera = push_camera;
	if(getresult())
		current_tool = (tilei*)getresult();
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
	case 'R': execute(choose_terrain); break;
	case 'S': execute(choose_scenery); break;
	case Ctrl + 'S': execute(save_map); break;
	case Ctrl + 'R': execute(read_map); break;
	case 'T': execute(choose_tile); break;
	case 'W': execute(choose_wall); break;
	case 'G': execute(choose_tilegroup); break;
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

void update_animation();

static void correct(point& p1, point& p2) {
	if(p2.x < p1.x)
		iswap(p2.x, p1.x);
	if(p2.y < p1.y)
		iswap(p2.y, p1.y);
}

static void place_rect() {
	correct(border[0], border[1]);
	auto push_hex = current_hexagon;
	for(short y = border[0].y; y < border[1].y; y++) {
		for(short x = border[0].x; x < border[1].x; x++) {
			current_hexagon = h2i({x, y});
			place_tool();
		}
	}
	current_hexagon = push_hex;
}

static void draw_choosed_rect() {
	if(hot.key == MouseLeft && !hot.pressed)
		execute(place_rect, 0, 0, current_tool);
	if(!hot.pressed)
		return;
	if(hot.key==MouseLeft && hot.pressed)
		border[0] = s2h(hot.mouse + camera);
	if(hot.pressed)
		border[1] = s2h(hot.mouse + camera);
	if(border[0] == border[1])
		return;
	auto push_caret = caret;
	caret = h2s(border[0]) - camera;
	auto p2 = h2s({border[1].x, border[0].y}) - camera;
	line(p2.x, p2.y);
	auto p3 = h2s({border[1].x, border[1].y}) - camera;
	line(p3.x, p3.y);
	auto p4 = h2s({border[0].x, border[1].y}) - camera;
	line(p4.x, p4.y);
	auto p5 = h2s({border[0].x, border[0].y}) - camera;
	line(p5.x, p5.y);
	caret = push_caret;
}

void editor() {
	update_animation();
	set_hexagon_position();
	control_map();
	redraw_floor();
	draw_choosed_rect();
	redraw_hexagon();
	paint_drawables();
	redraw_select_tool();
	status_window();
	status_text_main();
	editor_hotkey();
}