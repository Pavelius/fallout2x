#pragma once

struct command {
	typedef void(*fnevent)();
	const char*		id;
	unsigned		hotkey;
	fnevent			pexecute;
};