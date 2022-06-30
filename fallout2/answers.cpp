#include "answers.h"

int answers::compare(const void* v1, const void* v2) {
	return strcmp(((answers::element*)v1)->text, ((answers::element*)v2)->text);
}

void answers::addv(const void* value, const char* text, const char* format) {
	auto p = elements.add();
	p->value = value;
	p->text = sc.get();
	sc.addv(text, format);
	sc.addsz();
}

void answers::sort() {
	qsort(elements.data, elements.count, sizeof(elements.data[0]), compare);
}

void* answers::random() const {
	if(!elements.count)
		return 0;
	return (void*)elements.data[rand() % elements.count].value;
}

const char* answers::getname(void* v) {
	for(auto& e : elements) {
		if(e.value == v)
			return e.text;
	}
	return 0;
}

void answers::clear() {
	elements.clear();
	sc.clear();
}

static const char* find_separator(const char* pb) {
	auto p = pb;
	while(*p) {
		if(*p == '-' && p[1] == '=' && p[2] == '-' && (p[3] == 10 || p[3] == 13) && p > pb && (p[-1] == 10 || p[-1] == 13))
			return p;
		p++;
	}
	return 0;
}

const char* answers::message(const char* format) {
	if(!format)
		return 0;
	while(true) {
		auto p = find_separator(format);
		if(!p)
			break;
		auto pn = skipspcr(p + 3);
		while(p < format && (p[-1] == 10 || p[-1] == 13))
			p--;
		auto count = p - format;
		answers an; an.clear();
		if(count > sizeof(an.buffer) / sizeof(an.buffer[0]) - 1)
			count = sizeof(an.buffer) / sizeof(an.buffer[0]) - 1;
		memcpy(an.buffer, format, count);
		an.buffer[count] = 0;
		an.choose(an.buffer, getnm("Continue"));
		format = pn;
	}
	return format;
}