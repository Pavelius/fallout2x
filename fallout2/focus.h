#pragma once

namespace draw {
void		addfocus(const void* object);
const void*	getfocus();
const void*	getnext(const void* id, int key);
void		setfocus(const void* v);
}
