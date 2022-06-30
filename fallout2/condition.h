#pragma once

struct conditioni {
	typedef bool(*fnproc)(int counter, int param);
	const char*	id;
	fnproc		proc;
	int			param;
	static conditioni* add(const char* id, fnproc proc, int param = 0);
};
