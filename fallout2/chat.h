#include "variant.h"

#pragma once

struct speech {
	short			param, next;
	const char*		text;
	variants		effect;
	constexpr bool	isanswer() const { return next != -1; }
};
struct chat {
	const char*		id;
	sliceu<speech>	elements;
	static const char* folder;
	void			read();
	static void		readall();
};
