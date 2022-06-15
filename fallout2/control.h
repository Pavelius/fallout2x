#include "color.h"
#include "command.h"
#include "datasource.h"
#include "point.h"
#include "variant.h"
#include "widget.h"

#pragma once

struct control;
typedef void(*fncontrol)(const control& e);
struct decorator {
	const char*			id;
	fncontrol			proc;
};
struct control : point {
	struct guii : datasource {
		const char*		title;
		const char*		value;
		void*			object;
		int				number;
		bool			checked, disabled;
		unsigned		key;
		unsigned		flags;
		fnstatus		pgetname;
		void			clear() { memset(this, 0, sizeof(*this)); }
	};
	const widget*		type;
	variant				data;
	point				size;
	short				normal, pressed;
	const char*			text;
	const decorator*	format;
	const command*		command;
	static fncontrol	pbefore;
	static point		offset;
	static const control* last;
	int					getvalue() const;
	void				paint() const;
};
extern control::guii gui;