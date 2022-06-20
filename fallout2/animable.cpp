#include "draw.h"
#include "main.h"

using namespace draw;

BSDATA(animationi) = {
	{"AnimateStand"},
	{"AnimateWalk"},
	{"AnimatePickup"},
	{"AnimateUse"},
	{"AnimateDodge"},
	{"AnimateDamaged"},
	{"AnimateDamagedRear"},
	{"AnimateUnarmed1"},
	{"AnimateUnarmed2"},
	{"AnimateThrown"},
	{"AnimateRun"},
	{"AnimateKnockOutBack", AnimateDeadBackNoBlood, AnimateBloodedBack},
	{"AnimateKnockOutForward", AnimateDeadForwardNoBlood, AnimateBloodedForward},
	{"AnimateKilledSingle", AnimateDeadSingle},
	{"AnimateKilledBurst", AnimateDeadBurst},
	{"AnimateKilledBurstAuto", AnimateDeadBurstAuto},
	{"AnimateKilledBlowup", AnimateDeadBlowup},
	{"AnimateKilledMelt", AnimateDeadMelt},
	{"AnimateBloodedBack", AnimateDeadBack},
	{"AnimateBloodedForward", AnimateDeadForward},
	{"AnimateStandUpBack"},
	{"AnimateStandUpForward"},
	{"AnimateDeadBackNoBlood"},
	{"AnimateDeadForwardNoBlood"},
	{"AnimateDeadSingle", AnimateDeadSingle},
	{"AnimateDeadBurst", AnimateDeadBurst},
	{"AnimateDeadBurstAuto", AnimateDeadBurstAuto},
	{"AnimateDeadBlowup", AnimateDeadBlowup},
	{"AnimateDeadMelt", AnimateDeadMelt},
	{"AnimateDeadBack", AnimateDeadBack},
	{"AnimateDeadForward", AnimateDeadForward},
	{"AnimateWeaponTakeOn", AnimateWeaponStand},
	{"AnimateWeaponStand", AnimateWeaponStand},
	{"AnimateWeaponTakeOff"},
	{"AnimateWeaponWalk", AnimateWeaponStand},
	{"AnimateWeaponDodge", AnimateWeaponStand},
	{"AnimateWeaponThrust", AnimateWeaponStand},
	{"AnimateWeaponSwing", AnimateWeaponStand},
	{"AnimateWeaponAim", AnimateWeaponSingle},
	{"AnimateWeaponSingle", AnimateWeaponAimEnd},
	{"AnimateWeaponBurst", AnimateWeaponAimEnd},
	{"AnimateWeaponFlame", AnimateWeaponAimEnd},
	{"AnimateWeaponThrow", AnimateWeaponStand},
	{"AnimateWeaponAimEnd", AnimateWeaponStand},
};
assert_enum(animationi, AnimateWeaponAimEnd)

static bool isweaponanimate(animate_s v) {
	return v >= FirstWeaponAnimate;
}

animate_s animable::getbase(animate_s v, int* w) {
	if(isweaponanimate(v)) {
		if(w)
			*w = (v - FirstWeaponAnimate) / 13;
		return animate_s(FirstWeaponAnimate + (v - FirstWeaponAnimate) % 13);
	}
	return v;
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

static bool ismoving(animate_s v) {
	switch(animable::getbase(v, 0)) {
	case AnimateWalk:
	case AnimateWeaponWalk:
	case AnimateRun:
		return true;
	default:
		return false;
	}
}

static void correctposition(drawable* pd, const sprite* ps, animate_s a) {
	if(ismoving(a)) {
		auto pt = anminfo::getoffset(ps, pd->frame);
		pd->position.x += pt.x;
		pd->position.y += pt.y;
	}
}

void animable::setanimate(animate_s v) {
	animate = v;
	auto ps = gres(naked);
	auto cicle = getframe(animate) + getframe(direction);
	auto pc = ps->gcicle(cicle);
	if(!pc || !pc->count)
		return;
	drawable::setanimate(pc->start, pc->count);
	correctposition(this, ps, animate);
}

void animable::appear(point h) {
	data = this;
	position = h2s(h);
	direction = RightDown;
	setanimate(AnimateStand);
}

void animable::focusing() const {
	auto pt = position;
	pt.x -= 640 / 2;
	pt.y -= 480 / 2;
	camera = pt;
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

void animable::nextanimate() {
	switch(animate) {
	case AnimateStand:
		timer += xrand(3, 7) * 1000;
		break;
	}
}

void animable::updateui() {
	const int fps = 150;
	static unsigned long last_tick;
	if(!last_tick)
		last_tick = current_tick;
	auto d = current_tick - last_tick;
	last_tick = current_tick;
	if(d > 1000) // Game resume from pause
		return;
	for(auto& e : bsdata<character>()) {
		if(!e)
			continue;
		e.timer -= d;
		if(e.timer > 0)
			continue;
		e.timer += fps;
		auto next_action = false;
		if(e.frame_start == e.frame_stop)
			e.timer += 1000; // Check every second
		else if(e.frame_start < e.frame_stop) {
			if(e.frame < e.frame_stop) {
				e.frame++;
				correctposition(&e, gres(e.naked), e.animate);
			} else {
				next_action = true;
				e.frame = e.frame_start;
				correctposition(&e, gres(e.naked), e.animate);
			}
		} else {
			if(e.frame > e.frame_stop)
				e.frame--;
			else {
				next_action = true;
				e.frame = e.frame_start;
			}
		}
		if(next_action)
			e.nextanimate();
	}
}