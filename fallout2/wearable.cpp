#include "main.h"

void wearable::add(const item& v) {
	auto p = bsdata<itemwear>::add();
	copy<item>(*p, v);
	p->owner = this;
}