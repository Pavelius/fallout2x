#include "main.h"

item::item(const itemi* pi) {
	clear();
	if(!pi)
		return;
	type = getbsi(pi);
	count = pi->ammo.count ? pi->ammo.count : 1;
}

void item::transfer(item& from, item& to) {
	auto need_update = false;
	auto iswf = (bsdata<character>::source.indexof(&from)!=-1);
	auto iswt = (bsdata<character>::source.indexof(&to) != -1);
	from = *this;
}