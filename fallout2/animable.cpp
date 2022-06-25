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
	{"AnimateWeaponTakeOn"},
	{"AnimateWeaponStand"},
	{"AnimateWeaponTakeOff"},
	{"AnimateWeaponWalk"},
	{"AnimateWeaponDodge"},
	{"AnimateWeaponThrust"},
	{"AnimateWeaponSwing"},
	{"AnimateWeaponAim", AnimateWeaponSingle},
	{"AnimateWeaponSingle", AnimateWeaponAimEnd},
	{"AnimateWeaponBurst", AnimateWeaponAimEnd},
	{"AnimateWeaponFlame", AnimateWeaponAimEnd},
	{"AnimateWeaponThrow"},
	{"AnimateWeaponAimEnd"},
};
assert_enum(animationi, AnimateWeaponAimEnd)

static bool need_stop;

point t2s(point v) {
	return {
		(short)(32 * v.y + 48 * v.x),
		(short)(24 * v.y - 12 * v.x)
	};
}

point s2t(point pt) {
	int x = pt.x + 40;
	int y = pt.y + 26;
	return{(short)((x - 4 * y / 3) / 64), (short)((x + 4 * y) / 128)};
}

point h2s(point v) {
	return{
		(short)(16 * (v.y + v.x * 2 - v.x / 2)),
		(short)(12 * (v.y - v.x / 2))
	};
}

point s2h(point pt) {
	int x1 = pt.x + 15;
	int y1 = pt.y + 7;
	int nx;
	if(x1 < 0)
		nx = (x1 + 1) / 16 - 1;
	else
		nx = x1 / 16;
	int ny;
	if(y1 < 0)
		ny = (y1 + 1) / 12 - 1;
	else
		ny = y1 / 12;
	if(iabs(nx) % 2 != iabs(ny) % 2)
		nx--;
	long xhBase = 16 * nx;
	long yhBase = 12 * ny;
	int x = (4 * yhBase - 3 * xhBase) / 96;
	int y = yhBase / 12 - x / 2;
	return{(short)-x, (short)y};
}

static void paint_drawable(const drawable* p) {
	if(bsdata<sceneryi>::have(p->data))
		((sceneryi*)p->data)->paint();
	else if(bsdata<walli>::have(p->data))
		((walli*)p->data)->paint();
	else if(bsdata<character>::have(p->data))
		((character*)p->data)->paint();
}

static int getorder(const drawable* p) {
	if(bsdata<sceneryi>::have(p->data))
		return ((sceneryi*)p->data)->is(Flat) ? 0 : 1;
	return 1;
}

static drawable** select_drawables(drawable** ps, drawable* const* pe) {
	for(auto& e : bsdata<character>()) {
		if(ps < pe)
			*ps++ = &e;
	}
	return ps;
}

void initialize_adventure() {
	drawable::paint = paint_drawable;
	drawable::getorder = getorder;
	drawable::select = select_drawables;
}

direction_s animable::getdirection(point s, point d) {
	static const direction_s orientations[25] = {
		LeftUp, LeftUp, RightUp, RightUp, RightUp,
		LeftUp, LeftUp, LeftUp, RightUp, RightUp,
		Left, Left, Center, Right, Right,
		LeftDown, LeftDown, RightDown, RightDown, RightDown,
		LeftDown, LeftDown, LeftDown, RightDown, RightDown
	};
	const int osize = 5;
	int dx = d.x - s.x;
	int dy = d.y - s.y;
	int st = (2 * imax(iabs(dx), iabs(dy)) + osize - 1) / osize;
	if(!st)
		return Center;
	int ax = dx / st;
	int ay = dy / st;
	return orientations[(ay + (osize / 2)) * osize + ax + (osize / 2)];
}

static void marker() {
	auto push_caret = caret;
	caret.x -= 4; line(caret.x + 8, caret.y);
	caret = push_caret;
	caret.y -= 4; line(caret.x, caret.y + 8);
	caret = push_caret;
}

void sceneryi::paint() const {
	auto rs = gres(res::SCENERY);
	image(rs, rs->ganim(frame, current_tick / 200), 0);
}

void walli::paint() const {
	auto rs = gres(res::WALLS);
	image(rs, rs->ganim(frame, current_tick / 200), 0);
}

void animable::paint() const {
	auto rs = gres(getlook());
	if(!rs)
		return;
	image(rs, frame, 0);
}

void tilei::paint() const {
	auto rs = gres(res::TILES);
	image(rs, rs->ganim(frame, current_tick / 200), 0);
}

static bool isweaponanimate(animate_s v) {
	return v >= FirstWeaponAnimate;
}

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
			*w = 1 + (v - FirstWeaponAnimate) / 13;
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

void animable::turn(int d) {
	unsigned w = (int)direction + d;
	direction = direction_s(w % 6);
	setanimate(animate);
}

void animable::setanimate(animate_s v) {
	animate = v;
	auto ps = gres(getlook());
	if(!ps)
		return;
	auto cicle = getframe(animate, getweaponindex()) + getframe(direction);
	auto pc = ps->gcicle(cicle);
	if(pc && pc->count) {
		if(frame < pc->start || frame >= (pc->start + pc->count)) {
			drawable::setanimate(pc->start, pc->count);
			correctposition(this, ps, animate);
		}
	}
	timer = getdelay();
	remove(WaitNewAnimation);
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
	else
		clearanimate();
}

int	animable::getweaponindex() const {
	if(wear[RightHandItem])
		return wear[RightHandItem].geti().avatar.animation;
	return 0;
}

void animable::updateframe() {
	auto ps = gres(getlook());
	if(frame_start == frame_stop)
		timer += 2000; // Dead body lying on ground check every two seconds
	else if(frame_start < frame_stop) {
		if(frame < frame_stop) {
			frame++;
			correctposition(this, ps, animate);
		} else {
			flags |= FG(WaitNewAnimation);
			frame = frame_start;
			correctposition(this, ps, animate);
		}
	} else {
		if(frame > frame_stop)
			frame--;
		else {
			flags |= FG(WaitNewAnimation);
			frame = frame_start;
		}
	}
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