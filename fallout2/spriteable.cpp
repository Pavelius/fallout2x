#include "draw.h"
#include "main.h"

using namespace draw;

void spriteable::set(res r, short cicle) {
	resource = r; auto p = gres(resource);
	auto pc = p->gcicle(cicle);
	if(pc) {
		frame = pc->start;
		frame_stop = frame + pc->count;
	}
}

void spriteable::paint() const {
	auto push_caret = draw::caret;
	draw::caret = position;
	if(resource == res::INVEN) {
		auto& f = gres(resource)->get(frame);
		caret.x -= f.sx / 2;
		caret.y -= f.sy / 2;
		draw::image(gres(resource), frame, ImageNoOffset);
	} else
		draw::image(gres(resource), frame, 0);
	draw::caret = push_caret;
}