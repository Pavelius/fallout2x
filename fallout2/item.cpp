#include "main.h"

item::item(const char* id) {
	clear();
	auto pi = bsdata<itemi>::find(id);
	if(!pi)
		return;
	type = getbsi(pi);
	count = pi->ammo.count ? pi->ammo.count : 1;
}