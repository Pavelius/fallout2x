#include "main.h"

static itemwear* finditem() {
	for(auto& e : bsdata<itemwear>()) {
		if(!e)
			return &e;
	}
	return 0;
}

void wearable::additem(const item& v) {
	auto p = finditem();
	if(!p)
		p = bsdata<itemwear>::add();
	copy<item>(*p, v);
	p->owner = this;
}