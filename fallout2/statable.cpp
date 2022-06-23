#include "main.h"

statable::list statable::primary_list;
statable::list statable::skill_list;
statable::valuet statable::first_skill;

static void copyvalue(statable& e1, const statable& e2) {
	e1 = e2;
}

void statable::load(const statable& v) {
	copyvalue(*this, v);
}

int	statable::total(const statable::list& source) const {
	auto result = 0;
	for(auto v : source)
		result += stats[v];
	return result;
}

int	statable::getplayerstats() const {
	return 40 - total(primary_list);
}

int	statable::getactionpoints(action_s v, const item& it) const {
	auto r = it.geti().weapon.ap;
	if(!r)
		r = 3;
	auto& ai = bsdata<actioni>::elements[v];
	if(ai.is(Aimed))
		r++;
	if(v == FireBurst)
		r++;
	return r;
}