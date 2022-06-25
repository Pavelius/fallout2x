#include "point.h"

#pragma once

enum drawablef : unsigned char {
	WaitNewAnimation
};

struct drawable {
	typedef void(*fnevent)(const drawable* p);
	typedef int(*fnget)(const drawable* p);
	typedef drawable**(*fnselect)(drawable** ps, drawable* const* pe);
	point				position;
	const void*			data;
	unsigned short		frame_start, frame_stop;
	unsigned short		frame, flags;
	int					timer;
	static fnget		getorder;
	static fnevent		paint;
	static fnselect		select;
	explicit operator bool() const { return data != 0; }
	static drawable*	add(point v, const void* object);
	void				clear();
	bool				is(drawablef v) const { return (flags & (1 << v)) != 0; }
	void				remove(drawablef v) { flags &= ~(1 << v); }
	void				set(drawablef v) { flags |= (1 << v); }
	void				setanimate(unsigned short frame, unsigned short count);
	static drawable*	find(const void* object);
	static drawable*	find(const point pt);
	static drawable*	findadd(const point pt, const void* object);
	void				updateframe();
};