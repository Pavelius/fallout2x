#include "draw.h"
#include "main.h"

using namespace draw;



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