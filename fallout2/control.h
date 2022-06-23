#include "color.h"
#include "command.h"
#include "datasource.h"
#include "point.h"
#include "variant.h"
#include "widget.h"

#pragma once

struct control;
typedef void(*fncontrol)(const control& e);
struct guii : datasource {
	const char*			id;
	const char*			title;
	const char*			value;
	void*				object;
	int					number;
	bool				checked, disabled;
	short				normal, pressed, lines;
	unsigned			key;
	unsigned			flags;
	fnstatus			pgetname;
	fnevent				execute;
	static point		shift;
	void				clear() { memset(this, 0, sizeof(*this)); }
};
extern guii gui;
struct decorator {
	const char*			id;
	fncontrol			proc;
};
struct control : point {
	const widget*		type;
	variant				data;
	point				size;
	short				normal, pressed, lines;
	const char*			text;
	const decorator*	format;
	const command*		command;
	static fncontrol	pbefore;
	void				paint() const;
};