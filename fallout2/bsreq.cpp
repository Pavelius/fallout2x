#include "bsreq.h"
#include "crt.h"
#include "stringbuilder.h"

BSMETA(int) = {{"number"}, {}};
BSMETA(const char*) = {{"text"}, {}};
BSMETA(bsreq) = {
	BSREQ(id),
	BSREQ(shift),
	BSREQ(size),
	BSREQ(lenght),
	BSREQ(count),
	BSREQ(type),
{}};

const bsreq* bsreq::find(const char* name) const {
	if(!name || name[0] == 0)
		return 0;
	for(auto p = this; p->id; p++) {
		if(strcmp(p->id, name) == 0)
			return p;
	}
	return 0;
}

const bsreq* bsreq::find(const char* name, unsigned count) const {
	if(!name || name[0] == 0)
		return 0;
	for(auto p = this; p->id; p++) {
		if(memcmp(p->id, name, count) == 0)
			return p;
	}
	return 0;
}

const bsreq* bsreq::find(const char* name, const bsreq* type) const {
	for(auto p = this; p->id; p++) {
		if(p->type != type)
			continue;
		if(strcmp(p->id, name) == 0)
			return p;
	}
	return 0;
}

int bsreq::findenum(const char* name) const {
	if(!source || !type)
		return -1;
	auto pn = type->find("id", bsmeta<const char*>::meta);
	if(!pn)
		return -1;
	return source->find(name, pn->shift);
}

long bsreq::get(const void* p) const {
	switch(size) {
	case sizeof(char) : return *((char*)p);
	case sizeof(short) : return *((short*)p);
	case sizeof(long) : return *((long*)p);
	default: return *((int*)p);
	}
}

void bsreq::set(const void* p, long value) const {
	switch(size) {
	case sizeof(char) : *((char*)p) = (char)value; break;
	case sizeof(short) : *((short*)p) = (short)value; break;
	case sizeof(long) : *((long*)p) = value; break;
	default: *((int*)p) = value; break;
	}
}

bool bsreq::match(const void* p, const char* name) const {
	auto value = (const char*)get(p);
	if(!value || type != bsmeta<const char*>::meta)
		return false;
	for(int i = 0; name[i]; i++) {
		if(value[i] != name[i])
			return false;
	}
	return true;
}

const bsreq* bsreq::getname() const {
	auto p = find("name", bsmeta<const char*>::meta);
	if(!p)
		p = find("id", bsmeta<const char*>::meta);
	if(!p)
		p = find("text", bsmeta<const char*>::meta);
	return p;
}

const char* bsreq::gets(const void* p) const {
	if(is(KindText)) {
		auto v = (const char*)get(p);
		if(!v)
			return "";
		return v;
	} else if(is(KindReference)) {
		auto pf = type->getname();
		if(!pf)
			return "";
		auto v = (void*)get(p);
		if(!v)
			return "";
		return pf->gets(pf->ptr(v));
	} else if(is(KindEnum)) {
		auto pb = source;
		if(!pb)
			return "";
		auto pf = type->getname();
		if(!pf)
			return "";
		auto vi = get(p);
		auto v = (void*)pb->ptr(vi);
		return pf->gets(pf->ptr(v));
	}
	return "";
}

const char* bsreq::get(const void* p, char* result, const char* result_max) const {
	if(is(KindNumber)) {
		auto v = get(p);
		stringbuilder sb(result, result_max);
		sb.add("%1i", v);
		return result;
	}
	return gets(p);
}

bool bsreq::equal(const void* v1, const void* v2) const {
	if(type == bsmeta<const char*>::meta) {
		auto p1 = *((const char**)ptr(v1));
		if(!p1)
			p1 = "";
		auto p2 = *((const char**)ptr(v2));
		if(!p2)
			p2 = "";
		return strcmp(p1, p2);
	} else
		return memcmp(ptr(v1), ptr(v2), lenght);
}