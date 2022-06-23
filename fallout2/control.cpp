#include "crt.h"
#include "draw.h"
#include "control.h"
#include "widget.h"

using namespace draw;

guii gui;
fncontrol control::pbefore;
point guii::shift;

void control::paint() const {
	auto push_fore = fore;
	caret.x = x + gui.shift.x;
	caret.y = y + gui.shift.y;
	width = size.x;
	height = size.y;
	gui.clear();
	gui.normal = normal;
	gui.pressed = pressed;
	gui.lines = lines;
	if(text)
		gui.title = getnm(text);
	else if(data.type && data.geti().isnamed()) {
		gui.id = data.getid();
		gui.title = data.getname();
	} else if(command) {
		gui.id = command->id;
		gui.title = getnm(gui.id);
	} else {
		gui.id = type->id;
		gui.title = type->id;
	}
	if(command) {
		gui.key = command->hotkey;
		gui.execute = command->pexecute;
	}
	if(pbefore)
		pbefore(*this);
	if(format)
		format->proc(*this);
	type->paint();
	fore = push_fore;
}