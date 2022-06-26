#include "dialog.h"
#include "draw.h"
#include "main.h"
#include "pathfind.h"

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

struct order {
	point				position;
	animable*			object;
	animate_s			animate;
	constexpr explicit operator bool() const { return object != 0; }
	void				clear() { memset(this, 0, sizeof(*this)); }
};
BSDATAC(order, 256)

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

static order* find_empthy() {
	for(auto& e : bsdata<order>()) {
		if(!e)
			return &e;
	}
	return 0;
}

void animable::addanimate(animate_s a, point pt) {
	if(isanimate(AnimateStand)) {
		setanimate(a);
		order_position = pt;
	} else {
		auto p = find_empthy();
		if(!p)
			p = bsdata<order>::add();
		p->position = pt;
		p->object = this;
		p->animate = a;
	}
}

bool is_special_animate() {
	return bsdata<order>::source.getcount() != 0;
}

void animable::clearallanimate() {
	for(auto& e : bsdata<order>()) {
		if(e.object == this) {
			e.clear();
			break;
		}
	}
	clearanimate();
}

static void clean_last_order() {
	auto pb = bsdata<order>::begin();
	auto p = bsdata<order>::end() - 1;
	while(p >= pb) {
		if(*p)
			break;
		p--;
	}
	bsdata<order>::source.count = p - pb + 1;
}

static indext dtot(indext i, int d) {
	return areai::tot(i, animable::getdirection(d));
}

void initialize_adventure() {
	pathfind::to = dtot;
	pathfind::maxcount = mps * mps;
	pathfind::maxdir = 6;
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

long distance(point p1, point p2);

static void correctposition(animable* pd, const sprite* ps) {
	if(pd->order_position) {
		auto pt = anminfo::getoffset(ps, pd->frame);
		pt.x += pd->position.x;
		pt.y += pd->position.y;
		auto n1 = distance(pt, pd->order_position);
		auto n2 = distance(pd->position, pd->order_position);
		if(n1 > n2 && n2 < 16) {
			pd->position = pd->order_position;
			pd->clearanimate();
		} else
			pd->position = pt;
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
	clearanimate();
}

void animable::setanimate(animate_s v, point target) {
	auto ps = gres(getlook());
	if(!ps)
		return;
	auto cicle = getframe(v, getweaponindex()) + getframe(direction);
	auto pc = ps->gcicle(cicle);
	if(pc && pc->count) {
		if(frame < pc->start || frame >= (pc->start + pc->count)) {
			order_position = target;
			frame = pc->start;
			frame_start = frame;
			frame_stop = frame_start + pc->count - 1;
			correctposition(this, ps);
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

direction_s animable::getdirection(int d) {
	switch(d) {
	case 0: return RightUp;
	case 1: return Right;
	case 2: return RightDown;
	case 3: return LeftDown;
	case 4: return Left;
	case 5: return LeftUp;
	default: return Center;
	}
}

void animable::clearanimate() {
	order_position.clear();
	setanimate(AnimateStand);
	timer += xrand(3, 7) * 1000;
}

static order* findorder(const animable* pv) {
	for(auto& e : bsdata<order>()) {
		if(e.object == pv)
			return &e;
	}
	return 0;
}

void animable::nextanimate() {
	auto po = findorder(this);
	if(po) {
		setanimate(po->animate, po->position);
		po->clear();
	} else
		clearanimate();
}

int	animable::getweaponindex() const {
	if(wear[RightHandItem])
		return wear[RightHandItem].geti().avatar.animation;
	return 0;
}

void animable::moveto(indext target) {
	static indext path[256 * 16];
	if(target == Blocked)
		return;
	auto start = h2i(s2h(position));
	pathfind::clearpath();
	pathfind::makewave(target);
	auto mp = pathfind::getpath(target, start, path, sizeof(path)/sizeof(path[0]));
	addanimate(AnimateWalk, h2s(i2h(target)));
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

void animable::updateframe() {
	auto ps = gres(getlook());
	if(frame_start == frame_stop)
		timer += 2000; // Dead body lying on ground check every two seconds
	else if(frame_start < frame_stop) {
		if(frame < frame_stop) {
			frame++;
			correctposition(this, ps);
		} else {
			frame = frame_start;
			correctposition(this, ps);
			nextanimate();
		}
	} else {
		if(frame > frame_stop)
			frame--;
		else {
			nextanimate();
			frame = frame_start;
		}
	}
}

bool animable::isanimate(animate_s v) const {
	auto ps = gres(getlook());
	if(!ps)
		return false;
	auto pc = ps->gcicle(getframe(v, getweaponindex()) + getframe(direction));
	if(!pc)
		return false;
	return frame >= pc->start && frame <= (pc->start + pc->count - 1);
}