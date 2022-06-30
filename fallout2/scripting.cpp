#include "main.h"
#include "condition.h"
#include "script.h"

int last_value, last_stat, difficult;

static void clear_value(int bonus, int param) {
	last_value = 0;
}

static void add_value(int bonus, int param) {
	last_value += bonus;
}

static void set_value(int bonus, int param) {
	last_value += bonus;
}

static void divide_value(int bonus, int param) {
	if(bonus)
		last_value = last_value / bonus;
}

static void multiply_value(int bonus, int param) {
	last_value = last_value * bonus;
}

static void minimum_value(int bonus, int param) {
	if(last_value < bonus)
		last_value = bonus;
}

static int getbonus(int i, int ci) {
	switch(i) {
	case 100: return last_value;
	case 101: return ci;
	case -100: return -last_value;
	case -101: return -ci;
	default: return i;
	}
}

static void set_experience(int bonus, int param) {
	last_value = character::last->experience;
}

static void next_level(int bonus, int param) {
	last_value = 1000;
}

static bool is_random(int bonud, int param) {
	return d100() < 30;
}

void chat::play(const speech* p) {
	while(p && p->text) {
		answers an;
		auto pe = elements.end();
		for(auto p1 = p + 1; p1 < pe && p->index == p1->index; p1++)
			an.add(p1, p1->text);
		p = (speech*)an.choose(an.message(p->text), 0);
		if(p) {
			p = find(p->next);
		}
	}
}

bool speech::isallow() const {
	difficult = 1;
	for(auto v : tags) {
		if(v.iskind<conditioni>())
			return bsdata<conditioni>::elements[v.value].proc(v.counter,
				bsdata<conditioni>::elements[v.value].param);
		else if(v.iskind<modifier>())
			bsdata<modifier>::elements[v.value].variable = bsdata<modifier>::elements[v.value].value;
		else
			return true;
	}
	return true;
}

static void script_run(variant v) {
	difficult = 1;
	if(v.iskind<stati>()) {
		last_stat = v.value;
		if(!v.counter)
			last_value += character::last->stats[v.value];
		else
			character::last->stats[v.value] += getbonus(v.counter, character::last->stats[v.value]);
	} else if(v.iskind<modifier>())
		bsdata<modifier>::elements[v.value].variable = bsdata<modifier>::elements[v.value].value;
	else {

	}
}

static void add_stat(int bonus, int param) {
}

void initialize_script() {
	script::prun = script_run;
}

BSDATA(script) = {
	{"AddValue", add_value},
	{"Divide", divide_value},
	{"Experience", set_experience},
	{"Minimum", minimum_value},
	{"Multiply", multiply_value},
	{"NextLevel", next_level},
	{"Value", set_value},
};
BSDATAF(script)
BSDATA(modifier) = {
	{"Low", difficult, 0},
	{"Normal", difficult, 1},
	{"Hight", difficult, 2},
};
BSDATAF(modifier)
BSDATA(conditioni) = {
	{"Random", is_random},
};
BSDATAF(conditioni)