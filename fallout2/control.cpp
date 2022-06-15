#include "crt.h"
#include "draw.h"
#include "control.h"
#include "widget.h"

using namespace draw;

control::guii gui;
fncontrol control::pbefore;
const control* control::last;
point control::offset;

void control::paint() const {
	rectpush push;
	auto push_fore = fore;
	auto push_last = last;
	caret.x = x + offset.x;
	caret.y = y + offset.y;
	width = size.x;
	height = size.y;
	gui.clear();
	last = this;
	if(text)
		gui.title = getnm(text);
	else if(data.type && data.geti().isnamed())
		gui.title = data.getname();
	else if(command)
		gui.title = getnm(command->id);
	else
		gui.title = type->id;
	if(command)
		gui.key = command->hotkey;
	if(pbefore)
		pbefore(*this);
	if(format)
		format->proc(*this);
	type->paint();
	last = push_last;
	fore = push_fore;
}

int control::getvalue() const {
	return data.value;
}