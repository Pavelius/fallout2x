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
	draw::image(gres(resource), frame, 0);
	draw::caret = push_caret;
}