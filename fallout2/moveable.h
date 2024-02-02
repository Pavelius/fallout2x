#pragma once
#include "area.h"

struct moveable {
	indext*			path_start;
	indext*			path;
	moveable() : path_start(0), path(0) {}
	~moveable() { clearpath(); }
	void			clearpath();
	bool			ismoving() const { return path_start != 0; }
	void			makepath(indext start);
	void			makepath(indext start, indext goal);
};
