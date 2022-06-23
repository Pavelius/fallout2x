#include "draw.h"
#include "main.h"

using namespace draw;

void spriteable::set(res r, short cicle) {
	resource = r; auto p = gres(resource);
	auto pc = p->gcicle(cicle);
	if(pc) {
		frame = pc->start;
		frame_start = frame;
		frame_stop = frame + pc->count;
	}
}

void spriteable::paint() const {
	auto push_caret = draw::caret;
	draw::caret = position;
	auto i = frame;
	if(frame_start != frame_stop)
		i = frame_start + (current_tick / 200) % (frame_stop - frame_start);
	if(resource == res::INVEN) {
		auto& f = gres(resource)->get(i);
		caret.x -= f.sx / 2;
		caret.y -= f.sy / 2;
		draw::image(gres(resource), i, ImageNoOffset);
	} else
		draw::image(gres(resource), i, 0);
	draw::caret = push_caret;
}