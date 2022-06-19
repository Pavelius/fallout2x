#include "draw.h"
#include "main.h"
#include "archive.h"

template<> void archive::set<character>(character& e) {
	set(e.id);
	set(e.basic);
	set(e.perks);
	set(e.tags);
	set(e.experience);
	set(e.species);
	set(e.naked);
}

template<> void archive::set<areai>(areai& e) {
	set(&e, sizeof(e));
}

template<> void archive::set<drawable>(drawable& e) {
	set(e.position);
	set(e.data);
	set(e.frame);
	set(e.frame_stop);
	set(e.flags);
}

static void serial(character* p, const char* id, bool writemode) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("characters/premade/%1.chr", id);
	io::file file(temp, writemode ? StreamWrite : StreamRead);
	if(!file)
		return;
	archive a(file, writemode);
	a.set(*p);
}

static void serial(areai* p, const char* id, bool writemode) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("maps/%1.dat", id);
	io::file file(temp, writemode ? StreamWrite : StreamRead);
	if(!file)
		return;
	archive a(file, writemode);
	if(!a.signature("MAP"))
		return;
	if(!a.version(0, 1))
		return;
	a.set(draw::camera);
	a.set(*p);
	a.setc<drawable>(bsdata<drawable>::source);
}

void character::write(const char* id) const {
	serial(const_cast<character*>(this), id, true);
}

void character::read(const char* id) {
	serial(const_cast<character*>(this), id, false);
	update();
}

void areai::write(const char* id) const {
	serial(const_cast<areai*>(this), id, true);
}

void areai::read(const char* id) {
	serial(this, id, false);
}