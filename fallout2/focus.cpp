#include "crt.h"
#include "draw.h"
#include "focus.h"

using namespace draw;

struct focusi {
	const void*	object;
	rect		rc;
};
BSDATAC(focusi, 128)

static const void*	current_focus;

void draw::setfocus(const void* v) {
	current_focus = v;
}

static void event_focus() {
	setfocus(hot.object);
}

const void* draw::getfocus() {
	return current_focus;
}

static const focusi* getby(const void* object) {
	for(auto& e : bsdata<focusi>()) {
		if(e.object == object)
			return &e;
	}
	return 0;
}

static focusi* getfirst() {
	if(bsdata<focusi>::source.getcount() != 0)
		return bsdata<focusi>::elements;
	return 0;
}

static focusi* getlast() {
	if(bsdata<focusi>::source.getcount() != 0)
		return (focusi*)bsdata<focusi>::source.end();
	return 0;
}

static const focusi* getnext(const focusi* pc, int key) {
	if(!key)
		return pc;
	auto pf = getfirst();
	if(!pc)
		pc = getfirst();
	if(!pc)
		return 0;
	auto pe = pc;
	auto pl = getlast();
	int inc = 1;
	if(key == KeyLeft || key == KeyUp || key == (KeyTab | Shift))
		inc = -1;
	while(true) {
		pc += inc;
		if(pc > pl)
			pc = pf;
		else if(pc < pf)
			pc = pl;
		if(pe == pc)
			return pe;
		switch(key) {
		case KeyRight:
			if(pe->rc.y1 >= pc->rc.y1
				&& pe->rc.y1 <= pc->rc.y2
				&& pe->rc.x1 < pc->rc.x1)
				return pc;
			break;
		case KeyLeft:
			if(pe->rc.y1 >= pc->rc.y1
				&& pe->rc.y1 <= pc->rc.y2
				&& pe->rc.x1 > pc->rc.x1)
				return pc;
			break;
		case KeyDown:
			if(pc->rc.y1 >= pe->rc.y2)
				return pc;
			break;
		case KeyUp:
			if(pc->rc.y2 <= pe->rc.y1)
				return pc;
			break;
		default:
			return pc;
		}
	}
}

const void* draw::getnext(const void* id, int key) {
	auto p = getby(id);
	if(!p)
		return id;
	p = getnext(p, key);
	if(!p)
		return id;
	return p->object;
}

void draw::addfocus(const void* object) {
	auto p = bsdata<focusi>::add();
	p->object = object;
	p->rc = {caret.x, caret.y, caret.x + width, caret.y + height};
}

void focus_beforemodal() {
	bsdata<focusi>::source.clear();
}

void post_setfocus() {
	setfocus(hot.object);
}

void focus_finish() {
	const void* current;
	if(!getfocus())
		return;
	switch(hot.key) {
	case KeyLeft:
	case KeyRight:
	case KeyUp:
	case KeyDown:
		current = getnext(getfocus(), hot.key);
		if(current != getfocus())
			execute(post_setfocus, 0, 0, current);
		break;
	}
}