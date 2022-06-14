#pragma once

struct datasource {
	void*			data;
	size_t			size;
	unsigned		mask;
	long			get() const;
	const char*		getstr() const;
	template<typename T> void link(T& v) { data = (void*)&v; size = sizeof(T); mask = 0; }
	template<typename T> void link(T& v, unsigned m) { data = (void*)&v; size = sizeof(T); mask = m; }
	void			set(long v) const;
};
