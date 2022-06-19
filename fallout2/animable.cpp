#include "draw.h"
#include "main.h"

using namespace draw;

drawable* animable::getui() const {
	return drawable::find(this);
}

short animable::getframe(animate_s v, int weapon_index) {
	if(weapon_index > 0) {
		switch(v) {
		case AnimateStand: v = AnimateWeaponStand; break;
		case AnimateWalk: v = AnimateWeaponWalk; break;
		case AnimateDodge: v = AnimateWeaponDodge; break;
		case AnimateThrown: v = AnimateWeaponThrow; break;
		case AnimateUnarmed1: v = AnimateWeaponSwing; break;
		case AnimateUnarmed2: v = AnimateWeaponThrust; break;
		}
		return (v + (weapon_index - 1) * 13) * 6;
	}
	return v * 6;
}

void animable::appear(point h) {
	auto p = getui();
	if(!p) {
		p = drawable::add(h2s(h), this);
		p->frame = getframe(direction);
	}
}

void animable::focusing() const {
	auto p = getui();
	if(p) {
		auto pt = p->position;
		pt.x -= 640 / 2;
		pt.y -= 480 / 2;
		camera = pt;
	}
}

short animable::getframe(direction_s d) {
	switch(d) {
	case RightUp: return 0;
	case Right: return 1;
	case RightDown: return 2;
	case LeftDown: return 3;
	case Left: return 4;
	case LeftUp: return 5;
	default: return 0;
	}
}