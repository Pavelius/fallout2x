#include "point.h"

#pragma once

struct drawable {
	point				position;
	void*				data;
	unsigned short		frame, frame_stop;
	unsigned			flags;
	static drawable*	add(point v, void* object);
	static drawable*	find(const void* object);
	static drawable*	find(const point pt);
	void				paint() const;
};