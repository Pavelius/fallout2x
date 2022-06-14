#include "stringlist.h"
#include "stringbuilder.h"

const char* stringcopy::find(const char* v, unsigned len) const {
	if(v && len) {
		auto pe = data + sizeof(data);
		auto s = *v;
		for(auto p = data; p < pe; p++) {
			if(*p != s)
				continue;
			unsigned n1 = pe - p - 1;
			if(n1 < len)
				return 0;
			if(memcmp(p + 1, v + 1, len) == 0)
				return p;
		}
	}
	return 0;
}

const char* stringcopy::addcopy(const char* v) {
	auto result = data + count;
	stringbuilder sb(result, endof() - 1);
	sb.copy(v); count += zlen(result) + 1;
	return result;
}

const char* stringcopy::add(const char* v) {
	if(!v || v[0] == 0)
		return 0;
	auto c = zlen(v);
	auto p = find(v, c);
	if(p)
		return p;
	return addcopy(v);
}