#pragma once

struct datasource {
	void*			data;
	size_t			size;
	unsigned		mask;
	unsigned		count;
	long			get() const;
	const char*		getstr() const;
	template<typename T> void link(T& v) { data = (void*)&v; size = sizeof(T); mask = 0; }
	template<typename T> void link(T& v, unsigned m) { data = (void*)&v; size = sizeof(T); mask = m; }
	char*			ptr(int i) const { return (char*)data + size * i; }
	void			set(long v) const;
};
