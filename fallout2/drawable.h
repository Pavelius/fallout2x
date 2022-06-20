#include "point.h"

#pragma once

struct drawable {
	typedef void(*fnevent)(const drawable* p);
	typedef int(*fnget)(const drawable* p);
	point				position;
	const void*			data;
	unsigned short		frame_start, frame_stop;
	unsigned short		frame, flags;
	int					timer;
	static fnget		getorder;
	static fnevent		paint;
	explicit operator bool() const { return data != 0; }
	static drawable*	add(point v, const void* object);
	void				clear();
	void				setanimate(unsigned short frame, unsigned short count);
	static drawable*	find(const void* object);
	static drawable*	find(const point pt);
	static drawable*	findadd(const point pt, const void* object);
};