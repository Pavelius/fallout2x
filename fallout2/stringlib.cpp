#include "stringlib.h"

void stringlib::clear() {
	content.clear();
	indecies.clear();
}

unsigned stringlib::find(const char* v) const {
	if(!v || v[0] == 0)
		return 0;
	auto pe = (unsigned*)indecies.end();
	for(auto pi = (unsigned*)indecies.data + 1; pi < pe; pi++) {
		if(equal(v, (char*)content.data + *pi))
			return pi - (unsigned*)indecies.data;
	}
	return 0;
}

const char* stringlib::get(unsigned i) const {
	if(!i || !content.data)
		return "";
	return (char*)content.data + ((unsigned*)indecies.data)[i];
}

unsigned stringlib::add(const char* v) {
	auto i = find(v);
	if(i)
		return i;
	i = indecies.count;
	if(!i) {
		indecies.add();
		((unsigned*)indecies.data)[0] = 0;
		i++;
	}
	auto n = zlen(v);
	unsigned z = content.count;
	content.count += n + 1;
	content.reserve(content.count);
	memcpy((char*)content.data + z, v, n);
	((char*)content.data)[z + n] = 0;
	indecies.add(&z);
	return i;
}