#include "draw.h"
#include "main.h"

void spriteable::set(res r, short cicle) {
	resource = r; auto p = gres(resource);
	auto pc = p->gcicle(cicle);
	if(pc) {
		frame = pc->start;
		frame_stop = frame + pc->count;
	}
}

void spriteable::paint() const {
	draw::image(gres(resource), frame, 0);
}