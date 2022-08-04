#include "bsreq.h"
#include "dialog.h"
#include "draw.h"
#include "io_stream.h"
#include "screenshoot.h"

using namespace draw;

BSMETA(dialog) = {
	BSREQ(id),
	{}};
BSDATAC(dialog, 256)

const dialog* dialog::last;
static screenshoot* screen;

void dialog::paint() {
	if(screen)
		screen->restore();
	gui.shift.clear();
	for(auto& e : dialog::last->controls)
		e.paint();
}

bool dialog::isfullscreen() const {
	if(!controls)
		return false;
	auto p = controls.begin();
	return (p->x == 0) && (p->y == 0);
}

int dialog::open() const {
	if(!controls)
		return 0;
	rectpush push;
	auto push_last = last;
	auto push_screen = screen;
	auto push_offset = gui.shift;
	last = this;
	if(isfullscreen()) {
		screen = 0;
		draw::scene(paint);
		paint();
	} else {
		screenshoot push(false);
		screen = &push;
		draw::scene(paint);
	}
	gui.shift = push_offset;
	last = push_last;
	screen = push_screen;
	return getresult();
}

static const char* getformfile(stringbuilder& sb, const char* id) {
	sb.clear(); sb.add("forms/%1.txt", id);
	return sb.begin();
}

static void read_forms() {
	char temp[260]; stringbuilder sb(temp);
	for(auto& e : bsdata<dialog>()) {
		auto pb = (control*)bsdata<control>::end();
		bsreq::read(getformfile(sb, e.id));
		auto pe = (control*)bsdata<control>::end();
		e.controls = slice<control>(pb, pe);
	}
}

void dialog::initialize() {
	char temp[260];
	for(io::file::find find("forms"); find; find.next()) {
		auto name = find.name();
		if(name[0] == '.')
			continue;
		if(!szpmatch(name, "*.txt"))
			continue;
		auto p = bsdata<dialog>::add();
		p->id = szdup(szfnamewe(temp, name));
	}
	read_forms();
}