#include "draw.h"
#include "main.h"

using namespace draw;

void spriteable::set(res r, short cicle) {
	resource = r;
	frame = cicle;
}

void spriteable::paint() const {
	auto ps = gres(resource);
	if(!ps)
		return;
	auto push_caret = draw::caret;
	draw::caret = position;
	auto i = ps->ganim(frame, current_tick / 150);
	if(resource == res::INVEN) {
		auto& f = gres(resource)->get(i);
		caret.x -= f.sx / 2;
		caret.y -= f.sy / 2;
		draw::image(gres(resource), i, ImageNoOffset);
	} else
		draw::image(gres(resource), i, 0);
	draw::caret = push_caret;
}