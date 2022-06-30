#include "crt.h"
#include "stringbuilder.h"

#pragma once

class answers {
	struct element {
		const void* value;
		const char* text;
	};
	char				buffer[4096];
	stringbuilder		sc;
	adat<element, 32>	elements;
public:
	answers() : sc(buffer) {}
	constexpr operator bool() const { return elements.count != 0; }
	void				add(const void* value, const char* name, ...) { addv(value, name, xva_start(name)); }
	void				addv(const void* value, const char* name, const char* format);
	const element*		begin() const { return elements.data; }
	element*			begin() { return elements.data; }
	void*				choose(const char* prompt, const char* cancel) const;
	void				clear();
	static int			compare(const void* v1, const void* v2);
	const element*		end() const { return elements.end(); }
	int					getcount() const { return elements.getcount(); }
	const char*			getname(void* v);
	int					indexof(const void* v) const { return elements.indexof(v); }
	static void			message(const char* format);
	void*				random() const;
	void				remove(int index) { elements.remove(index, 1); }
	void				sort();
};