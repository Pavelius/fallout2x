#include "main.h"
#include "archive.h"

template<> void archive::set<character>(character& e) {
	set(e.id);
	set(e.basic);
	set(e.perks);
	set(e.tags);
	set(e.experience);
	set(e.boss);
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

void character::write(const char* id) const {
	serial(const_cast<character*>(this), id, true);
}

void character::read(const char* id) {
	serial(const_cast<character*>(this), id, false);
	update();
}