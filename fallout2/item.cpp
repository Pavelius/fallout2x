#include "main.h"

item::item(const itemi* pi) {
	clear();
	if(!pi)
		return;
	type = getbsi(pi);
	count = pi->ammo.count ? pi->ammo.count : 1;
}

itemkind_s item::getkind() const {
	auto& ei = geti();
	if(ei.weapon.max)
		return Weapon;
	if(ei.armor.male != res::None)
		return Armor;
	if(ei.ammo.count != 0)
		return Ammo;
	if(ei.effect)
		return Drug;
	return Misc;
}

static void putbackpack(item& it, item& itv) {
	auto p = it.getowner();
	if(p)
		p->additem(itv);
}

static void put(item& it, item& itv) {
	auto slot = it.getownerslot();
	if(slot != Backpack && itv.isallow(slot))
		it = itv;
	else
		putbackpack(it, itv);
}

void item::transfer(item& i1, item& i2) {
	auto dest_slot = i2.getownerslot();
	if(!isallow(dest_slot))
		i1 = *this;
	else if(dest_slot==Backpack)
		putbackpack(i2, *this);
	else {
		auto i2_item = i2;
		i2.clear();
		put(i2, *this);
		put(i1, i2_item);
	}
	auto p1 = i1.getowner();
	auto p2 = i2.getowner();
	if(p1)
		p1->update();
	if(p2 && p2 != p1)
		p2->update();
}

bool item::isallow(wear_s id) const {
	if(!(*this))
		return true;
	auto type = getkind();
	switch(id) {
	case BodyArmor: return type == Armor;
	case LeftHandItem: case RightHandItem: return type == Weapon || type == Drug || type == Misc;
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