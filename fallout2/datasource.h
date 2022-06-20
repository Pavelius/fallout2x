#pragma once

struct datasource {
	void*			data;
	size_t			size;
	unsigned		mask;
	unsigned		count;
	long			get() const;
	const char*		getstr() const;
	bool			is() const { return (get() & mask) != 0; }
	template<typename T> void link(T& v) { data = (void*)&v; size = sizeof(T); mask = 0; }
	template<typename T> void link(T& v, unsigned m) { data = (void*)&v; size = sizeof(T); mask = m; }
	template<typename T> void linklist(T& v, unsigned c) { data = (void*)v; size = sizeof(v[0]); mask = 0; count = c; }
	char*			ptr(int i) const { return (char*)data + size * i; }
	void			set(long v) const;
};
