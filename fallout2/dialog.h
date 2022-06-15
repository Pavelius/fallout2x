#include "control.h"
#include "crt.h"

#pragma once

struct dialog {
	const char*		id;
	slice<control>	controls;
	static int		open(const char* id);
	int				open() const;
	static const dialog* last;
	static void		initialize();
	bool			isfullscreen() const;
};
