#include "main.h"
#include "script.h"

int last_value, last_stat;

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

static void script_run(variant v) {
	if(v.iskind<stati>()) {
		last_stat = v.value;
		if(!v.counter)
			last_value += character::last->stats[v.value];
		else
			character::last->stats[v.value] += getbonus(v.counter, character::last->stats[v.value]);
	} else {

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