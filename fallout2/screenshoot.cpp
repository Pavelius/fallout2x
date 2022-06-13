﻿#include "draw.h"
#include "screenshoot.h"

using namespace draw;

screenshoot::screenshoot(rect rc, bool fade) : surface(rc.width(), rc.height(), getbpp()) {
	x = rc.x1;
	y = rc.y1;
	if(draw::canvas) {
		blit(*this, 0, 0, width, height, 0, *draw::canvas, x, y);
		if(fade) {
			auto push_canvas = canvas;
			auto push_clip = clipping;
			auto push_alpha = alpha;
			auto push_fore = fore;
			auto push_caret = caret;
			canvas = this;
			setclip();
			alpha = 128;
			fore = color(0,0,0);
			caret.x = 0;
			caret.y = 0;
			rectf();
			caret = push_caret;
			fore = push_fore;
			alpha = push_alpha;
			clipping = push_clip;
			canvas = push_canvas;
		}
	}
}

screenshoot::screenshoot(bool fade) : screenshoot({0, 0, getwidth(), getheight()}, fade) {
}

screenshoot::~screenshoot() {
}

void screenshoot::restore() {
	setclip();
	if(draw::canvas)
		blit(*draw::canvas, x, y, width, height, 0, *this, 0, 0);
}