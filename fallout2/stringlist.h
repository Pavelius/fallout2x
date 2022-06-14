#include "stringcopy.h"

#pragma once

class stringlist : public adat<const char*, 512> {
	stringcopy		content;
public:
	void			add(const char* v) { adat::add(content.add(v)); }
};