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

static void paint_dialog() {
	if(screen)
		screen->restore();
	for(auto& e : dialog::last->controls)
		e.paint();
}

int dialog::open(int mode) const {
	auto push_last = last;
	auto push_screen = screen;
	auto push_offset = control::offset;
	control::offset.clear();
	last = this;
	if(mode == 0) {
		screen = 0;
		draw::scene(paint_dialog);
	} else {
		screenshoot push(mode==2);
		screen = &push;
		draw::scene(paint_dialog);
	}
	control::offset = push_offset;
	last = push_last;
	screen = push_screen;
	return getresult();
}

int dialog::open(const char* id, int mode) {
	auto p = bsdata<dialog>::find(id);
	return p ? p->open(mode) : 0;
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