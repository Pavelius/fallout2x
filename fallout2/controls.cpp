#include "bsreq.h"
#include "dialog.h"
#include "draw.h"
#include "focus.h"
#include "io_stream.h"
#include "log.h"
#include "main.h"
#include "pathfind.h"
#include "screenshoot.h"

using namespace draw;

namespace {
typedef void(*fnlistrow)(const void* object);
struct number_widget {
	point		position;
	int			value;
	int			frame;
	unsigned	start;
};
struct actionparam {
	action_s	action;
	fnevent		proc;
	const void*	object;
};
}

extern "C" int system(const char* command);
extern int status_origin, status_maximum;

void apply_pallette_cicle(unsigned char* pal, unsigned dwCurrentTime);
void initialize_translation(const char* locale);
void focus_beforemodal();
void focus_finish();
void load_pallette(unsigned char* p1, int koeff = 4);
void post_setfocus();
void update_animation();

static screenshoot* screen;
static color default_palette[256];
static color temp_palette[256];
static adat<number_widget, 32> number_widgets;
static adat<actionparam, 16> actions;
static rect last_rect;
static guii cmd_gui;

static item drag_copy;

static void* drag_target;

static char edit_buffer[512];
static char info_buffer[1024];

static bool info_mode, mouse_clicked, allow_quick_info;

static int* list_current;

static int caret_position;
static int list_maximum, list_perpage;

spriteable cursor;
indext current_hexagon;

int last_list_current;

unsigned long current_tick;
static unsigned long last_tick, mouse_tick;

color getcolor(unsigned char i) {
	return default_palette[i];
}

static void gray_pallette() {
	memcpy(temp_palette, default_palette, sizeof(default_palette));
	for(auto& e : temp_palette)
		e = e.gray();
}

static void openerror() {
	char dir[512]; char temp[512];
	io::file::getdir(dir, sizeof(dir));
	stringbuilder sb(temp);
	sb.add("notepad \"%1\\errors.txt\"", dir);
	system(temp);
}

void setedit(const char* format, ...) {
	stringbuilder sb(edit_buffer);
	sb.add(format);
	caret_position = sb.getlenght();
}

const char* getedit() {
	return edit_buffer;
}

void theme_inititalize() {
	fore = getcolor(ColorText);
}

static actionparam* findaction(action_s v) {
	for(auto& e : actions) {
		if(e.action == v)
			return &e;
	}
	return 0;
}

static void addaction(action_s v, fnevent proc, const void* object = 0) {
	if(!info_mode)
		return;
	auto pa = findaction(v);
	if(pa)
		return;
	pa = actions.add();
	pa->action = v;
	pa->proc = proc;
	pa->object = object;
}

static void hilighting() {
	fore = fore.mix(color(255, 255, 255), 64);
}

static void focusing() {
	if(!gui.object)
		return;
	addfocus(gui.object);
	if(!getfocus())
		setfocus(gui.object);
	auto current = getfocus();
	if(getfocus() == gui.object)
		hilighting();
	else {
		if(hot.key == MouseLeft && hot.pressed) {
			if(ishilite())
				execute(post_setfocus, 0, 0, gui.object);
		}
	}
}

static bool istips(unsigned t) {
	if(!mouse_tick)
		return false;
	return (current_tick - mouse_tick) >= t;
}

static void button_command() {
	auto push_view = gui; gui = cmd_gui;
	if(hot.param == 1) {
		if(gui.object)
			setfocus(gui.object);
	}
	if(gui.execute)
		gui.execute();
	gui = push_view;
}

static void execute_button_command(int param = 0) {
	cmd_gui = gui;
	execute(button_command, param, 0, 0);
}

static void setcolor(int value) {
	fore = getcolor(value);
}

static void background() {
	auto p = gres(res::INTRFACE);
	if(!p)
		return;
	gui.shift = caret;
	image(gui.shift.x, gui.shift.y, p, p->ganim(gui.normal, current_tick), ImageNoOffset);
}

static void background_center() {
	auto p = gres(res::INTRFACE);
	if(!p)
		return;
	auto frame = p->ganim(gui.normal, current_tick);
	auto& f = p->get(frame);
	gui.shift.x = (getwidth() - f.sx) / 2 + caret.x;
	gui.shift.y = (getheight() - f.sy) / 2 + caret.y;
	image(gui.shift.x, gui.shift.y, p, frame, ImageNoOffset);
}

static void label() {
	text(gui.title);
}

static bool checkpressed(bool& run, unsigned key, bool execute_by_press = false, bool disabled = false) {
	static rect	button_rect;
	if(disabled)
		return false;
	auto pressed = false;
	if(ishilite(last_rect)) {
		if(hot.pressed)
			pressed = true;
		if(hot.pressed == execute_by_press && hot.key == MouseLeft)
			run = true;
	}
	if(key && hot.key == key)
		button_rect = last_rect;
	if(hot.key == InputKeyUp) {
		if(button_rect == last_rect) {
			run = true;
			button_rect.clear();
		}
	}
	if(button_rect == last_rect)
		pressed = true;
	return pressed;
}

static void center_text() {
	//char temp[260]; stringbuilder sb(temp);
	auto push_caret = caret;
	caret.x += (width - textw(gui.title)) / 2;
	draw::text(gui.title);
	caret = push_caret;
}

static void text_font3() {
	auto push_font = font;
	auto push_fore = fore;
	font = gres(res::FONT3);
	setcolor(ColorButton);
	text(gui.title);
	fore = push_fore;
	font = push_font;
}

static void center_text_font2() {
	auto push_font = font;
	auto push_fore = fore;
	font = gres(res::FONT3);
	setcolor(ColorButton);
	center_text();
	fore = push_fore;
	font = push_font;
}

static void label_checked() {
	auto push_fore = fore;
	auto push_height = height;
	height = texth();
	focusing();
	center_text();
	height = push_height;
	fore = push_fore;
}

static void label_right(const char* format) {
	if(!format)
		return;
	auto push_caret = caret;
	caret.x += width - textw(format);
	text(format);
	caret = push_caret;
}

static void label_left() {
	auto push_fore = fore;
	auto push_height = height;
	height = texth();
	if(gui.checked)
		fore = getcolor(ColorCheck);
	focusing();
	label();
	label_right(gui.value);
	//rectb();
	height = push_height;
	fore = push_fore;
}

static bool buttonf(int cicles_normal, int cicle_pressed, unsigned key, bool checked, bool execute_by_press, bool* result_pressed, bool disabled) {
	auto ps = gres(res::INTRFACE);
	auto normal = ps->ganim(cicles_normal, 0);
	auto pressed = ps->ganim(cicle_pressed, 0);
	auto result = false;
	last_rect = {caret.x, caret.y,
		caret.x + ps->get(normal).sx - 1,
		caret.y + ps->get(normal).sy - 1};
	auto a = checkpressed(result, key, execute_by_press, disabled);
	if(checked)
		a = true;
	if(result_pressed)
		*result_pressed = a;
	if(disabled) {
		gray_pallette();
		image(ps, normal, ImageNoOffset, temp_palette);
	} else
		image(ps, a ? pressed : normal, ImageNoOffset);
	return result;
}

static void text_block() {
	texta(gui.title, gui.flags);
}

static void text_info() {
	auto push_fore = fore;
	fore = getcolor(ColorInfo);
	texta(gui.title, 0);
	fore = push_fore;
}

static void text_button(bool rp) {
	char temp[260]; stringbuilder sb(temp);
	auto push_caret = caret;
	caret.x += (width - textw(gui.title)) / 2 + (rp ? 1 : 0);
	caret.y += 7;
	label();
	caret = push_caret;
}

static bool focused() {
	return gui.object == getfocus();
}

static void item_avatar() {
	if(!gui.number)
		return;
	auto ps = gres(res::INVEN);
	if(!ps)
		return;
	auto fr = ps->ganim(gui.number, 0);
	int isx = ps->get(fr).sx;
	int isy = ps->get(fr).sy;
	if(!gui.normal || (isx <= width && isy <= height))
		image(caret.x + (width - isx) / 2, caret.y + (height - isy) / 2, ps, fr, ImageNoOffset);
	else {
		int rsx = width;
		int rsy = height;
		if(isx > width)
			rsy = (isy * width) / isx;
		else
			rsx = (isx * height) / isy;
		if(rsy > height) {
			rsy = height;
			rsx = (isx * height) / isy;
		}
		surface real(rsx, rsy, 32);
		if(true) {
			rectpush push;
			surface zoom(isx, isy, 32);
			auto push_clip = clipping;
			auto push_canvas = canvas;
			canvas = &zoom; setclip();
			fore.r = fore.g = fore.b = fore.a = 0xFF;
			caret.x = caret.y = 0;
			width = isx; height = isy;
			rectf();
			image(ps, fr, ImageNoOffset);
			blit(real, 0, 0, real.width, real.height, 0, zoom, 0, 0, zoom.width, zoom.height);
			clipping = push_clip;
			canvas = push_canvas;
		}
		blit(*draw::canvas,
			caret.x + (width - real.width) / 2,
			caret.y + (height - real.height) / 2,
			real.width, real.height,
			ImageTransparent, real, 0, 0);
	}
}

static void text_hint(const char* format) {
	auto push_fore = fore;
	auto push_caret = caret;
	fore = getcolor(ColorCheck);
	caret.x += 4; caret.y += 4;
	text(format);
	fore = push_fore;
	caret = push_caret;
}

static void text_hint(const char* format, int v) {
	char temp[64]; stringbuilder sb(temp);
	sb.add(format, v);
	text_hint(temp);
}

static void get_object_information() {
	auto pi = (item*)hot.object;
	stringbuilder sb(info_buffer);
	pi->getinfo(sb);
}

static void look_object() {
	auto pi = (nameable*)hot.object;
	status("%ThisIs %1.", pi->getname());
}

static void talk_object() {
	auto p = (character*)hot.object;
	p->talk();
}

static void turn_object() {
}

static void drag_item() {
	drag_target = 0;
	if(!hot.pressed)
		execute(buttonok);
	dialog::paint();
	cursor.set(res::INVEN, drag_copy.geti().avatar.inventory);
}

static void begin_drag_item() {
	auto p1 = (item*)getdragged();
	drag_copy = *p1;
	p1->clear();
	draw::scene(drag_item);
	dragend();
	if(!drag_target)
		*p1 = drag_copy;
	else if(drag_target == character::last)
		character::last->additem(drag_copy);
	else
		drag_copy.transfer(*p1, *((item*)drag_target));
}

static void drop_item() {
	auto pi = (item*)hot.object;
	pi->dropdown();
}

static void item_button() {
	auto pi = (item*)gui.data;
	if(!pi)
		return;
	if(isdragtarget())
		drag_target = pi;
	auto a = ishilite();
	if(a) {
		hilite_object = pi;
		if(info_mode) {
			addaction(Look, get_object_information, pi);
			if(pi->getclip())
				addaction(Reload, 0, pi);
			addaction(Drop, drop_item, pi);
		} else {
			if(hot.key == MouseLeft && hot.pressed) {
				dragbegin(pi);
				execute(begin_drag_item);
			}
		}
	}
	if(!(*pi))
		return;
	if(true) {
		rectpush push;
		setoffset(4, 4);
		item_avatar();
	}
	auto count = pi->getcount();
	if(count > 1)
		text_hint("x%1i", count);
}

static void button_no_text() {
	unsigned key = 0;
	if(focused())
		key = gui.key;
	if(buttonf(gui.normal, gui.pressed, key, gui.checked, false, 0, gui.disabled))
		execute_button_command(1);
}

static void button_no_focus() {
	if(buttonf(gui.normal, gui.pressed, gui.key, gui.checked, false, 0, gui.disabled))
		execute_button_command(1);
}

static void button_def() {
	auto old_font = font;
	auto old_fore = fore;
	auto rp = false;
	auto r = buttonf(gui.normal, gui.pressed, gui.key, false, false, &rp, false);
	font = gres(res::FONT3);
	setcolor(ColorButton);
	text_button(rp);
	if(r)
		execute_button_command();
	font = old_font;
	fore = old_fore;
}

static void button_radio() {
	char temp[260]; stringbuilder sb(temp);
	auto push_fore = fore;
	auto push_font = font;
	font = gres(res::FONT3);
	auto ps = gres(res::INTRFACE);
	auto frame = ps->ganim(8, 0);
	int sx = ps->get(frame).sx;
	int sy = draw::texth();
	int w1 = width - sx;
	setcolor(ColorButton);
	auto push_caret = draw::caret;
	caret.x += sx + (w1 - draw::textw(gui.title)) / 2;
	text(gui.title);
	caret = push_caret;
	last_rect = {caret.x, caret.y, caret.x + width, caret.y + sy};
	auto a = ishilite(last_rect);
	auto result = false;
	if(checkpressed(result, gui.key))
		frame++;
	image(caret.x, caret.y + 1, ps, frame, ImageNoOffset);
	font = push_font;
	fore = push_fore;
	if(result)
		execute_button_command();
}

static number_widget* find_number_widget(point pt) {
	for(auto& e : number_widgets) {
		if(e.position == pt)
			return &e;
	}
	return 0;
}

static number_widget* get_number_widget(point pt, int value) {
	auto p = find_number_widget(pt);
	if(!p) {
		p = number_widgets.add();
		p->position = pt;
		p->value = value;
		p->frame = 0;
		p->start = 0;
	}
	return p;
}

static void render_number(int digits, int value, sprite* ps, int cicle, int symbol_offset, int symbol_width, bool animate) {
	if(!ps)
		return;
	auto pw = get_number_widget(caret, value);
	auto fr = ps->ganim(cicle, 0);
	auto& pf = ps->get(fr);
	auto symbol_height = pf.sy;
	char temp[16]; stringbuilder sb(temp);
	sb.adduint(value, digits, 10);
	auto push_clip = clipping;
	for(int i = 0; i < digits; i++) {
		clipping.y1 = caret.y;
		clipping.y2 = caret.y + symbol_height;
		clipping.x1 = caret.x + i * symbol_width;
		clipping.x2 = caret.x + (i + 1) * symbol_width;
		int sym = temp[i] - '0';
		if(animate && pw->value != value) {
			const int time_interval = 60;
			char t1[16]; stringbuilder sb(t1);
			sb.adduint(pw->value, digits, 10);
			if(t1[i] != temp[i]) {
				if(!pw->start) {
					pw->start = current_tick;
					if(pw->value > value)
						pw->frame = 1;
					else
						pw->frame = -1;
				} else if(pw->frame == 0) {
					pw->value = value;
					pw->start = 0;
				} else if(pw->frame > 0) {
					sym = 11;
					if((current_tick - pw->start) / time_interval) {
						pw->start = current_tick;
						pw->frame = 0;
					}
				} else {
					sym = 10;
					if((current_tick - pw->start) / time_interval) {
						pw->start = current_tick;
						pw->frame = 0;
					}
				}
			}
		}
		image(caret.x + (i - sym) * symbol_width - symbol_offset, caret.y, ps, fr, ImageNoOffset);
	}
	clipping = push_clip;
}

static void rollup() {
	static stat_s current = SmallGuns;
	auto p = getfocus();
	auto isfocused = false;
	if(bsdata<stati>::have(p)) {
		auto s = (stat_s)((stati*)p - bsdata<stati>::elements);
		if(s >= SmallGuns && s <= Outdoorsman) {
			current = s;
			isfocused = true;
		}
	}
	caret.y += 11 * (current - SmallGuns);
	auto ps = gres(res::INTRFACE);
	if(!ps)
		return;
	image(ps, ps->ganim(190, current_tick / 200), 0);
}

static bool radio(int cicle, unsigned key) {
	static rect	button_rect;
	auto ps = gres(res::INTRFACE);
	int frame = ps->ganim(cicle, 0);
	last_rect = {caret.x, caret.y,
		caret.x + ps->get(frame).sx - 1,
		caret.y + ps->get(frame).sy - 1};
	auto r = false;
	if(checkpressed(r, key))
		frame++;
	image(caret.x, caret.y, ps, frame, ImageNoOffset);
	return r;
}

static void number(int digits, int value) {
	if(value >= 0)
		render_number(digits, value, gres(res::INTRFACE), 170, 0, 14, true);
	else
		render_number(digits, -value, gres(res::INTRFACE), 170, 12 * 14, 14, true);
}

static void numberap(int digits, int value) {
	render_number(digits, value, gres(res::INTRFACE), 290, 0, 10, false);
}

static void horizontal(int d, sprite* ps, int frame, int count) {
	for(auto i = 0; i < count; i++) {
		image(ps, frame, 0);
		caret.x += d;
	}
}

static void redraw_ap(int green, int yellow, int red) {
	auto ps = gres(res::INTRFACE);
	if(!ps)
		return;
	auto push_caret = caret;
	const int w = 9;
	horizontal(w, ps, ps->ganim(83, 0), green);
	horizontal(w, ps, ps->ganim(85, 0), yellow);
	horizontal(w, ps, ps->ganim(84, 0), red);
	caret = push_caret;
}

static void number_standart() {
	auto n = gui.normal;
	if(!n)
		n = 2;
	number(2, gui.number);
}

static void number_small() {
	render_number(gui.normal, gui.number, gres(res::INTRFACE), 82, 0, 9, true);
}

static void number_actions() {
	redraw_ap(gui.number, 10 - gui.number, 0);
}

static void line_info() {
	auto push_fore = fore;
	fore = getcolor(ColorInfo);
	rectf();
	fore = push_fore;
}

static int getavatarindex(const void* object) {
	if(bsdata<stati>::have(object))
		return ((stati*)object)->avatar;
	else if(bsdata<perki>::have(object))
		return ((perki*)object)->avatar;
	return -1;
}

static void list_input(int& current, int& origin, int perpage, int perline) {
	list_current = &current;
	if(current >= list_maximum)
		current = list_maximum - 1;
	if(current < 0)
		current = 0;
	if(current < origin)
		origin = current;
	if(origin + perpage < current)
		origin = current - perpage;
	switch(hot.key) {
	case KeyUp:
	case MouseWheelUp:
		if(current)
			execute(cbsetint, current - 1, 0, &current);
		break;
	case MouseWheelDown:
	case KeyDown:
		if(current < list_maximum - 1)
			execute(cbsetint, current + 1, 0, &current);
		break;
	case KeyHome:
		if(current)
			execute(cbsetint, 0, 0, &current);
		break;
	case KeyEnd:
		if(current != list_maximum - 1)
			execute(cbsetint, list_maximum - 1, 0, &current);
		break;
	case KeyPageUp:
		if(current)
			execute(cbsetint, current - perpage, 0, &current);
		break;
	case KeyPageDown:
		execute(cbsetint, current + perpage, 0, &current);
		break;
	}
	if(list_maximum > origin + perpage + 1)
		list_maximum = origin + perpage + 1;
}

static void list_input_nocurrent(int& origin, int perpage, int perline) {
	list_current = &origin;
	if(origin + perpage >= list_maximum)
		origin = list_maximum - perpage;
	if(origin < 0)
		origin = 0;
	switch(hot.key) {
	case KeyUp:
	case MouseWheelUp:
		execute(cbsetint, origin - 1, 0, &origin);
		break;
	case MouseWheelDown:
	case KeyDown:
		execute(cbsetint, origin + 1, 0, &origin);
		break;
	case KeyHome:
		execute(cbsetint, 0, 0, &origin);
		break;
	case KeyEnd:
		execute(cbsetint, list_maximum, 0, &origin);
		break;
	case KeyPageUp:
		execute(cbsetint, origin - perpage, 0, &origin);
		break;
	case KeyPageDown:
		execute(cbsetint, origin + perpage, 0, &origin);
		break;
	}
	if(list_maximum > origin + perpage + 1)
		list_maximum = origin + perpage + 1;
}

static void list_paint(int& origin, int& current, int perline, fnlistrow prow) {
	if(!perline)
		return;
	list_perpage = height / perline;
	if(!list_perpage)
		return;
	list_maximum = gui.count;
	list_input(current, origin, list_perpage, perline);
	auto push_height = height;
	height = perline;
	for(auto i = origin; i < list_maximum; i++) {
		auto push_fore = fore;
		if(i == current)
			hilighting();
		if(ishilite() && hot.key == MouseLeft && hot.pressed)
			execute(cbsetint, i, 0, &current);
		if(prow)
			prow(gui.ptr(i));
		fore = push_fore;
		caret.y += height;
	}
	height = push_height;
}

static void list_paint_nocurrent(int& origin, int perline, fnlistrow prow) {
	if(!perline || !prow)
		return;
	list_perpage = height / perline;
	if(!list_perpage)
		return;
	list_maximum = gui.count;
	list_input_nocurrent(origin, list_perpage, perline);
	auto push_height = height;
	auto push_clip = clipping;
	rect rc = {caret.x, caret.y, caret.x + width, caret.y + height};
	auto a = getdragged() && hot.mouse.in(rc);
	clipping.set(caret.x, caret.y, caret.x + width, caret.y + height);
	height = perline;
	for(auto i = origin; i < list_maximum; i++) {
		prow(gui.ptr(i));
		caret.y += height;
	}
	if(a && !drag_target)
		drag_target = character::last;
	clipping = push_clip;
	height = push_height;
}

static void string_row(const void* object) {
	if(!gui.pgetname)
		return;
	char temp[260]; stringbuilder sb(temp);
	sb.clear();
	gui.pgetname(object, sb);
	text(temp);
}

static void text_list() {
	static int origin;
	list_paint(origin, last_list_current, gui.lines, string_row);
}

static void item_row(const void* object) {
	auto push_gui = gui;
	auto pi = *((item**)object);
	gui.title = pi->getname();
	gui.data = pi;
	gui.normal = 1;
	gui.number = pi->geti().avatar.inventory;
	item_button();
	gui = push_gui;
}

static void items_list() {
	static int origin;
	list_paint_nocurrent(origin, gui.lines, item_row);
}

static void remove_symbol() {
	auto p = (char*)hot.object;
	auto i = hot.param;
	auto m = zlen(p + i);
	if(!m)
		return;
	memmove(p + i, p + i + 1, m);
	caret_position = i;
}

static void add_symbol() {
	auto p = (char*)hot.object;
	auto i = hot.param;
	auto s = (char)hot.param2;
	auto m = zlen(p);
	auto c = m - i;
	memmove(p + i + 1, p + i, c);
	p[i + c + 1] = 0;
	p[i] = s;
	caret_position = i + 1;
}

static void seteditpos(unsigned index) {
	caret_position = index;
}

static void edit() {
	auto title = edit_buffer;
	int maximum = sizeof(edit_buffer) / sizeof(edit_buffer[0]);
	int lenght = zlen(title);
	if(caret_position > lenght)
		caret_position = lenght;
	text(title);
	auto frame = (current_tick / 200) % 3;
	if(frame != 0) {
		rectpush push;
		auto push_alpha = alpha;
		caret.x += textw(title, caret_position) - 1;
		width = 3; height = texth() - 1;
		alpha = 192;
		rectf();
		alpha = push_alpha;
	}
	switch(hot.key) {
	case KeyLeft:
		if(caret_position > 0)
			execute(cbsetint, caret_position - 1, 0, &caret_position);
		break;
	case KeyRight:
		if(caret_position < lenght)
			execute(cbsetint, caret_position + 1, 0, &caret_position);
		break;
	case KeyHome:
		execute(cbsetint, 0, 0, &caret_position);
		break;
	case KeyEnd:
		execute(cbsetint, lenght, 0, &caret_position);
		break;
	case KeyBackspace:
		if(caret_position > 0)
			execute(remove_symbol, caret_position - 1, 0, title);
		break;
	case KeyDelete:
		execute(remove_symbol, caret_position, 0, title);
		break;
	case InputSymbol:
		if(lenght < maximum - 1 && hot.param >= ' ')
			execute(add_symbol, caret_position, hot.param, title);
		break;
	}
}

static void custom_image() {
	auto p = gres(res::INTRFACE);
	if(gui.pressed)
		image(p, p->ganim(gui.normal, gui.pressed), ImageNoOffset);
	else
		image(p, p->ganim(gui.normal, current_tick / 200), ImageNoOffset);
}

static void block_information() {
	auto object = getfocus();
	if(!object)
		return;
	auto id = *((const char**)object);
	auto name = getnm(id);
	auto description = getdescription(id);
	auto push_font = font;
	auto push_fore = fore;
	auto push_width = width;
	auto push_height = height;
	font = gres(res::FONT2);
	setcolor(ColorInfo);
	text(name);
	caret.y += texth() + 5;
	font = gres(res::FONT1);
	auto avatar = getavatarindex(object);
	if(avatar != -1) {
		width = 160;
		height = 125;
		image(caret.x + 135, caret.y, gres(res::SKILLDEX), avatar, ImageNoOffset);
		//rectb();
		texta(description);
	}
	height = push_height;
	width = push_width;
	fore = push_fore;
	font = push_font;
}

static void object_information() {
	textf(info_buffer);
}

static void paper_doll() {
	auto push_caret = caret;
	auto push_clip = clipping;
	//rectb();
	auto pc = character::last;
	clipping.set(caret.x, caret.y, caret.x + width, caret.y + height);
	auto p = gres(pc->getlook());
	auto f = pc->getframe(AnimateStand, pc->getweaponindex()) + (current_tick / 500) % 6;
	caret.x += width / 2;
	caret.y += height - 12;
	image(p, p->ganim(f, 0), 0);
	clipping = push_clip;
	caret = push_caret;
}

static void scroll_up() {
	if(list_current) {
		if(buttonf(gui.normal, gui.pressed, KeyUp, false, false, 0, (*list_current) == 0))
			execute(cbsetint, *list_current - 1, 0, list_current);
	}
}

static void scroll_down() {
	if(list_current) {
		if(buttonf(gui.normal, gui.pressed, KeyDown, false, false, 0, (*list_current + list_perpage) >= list_maximum))
			execute(cbsetint, *list_current + 1, 0, list_current);
	}
}

void set_hexagon_position() {
	current_hexagon = Blocked;
	if(ishilite())
		current_hexagon = h2i(s2h(hot.mouse + camera));
}

static void point_of_view() {
	auto p2 = h2s(i2h((indext)hot.param));
	auto p1 = character::last->position;
	character::last->moveto((indext)hot.param, hot.param2);
}

static void click_command() {
	if(!info_mode && current_hexagon != Blocked) {
		if((hot.key & 0xFF) == MouseLeft && hot.pressed)
			execute(point_of_view, current_hexagon, (hot.key & Shift) ? 0 : 1);
	}
}

static void move_to_combat() {
	auto index = (indext)hot.param;
	auto cost = pathfind::getmove(index);
	auto p2 = h2s(i2h(index));
	auto p1 = character::last->position;
	character::last->moveto(index, hot.param2);
	character::last->set(APCur, character::last->get(APCur) - cost);
	character::last->wait();
	buttonok();
}

static void click_command_combat() {
	if(current_hexagon == Blocked)
		return;
	if(pathfind::getmove(current_hexagon) == Blocked)
		return;
	if(!info_mode) {
		if((hot.key & 0xFF) == MouseLeft && hot.pressed)
			execute(move_to_combat, current_hexagon, 0);
	}
}

static void marker() {
	auto push_caret = caret;
	caret.x -= 4; line(caret.x + 4, caret.y);
	caret = push_caret;
	caret.y -= 4; line(caret.x, caret.y + 4);
	caret = push_caret;
}

static void textac(const char* format) {
	auto push_caret = caret;
	caret.x -= textw(format) / 2;
	caret.y -= texth() / 2;
	text(format);
	caret = push_caret;
}

static void textwc(const char* format, void(*proc)(const char*)) {
	auto push_fore = fore;
	fore = getcolor(ColorCheck);
	proc(format);
	fore = push_fore;
}

static void textacw(int number) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", number);
	textwc(temp, textac);
}

void redraw_hexagon(bool show_movement) {
	if(hot.key == InputUpdate || info_mode || current_hexagon == Blocked)
		return;
	cursor.clear();
	auto push_caret = caret;
	caret = h2s(i2h(current_hexagon)) - camera;
	image(gres(res::INTRFACE), 1, 0);
	if(show_movement) {
		auto cost = pathfind::getmove(current_hexagon);
		if(cost != Blocked)
			textacw(cost);
	}
	//marker();
	caret = push_caret;
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
			if(!caret.in(rc))
				continue;
			auto tv = bsdata<tilei>::elements[loc.getfloor(t2i({x, y}))].frame;
			if(tv > 1)
				draw::image(ps, ps->ganim(tv, tm), 0);
		}
	}
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

void paint_drawables();

static void action_number(int x, int y, int v) {
	auto push_caret = caret;
	caret.x += x; caret.y += y;
	numberap(1, v);
	caret = push_caret;
}

static void next_action() {
	character::last->nextaction();
}

static void action_item_button() {
	auto ps = gres(res::INTRFACE);
	auto pi = (item*)gui.data;
	if(!pi || !ps)
		return;
	auto a = ishilite();
	if(a && hot.pressed) {
		caret.y -= 2;
		caret.x += 1;
	}
	if(!(*pi))
		return;
	gui.number = pi->geti().avatar.inventory;
	auto actions = pi->getactions();
	if(actions) {
		auto index = character::last->action_index[0];
		if(index > actions.getcount())
			index = actions.getcount() - 1;
		auto ac = actions.data[index];
		auto& ai = bsdata<actioni>::elements[ac];
		image(caret.x + width - 30, caret.y + 16, ps, ps->ganim(ai.frame, current_tick), 0);
		if(ai.is(Aimed))
			image(caret.x + width - 10, caret.y + 36, ps, ps->ganim(288, current_tick), 0);
		image(caret.x + 16, caret.y + 56, ps, ps->ganim(289, 0), 0);
		auto ap = character::last->getactionpoints(ac, *pi);
		action_number(30, 45, ap);
		rectpush push;
		setoffset(4, 4);
		item_avatar();
		if(a && hot.key == MouseRight && !hot.pressed)
			execute(next_action);
	}
}

static void action_button() {
	button_no_text();
	gui.data = character::last->wear + RightHandItem;
	action_item_button();
}

void paint_animation() {
	auto push_clip = clipping;
	clipping.set(caret.x, caret.y, caret.x + width, caret.y + height);
	redraw_floor();
	paint_drawables();
	clipping = push_clip;
}

static void set_allow_quick_info() {
	allow_quick_info = gui.normal;
}

static void apply_info_mode() {
	set_allow_quick_info();
	if(hot.key == MouseRight && hot.pressed)
		execute(cbsetbool, !info_mode, 0, &info_mode);
	switch(gui.normal) {
	case 2:
		if(info_mode)
			cursor.set(res::INTRFACE, 250);
		break;
	default:
		cursor.set(res::INTRFACE, info_mode ? 250 : 286);
		break;
	}
}

static void stroke_character(character* player) {
	auto push_stroke = fore_stroke;
	fore_stroke = getcolor(ColorButton);
	auto pr = gres(player->getlook());
	auto ps = player->position - camera;
	stroke(ps.x, ps.y, pr, player->frame, 0, 1);
	fore_stroke = push_stroke;
}

static void hiliting_object() {
	if(!hilite_object)
		return;
	if(bsdata<character>::have(hilite_object)) {
		auto p = static_cast<character*>(((drawable*)hilite_object));
		stroke_character(p);
		if(character::last != p && p->chat)
			addaction(Talk, talk_object, p);
		addaction(Look, look_object, static_cast<nameable*>(p));
		addaction(Turn, turn_object, p);
	}
}

static void paint_combat() {
	auto push_clip = clipping;
	clipping.set(caret.x, caret.y, caret.x + width, caret.y + height);
	if(hot.mouse.in(clipping))
		apply_info_mode();
	set_hexagon_position();
	control_map();
	redraw_floor();
	redraw_hexagon(true);
	paint_drawables();
	hiliting_object();
	click_command_combat();
	clipping = push_clip;
}

static void paint_game() {
	auto push_clip = clipping;
	clipping.set(caret.x, caret.y, caret.x + width, caret.y + height);
	if(hot.mouse.in(clipping))
		apply_info_mode();
	set_hexagon_position();
	control_map();
	redraw_floor();
	redraw_hexagon(false);
	paint_drawables();
	hiliting_object();
	click_command();
	clipping = push_clip;
}

static void redraw_game() {
	rectpush push;
	auto push_clip = clipping;
	caret.x = caret.y = 0;
	width = 640; height = 381;
	clipping.set(0, 0, width, height);
	redraw_floor();
	paint_drawables();
	clipping = push_clip;
}

static void hotkey() {
	if(gui.key == hot.key)
		execute_button_command();
}

static void list_input_nocurrent_nokeys(int& origin, int maximum) {
	if(!maximum)
		return;
	if(origin + height >= maximum)
		origin = maximum - height;
	if(origin < 0)
		origin = 0;
	const int dy = 8;
	rect rb = {caret.x, caret.y, caret.x + width, caret.y + height};
	if(hot.mouse.in(rb)) {
		rect ru = rb; ru.y2 = ru.y1 + dy;
		rect rd = rb; rd.y1 = rd.y2 - dy;
		if(hot.mouse.in(ru) && origin > 0) {
			cursor.set(res::INTRFACE, 268);
			if(hot.key == MouseLeft && hot.pressed)
				execute(cbsetint, origin - texth(), 0, &origin);
		} else if(hot.mouse.in(rd) && (origin + height < maximum)) {
			cursor.set(res::INTRFACE, 269);
			if(hot.key == MouseLeft && hot.pressed)
				execute(cbsetint, origin + texth(), 0, &origin);
		}
		switch(hot.key) {
		case MouseWheelUp:
			execute(cbsetint, origin - texth(), 0, &origin);
			break;
		case MouseWheelDown:
			execute(cbsetint, origin + texth(), 0, &origin);
			break;
		}
	}
}

static void scrolltext(const char* format, int& origin, int& origin_cashe, int& format_maximum) {
	static const char* format_cashe;
	static int format_origin;
	list_input_nocurrent_nokeys(origin, format_maximum);
	if(origin_cashe != origin || !format_maximum) {
		if(origin_cashe < origin)
			origin_cashe++;
		else if(origin_cashe > origin)
			origin_cashe--;
		auto push_height = height;
		auto push_width = width;
		textfs(format, origin_cashe, format_cashe, format_origin);
		format_maximum = height;
		width = push_width;
		height = push_height;
	}
	auto push_clipping = clipping;
	clipping = {caret.x, caret.y, caret.x + width, caret.y + height};
	auto push_caret = caret;
	caret.y += format_origin;
	textf(format_cashe);
	caret = caret;
	clipping = push_clipping;
}

static void tab_paint(int& current, int* frames, const char** names) {
	auto ps = gres(res::INTRFACE);
	if(!ps)
		return;
	auto frame = ps->ganim(frames[current], current_tick / 200);
	image(gres(res::INTRFACE), frame, ImageNoOffset);
	auto push_caret = caret;
	auto push_font = font;
	auto push_fore = fore;
	font = metrics::h2;
	fore = getcolor(ColorButton);
	for(auto i = 0; i < 3; i++) {
		auto x1 = push_caret.x + width * i + 10;
		rect rc = {x1 + 4, push_caret.y, x1 + width - 4, push_caret.y + 28};
		if((ishilite(rc) && hot.key == MouseLeft && hot.pressed)
			|| (hot.key == ('1' + i)))
			execute(cbsetint, i, 0, &current);
		auto pn = getnm(names[i]);
		caret.x = x1 + (width - textw(pn)) / 2;
		caret.y = push_caret.y + ((i == current) ? 4 : 6);
		text(pn);
	}
	fore = push_fore;
	font = push_font;
	caret = push_caret;
}

static void tab_paint_page() {
	static int frames[] = {180, 178, 179};
	static const char* names[] = {"Bonuses", "Karma", "Kills"};
	static int current;
	tab_paint(current, frames, names);
}

static void status_bar() {
	static int origin_cashe;
	scrolltext(getstatusmessage(), status_origin, origin_cashe, status_maximum);
}

static void image(int x, int y, res id, int cicle, unsigned flags) {
	auto ps = gres(id);
	if(!ps)
		return;
	image(x, y, ps, ps->ganim(cicle, current_tick / 200), flags);
}

static void open_dialog() {
	draw::opendialog((const char*)hot.object);
}

static unsigned gethotkey(int index) {
	if(index == -1)
		return KeyEscape;
	else if(index <= 8)
		return '1' + index;
	else if(index == 9)
		return '0';
	else
		return 'A' + (index - 10);
}

static void paint_answer(int index, const void* data, const char* format, unsigned key) {
	auto push_caret = caret;
	auto push_width = width;
	auto push_height = height;
	auto push_fore = fore;
	auto dx = textw("~");
	text("~"); caret.x += dx + 6; width -= dx + 6;
	textfs(format);
	if(ishilite()) {
		if(hot.pressed)
			fore = fore.mix(color(0, 0, 0), 128);
		else
			fore = fore.mix(color(255, 255, 255), 128);
		if(hot.key == MouseLeft && !hot.pressed)
			execute(buttonparam, (long)data);
	}
	textf(format);
	if(hot.key == gethotkey(index) || (key && key == hot.key))
		execute(buttonparam, (long)data);
	fore = push_fore;
	height = push_height;
	width = push_width;
	caret.x = push_caret.x;
}

static void animate_image(int x, int y, res id, int frame, int step) {
	auto push_cursor = cursor;
	screenshoot push;
	auto pi = gres(id);
	auto pc = pi->gcicle(frame);
	auto bs = getcputime();
	auto index = 0;
	while(true) {
		ismodal();
		push.restore();
		if(step == -1)
			image(x, y, pi, pc->start + pc->count - (index + 1), 0);
		else
			image(x, y, pi, pc->start + index, 0);
		cursor.position = hot.mouse;
		cursor.set(res::INTRFACE, 295);
		cursor.paint();
		doredraw();
		waitcputime(1);
		index = (getcputime() - bs) / 200;
		if(index >= pc->count)
			break;
	}
	cursor = push_cursor;
}

void animate_combat_mode(int open) {
	if(dialog::last)
		dialog::paint();
	animate_image(608, 477, res::INTRFACE, 104, open);
}

void* answers::choose(const char* promt, const char* cancel) const {
	int origin = 0, origin_cashe = 0, format_maximum = 0;
	auto push_strings = disable_floatstring;
	disable_floatstring = true;
	redraw_game();
	disable_floatstring = push_strings;
	image(0, 0, res::INTRFACE, 103, ImageNoOffset);
	screenshoot push;
	while(ismodal()) {
		push.restore();
		paintstart();
		image(0, 290, res::INTRFACE, 99, ImageNoOffset);
		caret = {140, 232};
		width = 370; height = 45;
		scrolltext(promt, origin, origin_cashe, format_maximum);
		caret = {13, 444};
		if(buttonf(97, 98, 'V', false, false, 0, false))
			execute(open_dialog, 0, 0, "ChatHistory");
		caret = {593, 331};
		if(buttonf(96, 95, 'T', false, false, 0, false))
			execute(open_dialog, 0, 0, "ChatTrade");
		caret = {130, 340};
		width = 370; height = 108;
		auto index = 0;
		for(auto& e : elements)
			paint_answer(index++, e.value, e.text, 0);
		if(cancel)
			paint_answer(index++, 0, cancel, KeyEscape);
		paintfinish();
		domodal();
	}
	return (void*)getresult();
}

int draw::opendialog(const char* id) {
	auto p = bsdata<dialog>::find(id);
	if(!p)
		return 0;
	info_mode = false;
	return p->open();
}

static void update_tick() {
	auto tick = getcputime();
	if(last_tick && tick > last_tick)
		current_tick += tick - last_tick;
	last_tick = tick;
}

static void update_mouse_tick() {
	static point last;
	if(last != hot.mouse || hot.key == MouseLeft || hot.key == MouseRight) {
		last = hot.mouse;
		mouse_tick = current_tick;
		mouse_clicked = false;
	}
	if(hot.key == MouseLeft)
		mouse_clicked = true;
}

static void update_animation() {
	static unsigned long last_tick;
	if(!last_tick)
		last_tick = current_tick;
	auto d = current_tick - last_tick;
	last_tick = current_tick;
	if(d > 1000) // Game resume from pause
		return;
	for(auto& e : bsdata<character>()) {
		if(!e)
			continue;
		e.timer -= d;
		if(e.timer > 0)
			continue;
		e.timer += e.getdelay();
		e.updateframe();
	}
	update_floatstring();
}

void animable::wait() {
	auto need_stop = false;
	while(!need_stop && ismodal()) {
		hot.key = InputUpdate;
		dialog::paint();
		cursor.position = hot.mouse;
		cursor.set(res::INTRFACE, 295);
		cursor.paint();
		doredraw();
		update_animation();
		need_stop = isanimate(AnimateStand);
		waitcputime(1);
	}
}

bool is_special_animate();

static void beforemodal() {
	update_tick();
	update_mouse_tick();
	focus_beforemodal();
	apply_pallette_cicle((unsigned char*)palt, current_tick);
	allow_quick_info = false;
	if(pfinish) {
		actions.clear();
		cursor.set(res::INTRFACE, 267);
	}
}

static int current_action;

static void action_row(const void* object) {
	auto ps = gres(res::INTRFACE);
	if(!ps)
		return;
	auto pa = (actionparam*)object;
	auto frame = bsdata<actioni>::elements[pa->action].frame;
	auto hilite = (gui.ptr(current_action) == object);
	if(!hilite)
		frame++;
	image(ps, ps->ganim(frame, 0), 0);
}

static void paint_action_list() {
	static int origin;
	const auto dy = 40;
	width = dy;
	height = actions.count * dy;
	gui.clear();
	gui.linklist(actions.data, actions.count);
	caret = cursor.position;
	caret.x += 48;
	caret.y += dy;
	current_action = (hot.mouse.y - cursor.position.y) / dy;
	list_paint(origin, current_action, dy, action_row);
}

static void context_menu_loop() {
	if(screen)
		screen->restore();
	if(!hot.pressed)
		execute(buttonparam, current_action);
	paint_action_list();
}

static void do_nothing() {
}

static int choose_action() {
	auto push_finish = pfinish; pfinish = 0;
	auto push_screen = screen;
	screenshoot save; screen = &save;
	addaction(NoAction, do_nothing, 0);
	draw::scene(context_menu_loop);
	pfinish = push_finish;
	screen = push_screen;
	return getresult();
}

static void context_menu_modal() {
	auto i = choose_action();
	auto& e = actions.data[i];
	if(e.proc) {
		hot.param = e.action;
		hot.object = e.object;
		e.proc();
	}
}

static void paint_quickaction() {
	auto ps = gres(res::INTRFACE);
	if(!ps || !actions)
		return;
	auto& e = actions[0];
	auto frame = bsdata<actioni>::elements[e.action].frame;
	image(cursor.position.x + 48, cursor.position.y + 40, ps, ps->ganim(frame + 1, 0), 0);
}

static void context_menu() {
	static const void* tips_object;
	if(cursor.resource == res::INTRFACE && cursor.frame == 250) {
		if(actions) {
			if(istips(500)) {
				paint_quickaction();
				if(mouse_clicked && hot.pressed)
					execute(context_menu_modal);
				else if(allow_quick_info) {
					if(tips_object != hilite_object) {
						tips_object = hilite_object;
						cursor.paint();
						auto pa = findaction(Look);
						if(pa)
							execute(look_object, Look, 0, pa->object);
					}
				}
			} else {
				if(hot.key == MouseLeft && hot.pressed) {
					auto& e = actions[0];
					execute(e.proc, e.action, 0, e.object);
				}
			}
		}
	}
}

static void finish() {
	focus_finish();
	caret = cursor.position = hot.mouse;
	context_menu();
}

void paint_pathfind() {
	auto p = character::last->path;
	if(!p)
		return;
	auto push_caret = caret;
	rect rc = {caret.x, caret.y, caret.x + width, caret.y + height};
	caret = character::last->position - camera;
	while(*p != Blocked) {
		auto pt = h2s(i2h(*p)) - camera;
		line(pt.x, pt.y);
		p++;
	}
	caret = push_caret;
}

static void tips() {
	cursor.paint();
}

int start_application(fnevent proc, fnevent afterread) {
	if(!proc)
		return -1;
	metrics::border = 2;
	metrics::padding = 2;
	palt = default_palette;
	load_pallette((unsigned char*)palt);
	bsreq::read("rules/Basic.txt");
	initialize_translation("ru");
	if(afterread)
		afterread();
	if(log::geterrors()) {
		log::stoplogging();
		openerror();
		return -1;
	}
	pbeforemodal = beforemodal;
	pfinish = finish;
	ptips = tips;
	initialize(getnm("AppTitle"));
	syscursor(false);
	settimer(40);
	theme_inititalize();
	setnext(proc);
	start();
	return 0;
}

BSDATA(widget) = {
	{"ApplyInfoMode", apply_info_mode},
	{"ActionButton", action_button},
	{"Background", background},
	{"BackgroundC", background_center},
	{"Button", button_radio},
	{"ButtonDF", button_def},
	{"ButtonNT", button_no_text},
	{"ButtonNF", button_no_focus},
	{"Edit", edit},
	{"Hotkey", hotkey},
	{"Information", block_information},
	{"Image", custom_image},
	{"ItemAvatar", item_avatar},
	{"ItemButton", item_button},
	{"ItemList", items_list},
	{"Label", label_left},
	{"LabelSM", label_checked},
	{"LineInfo", line_info},
	{"List", text_list},
	{"Number", number_standart},
	{"NumberAction", number_actions},
	{"NumberSM", number_small},
	{"ObjectInformation", object_information},
	{"PaintBlock", paint_pathfind},
	{"PaintCombat", paint_combat},
	{"PaintGame", paint_game},
	{"PaperDoll", paper_doll},
	{"RollUp", rollup},
	{"ScrollDown", scroll_down},
	{"ScrollUp", scroll_up},
	{"StatusBar", status_bar},
	{"Tab", tab_paint_page},
	{"Text", center_text_font2},
	{"TextBlock", text_block},
	{"TextInfo", text_info},
	{"TextL", text_font3},
	{"UpdateAnimation", update_animation},
};
BSDATAF(widget)