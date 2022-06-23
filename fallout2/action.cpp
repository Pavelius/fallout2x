#include "main.h"

BSDATA(actioni) = {
	{"NoAction", 252},
	{"ThrowPunch", 42},
	{"ThrowPunchAimed", 42, ThrowPunch, FG(Aimed)},
	{"KickLeg", 41},
	{"Swing", 44},
	{"SwingAimed", 44, Swing, FG(Aimed)},
	{"Thrust", 45},
	{"ThrustAimed", 45, Thrust, FG(Aimed)},
	{"Throw", 117},
	{"FireSingle", 43},
	{"FireSingleAimed", 43, FireSingle, FG(Aimed)},
	{"FireBurst", 40},
	{"Reload"},
	{"Drop", 254},
	{"Look", 258},
	{"Talk", 262},
	{"Turn", 260},
	{"Unload", 301},
	{"Use", 264},
	{"UseOnObject", 256},
	{"UseSkill", 303},
};
assert_enum(actioni, UseSkill)