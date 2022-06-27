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
	if(isanimate(AnimateStand))
		setanimate(a);
	else {
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

void moveable::clearpath() {
	if(path_start)
		delete[] path_start;
	path_start = path = 0;
}

void moveable::makepath(indext start, indext goal) {
	static indext temp[256 * 16];
	clearpath();
	pathfind::clearpath();
	pathfind::makewave(goal);
	auto count = pathfind::getpath(start, goal, temp, sizeof(temp) / sizeof(temp[0]));
	path_start = new indext[count + 1];
	memcpy(path_start, temp, sizeof(temp[0]) * count);
	path_start[count] = Blocked;
	path = path_start;
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

static point getdirection(point hex, int direction) {
	static point evenr_directions[2][6] = {
		{{1, -1}, {1, 0}, {0, 1}, {-1, 0}, {-1, -1}, {0, -1}},
		{{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {0, -1}},
	};
	auto parity = hex.x & 1;
	auto offset = evenr_directions[parity][direction];
	return hex + offset;
}

static indext getdirection(indext index, int direction) {
	if(index == Blocked)
		return Blocked;
	auto hex = getdirection(i2h(index), direction);
	if(hex.x < 0 || hex.y < 0 || hex.x >= mps * 2 || hex.y >= mps * 2)
		return Blocked;
	return h2i(hex);
}

void initialize_adventure() {
	pathfind::to = getdirection;
	pathfind::maxcount = mps * mps;
	pathfind::maxdir = 6;
	drawable::paint = paint_drawable;
	drawable::getorder = getorder;
	drawable::select = select_drawables;
}

hexdir_s animable::getdirection(point s, point d) {
	static const hexdir_s orientations[25] = {
		HexLU, HexLU, HexRU, HexRU, HexRU,
		HexLU, HexLU, HexLU, HexRU, HexRU,
		HexL, HexL, HexRD, HexR, HexR,
		HexLD, HexLD, HexRD, HexRD, HexRD,
		HexLD, HexLD, HexLD, HexRD, HexRD
	};
	const int osize = 5;
	int dx = d.x - s.x;
	int dy = d.y - s.y;
	int st = (2 * imax(iabs(dx), iabs(dy)) + osize - 1) / osize;
	if(!st)
		return HexRD;
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

long distance(point p1, point p2);

static hexdir_s getnextdirection(indext start, indext goal) {
	for(auto i = 0; i < pathfind::maxdir; i++) {
		if(pathfind::to(start, i) == goal)
			return (hexdir_s)i;
	}
	return HexRD;
}

static void correctposition(animable* pd, const sprite* ps) {
	if(pd->ismoving()) {
		auto pt = anminfo::getoffset(ps, pd->frame);
		auto prev_position = pd->position;
		pd->position.x += pt.x;
		pd->position.y += pt.y;
		auto next_position = h2s(i2h(pd->path[0]));
		auto change_index = (prev_position.x < next_position.x && next_position.x <= pd->position.x)
			|| (prev_position.x > next_position.x && next_position.x >= pd->position.x)
			|| (prev_position.y < next_position.y && next_position.y <= pd->position.y)
			|| (prev_position.y > next_position.y && next_position.y >= pd->position.y);
		if(change_index) {
			pd->path++;
			if(pd->path[0] == Blocked)
				pd->clearanimate();
			else {
				auto new_direction = getnextdirection(pd->path[-1], pd->path[0]);
				if(new_direction != pd->direction) {
					pd->direction = new_direction;
					pd->position = next_position;
					pd->setanimate(AnimateWalk);
				}
			}
		}
	}
}

int animable::getdelay() const {
	auto pi = anminfo::get(getlook());
	if(pi && pi->fps)
		return 1000 / pi->fps;
	return 1000 / 10;
}

void animable::turn(int d) {
	auto w = getframe(direction) + d;
	if(w < 0)
		w += 6;
	direction = hexdir_s(w % 6);
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
	direction = HexRD;
	setanimate(AnimateStand);
}

void animable::focusing() const {
	auto pt = position;
	pt.x -= 640 / 2;
	pt.y -= 480 / 2;
	camera = pt;
}

void animable::clearanimate() {
	clearpath();
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
	if(ismoving())
		return;
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

void animable::moveto(indext goal) {
	if(goal == Blocked)
		return;
	auto start = h2i(s2h(position));
	makepath(start, goal);
	direction = getnextdirection(start, path_start[0]);
	setanimate(AnimateWalk);
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

void animable::addanimate(action_s v) {
	switch(v) {
	case FireSingle:
	case FireSingleAimed:
		addanimate(AnimateWeaponAim);
		addanimate(AnimateWeaponSingle);
		addanimate(AnimateWeaponAimEnd);
		break;
	case FireBurst:
		addanimate(AnimateWeaponAim);
		addanimate(AnimateWeaponBurst);
		addanimate(AnimateWeaponAimEnd);
		break;
	case Swing:
	case SwingAimed:
		addanimate(AnimateWeaponSwing);
		break;
	case Thrust:
	case ThrustAimed:
		addanimate(AnimateWeaponThrust);
		break;
	default:
		break;
	}
}