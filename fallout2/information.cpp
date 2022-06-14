#include "io_stream.h"
#include "main.h"

static void write(const char* url, const char* text) {
	io::file file(url, StreamWrite | StreamText);
	if(!file)
		return;
	file << text;
}

static void write_export(const char* folder, const char* id, const char* text) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("%1/%2.txt", folder, id);
	write(temp, text);
}

void character::exporting(const char* id) const {
	char temp[8192]; stringbuilder sb(temp);
	sb.addn(getname());
	bool need_separator = true;
	for(auto& e : bsdata<stati>()) {
		auto i = (stat_s)(&e - bsdata<stati>::elements);
		if(i == SmallGuns || i==HP)
			need_separator = true;
		auto v = get(i);
		if(!v)
			continue;
		if(need_separator) {
			sb.addn("---");
			need_separator = false;
		}
		sb.addn("%1: %2i", getnm(e.id), v);
	}
	write_export("characters/export", id, temp);
}