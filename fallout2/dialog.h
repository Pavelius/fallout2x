#include "control.h"
#include "crt.h"

#pragma once

struct dialog {
	const char*		id;
	slice<control>	controls;
	int				open() const;
	static const dialog* last;
	static void		initialize();
	static void		paint();
	bool			isfullscreen() const;
};
