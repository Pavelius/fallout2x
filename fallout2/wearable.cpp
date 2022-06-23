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
	p->owner = pc;
}

void wearable::updateaction() {
	auto actions = getactions();
	if(action_index[0] >= actions.getcount())
		action_index[0] = 0;
	if(action_index[0] < 0)
		action_index[0] = 0;
}