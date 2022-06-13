#pragma once

typedef void(*fnevent)();
struct widget {
	const char*	id;
	fnevent		proc;
	static const widget* last;
	void		paint() const;
};
