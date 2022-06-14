#include "crt.h"
#include "draw.h"
#include "control.h"
#include "widget.h"

using namespace draw;

fnevent control::pbefore;
control::viewi control::view;
const control* control::last;
point control::offset;

void control::paint() const {
	rectpush push;
	auto push_view = view;
	auto push_fore = fore;
	auto push_last = last;
	caret.x = x + offset.x;
	caret.y = y + offset.y;
	width = size.x;
	height = size.y;
	view.clear();
	last = this;
	if(text)
		view.title = getnm(text);
	else if(data.type && data.geti().isnamed())
		view.title = data.getname();
	else if(command)
		view.title = getnm(command->id);
	else
		view.title = type->id;
	if(command)
		view.key = command->hotkey;
	if(pbefore)
		pbefore();
	if(format)
		format->proc(*this);
	type->paint();
	last = push_last;
	fore = push_fore;
	view = push_view;
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