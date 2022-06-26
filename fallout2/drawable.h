#include "point.h"

#pragma once

struct drawable {
	typedef void(*fnevent)(const drawable* p);
	typedef int(*fnget)(const drawable* p);
	typedef drawable**(*fnselect)(drawable** ps, drawable* const* pe);
	point				position;
	const void*			data;
	int					timer;
	static fnget		getorder;
	static fnevent		paint;
	static fnselect		select;
	explicit operator bool() const { return data != 0; }
	static drawable*	add(point v, const void* object);
	void				clear();
	static drawable*	find(const void* object);
	static drawable*	find(const point pt);
	static drawable*	findadd(const point pt, const void* object);
};