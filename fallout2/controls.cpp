#include "bsreq.h"
#include "dialog.h"
#include "draw.h"
#include "focus.h"
#include "main.h"
#include "screenshoot.h"

namespace {
struct number_widget {
	point		position;
	int			value;
	int			frame;
	unsigned	start;
};
}

using namespace draw;

static adat<number_widget, 32> number_widgets;
static rect last_rect;
static char edit_buffer[512];
static int caret_position;
static int last_list_origin;
int last_list_current;

void post_setfocus();

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

static void hilighting() {
	fore = fore.mix(color(255, 255, 255), 64);
}

static void focusing() {
	auto object = control::last->getobject();
	if(!object)
		return;
	addfocus(object);
	if(!getfocus())
		setfocus(object);
	auto current = getfocus();
	if(getfocus() == object)
		hilighting();
	else {
		if(hot.key == MouseLeft && hot.pressed) {
			if(ishilite())
				execute(post_setfocus, 0, 0, object);
		}
	}
}

static void execute_standart_command() {
	auto push_last = control::last;
	control::last = (control*)hot.object;
	if(hot.param == 1) {
		auto object = control::last->getobject();
		if(object)
			setfocus(object);
	}
	if(control::last->command)
		control::last->command->pexecute();
	control::last = push_last;
}

static void execute_standart(int param = 0) {
	execute(execute_standart_command, param, 0, control::last);
}

static void setcolor(int value) {
	fore = getcolor(value);
}

static void background() {
	auto p = gres(res::INTRFACE);
	if(!p)
		return;
	image(0, 0, p, p->ganim(control::last->normal, current_tick), ImageNoOffset);
}

static void background_center() {
	auto p = gres(res::INTRFACE);
	if(!p)
		return;
	auto frame = p->ganim(control::last->normal, current_tick);
	auto& f = p->get(frame);
	control::offset.x = (getwidth() - f.sx) / 2 + control::last->x;
	control::offset.y = (getheight() - f.sy) / 2 + control::last->y;
	image(control::offset.x, control::offset.y, p, frame, ImageNoOffset);
}

static void label() {
	text(control::view.title);
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
	char temp[260]; stringbuilder sb(temp);
	auto push_caret = caret;
	caret.x += (width - textw(control::view.title)) / 2;
	draw::text(control::view.title);
	caret = push_caret;
}

static void text_font3() {
	auto push_font = font;
	auto push_fore = fore;
	font = gres(res::FONT3);
	setcolor(ColorButton);
	text(control::view.title);
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
	if(control::view.checked)
		fore = getcolor(ColorCheck);
	focusing();
	label();
	label_right(control::view.value);
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
	image(ps, a ? pressed : normal, ImageNoOffset);
	return result;
}

static void text_block() {
	texta(control::view.title, control::view.flags);
	//rectb();
}

static void text_info() {
	auto push_fore = fore;
	fore = getcolor(ColorInfo);
	texta(control::view.title, 0);
	fore = push_fore;
}

static void text_button(bool rp) {
	char temp[260]; stringbuilder sb(temp);
	auto push_caret = caret;
	caret.x += (width - textw(control::view.title)) / 2 + (rp ? 1 : 0);
	caret.y += 7;
	label();
	caret = push_caret;
}

static bool focused() {
	return control::last->getobject() == getfocus();
}

static void button_no_text() {
	unsigned key = 0;
	if(focused())
		key = control::view.key;
	if(buttonf(control::last->normal, control::last->pressed, key, control::view.checked, false, 0, control::view.disabled))
		execute_standart(1);
}

static void button_def() {
	auto old_font = font;
	auto old_fore = fore;
	auto rp = false;
	auto r = buttonf(control::last->normal, control::last->pressed, control::view.key, false, false, &rp, false);
	font = gres(res::FONT3);
	setcolor(ColorButton);
	text_button(rp);
	if(r)
		execute_standart();
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
	caret.x += sx + (w1 - draw::textw(control::view.title)) / 2;
	text(control::view.title);
	caret = push_caret;
	last_rect = {caret.x, caret.y, caret.x + width, caret.y + sy};
	auto a = ishilite(last_rect);
	auto result = false;
	if(checkpressed(result, control::view.key))
		frame++;
	image(caret.x, caret.y + 1, ps, frame, ImageNoOffset);
	font = push_font;
	fore = push_fore;
	if(result)
		execute_standart();
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

static void numbersm(int digits, int value) {
	render_number(digits, value, gres(res::INTRFACE), 82, 0, 9, true);
}

static void numberap(int digits, int value) {
	render_number(digits, value, gres(res::INTRFACE), 290, 0, 10, false);
}

static void number_standart() {
	number(2, character::last->stats[control::last->getvalue()]);
}

static void number_custom() {
	number(2, control::view.number);
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

static void list_input(int& current, int& origin, int perpage, int perline, int maximum) {
	if(current >= maximum)
		current = maximum - 1;
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
		if(current < maximum - 1)
			execute(cbsetint, current + 1, 0, &current);
		break;
	case KeyHome:
		if(current)
			execute(cbsetint, 0, 0, &current);
		break;
	case KeyEnd:
		if(current != maximum - 1)
			execute(cbsetint, maximum - 1, 0, &current);
		break;
	case KeyPageUp:
		if(current)
			execute(cbsetint, current - perpage, 0, &current);
		break;
	case KeyPageDown:
		execute(cbsetint, current + perpage, 0, &current);
		break;
	}
}

static void list_paint(int& origin, int& current, int perline) {
	if(!perline)
		return;
	auto perpage = height / perline;
	if(!perpage)
		return;
	int maximum = control::view.source.count;
	list_input(current, origin, perpage, perline, maximum);
	if(maximum > origin + perpage + 1)
		maximum = origin + perpage + 1;
	auto push_height = height;
	height = perline;
	char temp[260]; stringbuilder sb(temp);
	for(auto i = origin; i < maximum; i++) {
		auto push_fore = fore;
		if(i == current)
			hilighting();
		if(ishilite() && hot.key == MouseLeft && hot.pressed)
			execute(cbsetint, i, 0, &current);
		auto object = control::view.source.ptr(i);
		if(object && control::view.pgetname) {
			sb.clear();
			control::view.pgetname(object, sb);
			text(temp);
		}
		fore = push_fore;
		caret.y += height;
	}
	height = push_height;
}

static void listview() {
	//rectb();
	list_paint(last_list_origin, last_list_current, 10);
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

static void custom_event() {
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

BSDATA(widget) = {
	{"Background", background},
	{"BackgroundC", background_center},
	{"Button", button_radio},
	{"ButtonDF", button_def},
	{"ButtonNT", button_no_text},
	{"Edit", edit},
	{"Event", custom_event},
	{"Information", block_information},
	{"Text", center_text_font2},
	{"TextBlock", text_block},
	{"TextInfo", text_info},
	{"TextL", text_font3},
	{"Label", label_left},
	{"LabelSM", label_checked},
	{"LineInfo", line_info},
	{"List", listview},
	{"Number", number_standart},
	{"NumberCustom", number_custom},
};
BSDATAF(widget)