#include "control.h"
#include "crt.h"

#pragma once

struct dialog {
	const char*		id;
	slice<control>	controls;
	static int		open(const char* id, fnevent pbefore);
	static int		open(const char* id) { return open(id, control::pbefore); }
	int				open(fnevent pbefore) const;
	static const dialog* last;
	static void		initialize();
	bool			isfullscreen() const;
};
