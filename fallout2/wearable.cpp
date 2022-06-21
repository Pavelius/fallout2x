#include "main.h"

static itemwear* finditem() {
	for(auto& e : bsdata<itemwear>()) {
		if(!e)
			return &e;
	}
	return 0;
}

void wearable::additem(item v) {
	auto pc = static_cast<character*>(this);
	// Join into existing
	for(auto& e : bsdata<itemwear>()) {
		if(e.owner == pc) {
			e.join(v);
			if(!v)
				return;
		}
	}
	// Find vacant or add new
	auto p = finditem();
	if(!p)
		p = bsdata<itemwear>::add();
	copy<item>(*p, v);
	p->owner = static_cast<character*>(this);
}