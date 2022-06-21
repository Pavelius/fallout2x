#include "main.h"

item::item(const itemi* pi) {
	clear();
	if(!pi)
		return;
	type = getbsi(pi);
	count = pi->ammo.count ? pi->ammo.count : 1;
}

static void put(item& it, item itv) {
	auto slot = it.getownerslot();
	if(slot == Backpack) {
		auto p = it.getowner();
		if(p)
			p->additem(itv);
	} else
		it = itv;
}

void item::transfer(item& i1, item& i2) {
	auto i1_item = *this;
	auto i2_item = i2;
	if(i2.geti().isallow(i1.getownerslot()) && i1.geti().isallow(i2.getownerslot())) {
		put(i2, i1_item);
		put(i1, i2_item);
	} else
		i1 = *this;
}

bool itemi::isallow(wear_s id) const {
	switch(id) {
	case BodyArmor: return armor.male != res::None;
	case LeftHandItem: case RightHandItem: return weapon.max != 0; break;
	default: return true;
	}
}

character* item::getowner() const {
	auto i = bsdata<character>::source.indexof(this);
	if(i != -1)
		return bsdata<character>::elements + i;
	if(bsdata<itemwear>::have(this))
		return static_cast<itemwear*>(const_cast<item*>(this))->owner;
	return 0;
}

wear_s item::getownerslot() const {
	auto i = bsdata<character>::source.indexof(this);
	if(i != -1) {
		auto p = bsdata<character>::elements + i;
		if(this >= p->wear && this <= p->wear + LeftHandItem)
			return (wear_s)(this - p->wear);
	}
	return Backpack;
}

void item::join(item& v) {
	if(v.type != type || v.ammo_type != v.ammo_type)
		return;
	count += v.count;
	ammo_count += v.ammo_count;
	v.clear();
}