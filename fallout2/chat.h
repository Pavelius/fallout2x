#include "variant.h"

#pragma once

struct speech {
	short			index, next;
	const char*		text;
	variants		tags;
	void			clear() { memset(this, 0, sizeof(*this)); }
	bool			isallow() const;
	constexpr bool	isanswer() const { return next != -1; }
	bool			isdialog() const;
};
struct chat {
	const char*		id;
	sliceu<speech>	elements;
	static chat*	last;
	static const char* folder;
	const speech*	find(short index);
	void			read();
	static void		readall();
};
