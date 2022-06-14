#include "color.h"
#include "command.h"
#include "point.h"
#include "variant.h"
#include "widget.h"

#pragma once

struct control;
struct decorator {
	typedef void(*fnproc)(const control& e);
	const char*			id;
	fnproc				proc;
};
struct control : point {
	struct viewi {
		const char*		title;
		const char*		value;
		int				number;
		bool			checked, disabled;
		unsigned		key;
		unsigned		flags;
		fnstatus		pgetname;
		array			source;
		void			clear() { memset(this, 0, sizeof(*this)); }
	};
	const widget*		type;
	variant				data;
	point				size;
	short				normal, pressed, disabled;
	const char*			text;
	const decorator*	format;
	const command*		command;
	static point		offset;
	static const control* last;
	static viewi		view;
	const void*			getobject() const;
	int					getvalue() const;
	void				paint() const;
};
