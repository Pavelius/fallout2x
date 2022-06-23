#include "dialog.h"
#include "main.h"

const int skill_tag_bonus = 20;
character* character::last;

static void apply_skill_tag() {
	for(auto v : character::skill_list) {
		if(character::last->istagged(v))
			character::last->stats[v] += skill_tag_bonus;
	}
}

void character::apply_equipment() {
	stats[AC] += wear[BodyArmor].geti().armor.ac;
}

static bool update_minmax() {
	auto p = character::last;
	auto need_update = false;
	for(auto& e : bsdata<stati>()) {
		auto i = getbsi(&e);
		if(e.maximum > 0 && p->stats[i] > e.maximum) {
			auto d = p->stats[i] - e.maximum;
			if(p->basic.stats[i] >= d) {
				p->basic.stats[i] -= d;
				need_update = true;
			}
		} else if(e.minimum > 0 && p->stats[i] < e.minimum) {
			p->basic.stats[i] += e.minimum - p->stats[i];
			need_update = true;
		}
	}
	return need_update;
}

static void update_perks(int order) {
	for(auto& e : bsdata<perki>()) {
		if(e.order != order)
			continue;
		auto i = getbsi(&e);
		if(!character::last->perks.is(i))
			continue;
		script::run(e.use);
	}
}

void character::update() {
	auto push_character = character::last;
	character::last = this;
	auto need_update = true;
	while(need_update) {
		load(basic);
		update_perks(0);
		for(auto& e : bsdata<stati>()) {
			if(!e.formula)
				continue;
			last_value = 0;
			script::run(e.formula);
			stats[getbsi(&e)] += last_value;
		}
		update_perks(1);
		need_update = update_minmax();
	}
	apply_skill_tag();
	apply_equipment();
	character::last = push_character;
	setanimate(animate);
}

character* character::add(const char* id) {
	auto pt = bsdata<prototype>::find(id);
	if(!pt)
		return 0;
	auto p = bsdata<character>::add();
	p->id = pt->id;
	p->load(*pt);
	memcpy(&p->perks, &pt->perks, sizeof(pt->perks));
	p->basic.load(*pt);
	p->basic.stats[SkillTagPoints] = 3;
	for(auto pa : pt->tags) {
		if(pa)
			p->settag((stat_s)getbsi(pa), 1);
	}
	p->naked = res::HMJMPS;
	p->update();
	return p;
}

bool character::istagged(valuet v) const {
	if(v >= first_skill)
		return tags.is(v - first_skill);
	return false;
}

void character::settag(valuet v, int i) {
	if(v >= first_skill)
		tags.set(v - first_skill, i != 0);
}

int character::gettaggedskills() const {
	auto result = 0;
	for(auto v : skill_list) {
		if(istagged(v))
			result += 1;
	}
	return result;
}

int	character::getperkstotal() const {
	return perks.getcount();
}

static void preparelist(character::list& result, character::valuet& first, const char* id) {
	auto p = bsdata<list>::find(id);
	if(!p)
		return;
	auto count = p->elements.size();
	auto data = new short unsigned[count];
	auto index = 0;
	for(size_t i = 0; i < count; i++) {
		data[i] = p->elements.begin()[i].value;
		if(i == 0)
			first = data[0];
	}
	result = character::list(data, count);
}

static character::valuet first_stat;

void character::initialize() {
	preparelist(skill_list, first_skill, "Skills");
	preparelist(primary_list, first_stat, "PrimaryStats");
}