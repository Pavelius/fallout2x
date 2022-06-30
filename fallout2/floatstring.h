#include "drawable.h"

#pragma once

struct floatstring : drawable {
	const char*		text;
	unsigned long	stop;
	explicit operator bool() const { return text != 0; }
	void			clear();
	void			paint() const;
};

void addstring(point position, const char* string, unsigned long delay = 2000, void* data = 0);
void update_floatstring();
