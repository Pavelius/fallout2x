#include "main.h"

void itemlist::select(const wearable* pv) {
	auto ps = begin();
	auto pe = endof();
	for(auto& e : bsdata<itemwear>()) {
		if(e.owner != pv)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}