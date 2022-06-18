#include "point.h"

#pragma once

struct drawable {
	typedef void(*fnevent)(const drawable* p);
	typedef int(*fnget)(const drawable* p);
	point				position;
	const void*			data;
	unsigned short		frame, frame_stop;
	unsigned			flags;
	static drawable*	add(point v, const void* object);
	static drawable*	find(const void* object);
	static drawable*	find(const point pt);
	static drawable*	findadd(const point pt, const void* object);
	static fnget		getorder;
	static fnevent		paint;
};