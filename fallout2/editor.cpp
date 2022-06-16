#include "draw.h"
#include "main.h"

using namespace draw;

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

void scenery::painted() const {
	auto push_clipping = clipping;
	auto pr = gres(res::SCENERY);
	clipping.set(caret.x, caret.y, caret.x + width, caret.y + height);
	image(caret.x + width / 2, caret.y + height - 32, pr, pr->ganim(frame, current_tick / 200), 0);
	clipping = push_clipping;
	if(ishilite()) {
		rectb();
		hilite_object = this;
		if(hot.key == MouseLeft && !hot.pressed)
			execute(buttonparam, (long)this);
	}
	auto push_caret = caret;
	caret.y += height - 32;
	texta(getname(), AlignCenter);
	caret = push_caret;
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

void choose_scenery() {
	scene(scenery_scene);
}

