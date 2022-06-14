#include "datasource.h"

void datasource::set(long v) const {
	switch(size) {
	case 1: *((char*)data) = (char)v; break;
	case 2: *((short*)data) = (short)v; break;
	case 4: *((int*)data) = (int)v; break;
	case 8: *((long long*)data) = v; break;
	default: break;
	}
}

long datasource::get() const {
	switch(size) {
	case 1: return *((char*)data);
	case 2: return *((short*)data);
	case 4: return *((int*)data);
	default: return 0;
	}
}

const char* datasource::getstr() const {
	auto v = get();
	if(!v)
		return (const char*)v;
	return "";
}