#include "dialog.h"
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
	{"AnimateKnockOutBack", AnimateDeadBackNoBlood},
	{"AnimateKnockOutForward", AnimateDeadForwardNoBlood},
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

static bool need_stop;

static bool isweaponanimate(animate_s v) {
	return v >= FirstWeaponAnimate;
}

static animable* last_wait;

res	animable::getlook() const {
	if(wear[BodyArmor]) {
		auto& ei = wear[BodyArmor].geti();
		return iswoman() ? ei.armor.female : ei.armor.male;
	}
	return naked;
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

int animable::getdelay() const {
	auto pi = anminfo::get(getlook());
	if(pi && pi->fps)
		return 1000 / pi->fps;
	return 1000 / 10;
}

void animable::setanimate(animate_s v) {
	animate = v;
	auto ps = gres(getlook());
	if(!ps)
		return;
	auto cicle = getframe(animate, getweaponindex()) + getframe(direction);
	auto pc = ps->gcicle(cicle);
	if(pc && pc->count) {
		if(frame < pc->start || frame >(pc->start + pc->count)) {
			drawable::setanimate(pc->start, pc->count);
			correctposition(this, ps, animate);
		}
	}
	timer = getdelay();
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

void animable::clearanimate() {
	setanimate(AnimateStand);
	timer += xrand(3, 7) * 1000;
}

void animable::nextanimate() {
	auto& ei = bsdata<animationi>::elements[animate];
	if(ei.next)
		setanimate(ei.next);
	else if(animate == AnimateStand)
		clearanimate();
}

int	animable::getweaponindex() const {
	if(wear[RightHandItem])
		return wear[RightHandItem].geti().avatar.animation;
	return 0;
}

void animable::updateframe() {
	auto next_action = false;
	auto ps = gres(getlook());
	if(frame_start == frame_stop)
		timer += 1000;
	else if(frame_start < frame_stop) {
		if(frame < frame_stop) {
			frame++;
			correctposition(this, ps, animate);
		} else {
			next_action = true;
			frame = frame_start;
			correctposition(this, ps, animate);
		}
	} else {
		if(frame > frame_stop)
			frame--;
		else {
			next_action = true;
			frame = frame_start;
		}
	}
	if(next_action) {
		nextanimate();
		if(last_wait == this)
			need_stop = true;
	}
}

void update_animation() {
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
		e.timer += e.getdelay();
		e.updateframe();
	}
}

void paint_animation();

void animable::wait() {
	auto push_last = last_wait;
	last_wait = this;
	need_stop = false;
	while(!need_stop && ismodal()) {
		height = 381;
		paint_animation();
		cursor.position = hot.mouse;
		cursor.set(res::INTRFACE, 295);
		cursor.paint();
		doredraw();
		update_animation();
		waitcputime(1);
	}
	last_wait = push_last;
}

void animable::changeweapon() {
	setanimate(AnimateWeaponTakeOff);
	wait();
	iswap(wear[LeftHandItem], wear[RightHandItem]);
	iswap(action_index[0], action_index[1]);
	setanimate(AnimateWeaponTakeOn);
	wait();
	clearanimate();
}