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

static const char* getflagsor(unsigned v, array& source) {
	static char temp[512]; stringbuilder sb(temp);
	auto c = 0;
	temp[0] = 0;
	auto m = source.getcount();
	for(unsigned i = 0; i < m; i++) {
		auto& e = *((lighti*)source.ptr(i));
		if((v & e.value) == 0)
			continue;
		sb.adds(e.id);
		c++;
	}
	return temp;
}

void sceneryi::getinfoed(stringbuilder& sb) const {
	sb.adds("(%1i, %2i)", index, frame);
	sb.adds(getflagsor(light, bsdata<lighti>::source));
	sb.adds(getflagsor(object, bsdata<objectfi>::source));
	sb.adds(getdescription(id));
}

void walli::getinfoed(stringbuilder& sb) const {
	sb.adds("(%1i %2i)", index, frame);
	sb.adds(getflagsor(light, bsdata<lighti>::source));
	sb.adds(getflagsor(object, bsdata<objectfi>::source));
	sb.adds(getdescription(id));
}

void tilegroup::getinfoed(stringbuilder& sb) const {
	sb.adds("%1i-%2i", getbeginid(), getendid());
}

void tilegroup::getinfolist(stringbuilder& sb) const {
	sb.adds("%1i-%2i", getbeginid(), getendid());
	auto index = start;
	auto line = 1;
	for(auto& e : *this) {
		sb.addn("#%5i %1i-%2i (%3i,%4i) %6i", index, index + e.count - 1, e.shift.x, e.shift.y, line, e.count);
		index += e.count;
		line++;
	}
}

void item::getinfo(stringbuilder& sb) const {
	auto& ei = geti();
	sb.add(getname());
	sb.addn("---");
	sb.addn(getdescription(ei.id));
	sb.add("\n\n");
	sb.addn("%Weight: %1i", ei.weight);
}