#include "crt.h"

#pragma once

class stringlib {
	array		indecies;
	array		content;
	const char*	getcontent(unsigned i) const { return (const char*)content.ptr(i); }
public:
	stringlib() : indecies(sizeof(unsigned)), content(1) {}
	unsigned	add(const char* v);
	void		clear();
	unsigned	find(const char* v) const;
	const char* get(unsigned i) const;
	size_t		getcount() const { return indecies.getcount(); }
};

