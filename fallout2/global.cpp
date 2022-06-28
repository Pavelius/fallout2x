#include "main.h"

int getglobal(const char* id) {
	auto p = bsdata<globalstati>::find(id);
	if(p)
		return p->value;
	return 0;
}
