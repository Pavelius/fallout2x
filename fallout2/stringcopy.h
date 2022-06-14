#include "crt.h"

#pragma once

class stringcopy : public adat<char, 4096> {
	const char*		addcopy(const char* v);
	const char*		find(const char* v, unsigned len) const;
public:
	const char*		add(const char* v);
};