#include "crt.h"
#include "draw.h"
#include "control.h"
#include "widget.h"

using namespace draw;

control::viewi control::view;
const control* control::last;
point control::offset;

void control::paint() const {
	auto push_caret = caret;
	auto push_width = width;
	auto push_height = height;
	auto push_view = view;
	auto push_fore = fore;
	caret.x = x + offset.x;
	caret.y = y + offset.y;
	width = size.x;
	height = size.y;
	view.clear();
	if(text)
		view.title = getnm(text);
	else if(data.type)
		view.title = data.getname();
	else if(command)
		view.title = getnm(command->id);
	else
		view.title = type->id;
	if(command)
		view.key = command->hotkey;
	if(format)
		format->proc(*this);
	auto push_last = last;
	last = this; type->paint();
	last = push_last;
	fore = push_fore;
	view = push_view;
	height = push_height;
	width = push_width;
	caret = push_caret;
}

int control::getvalue() const {
	return data.value;
}

const void* control::getobject() const {
	auto& ei = data.geti();
	if(!ei.source)
		return 0;
	return ei.source->ptr(data.value);
}