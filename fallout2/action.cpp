#include "main.h"

BSDATA(actioni) = {
	{"NoAction", 252},
	{"ThrowPunch"},
	{"KickLeg"},
	{"Swing"},
	{"Thrust"},
	{"Throw"},
	{"FireSingle"},
	{"FireBurst"},
	{"Reload"},
	{"Drop", 254},
	{"Look", 258},
	{"Talk"},
	{"Turn"},
	{"Unload"},
	{"Use", 256},
	{"UseOnObject"},
	{"UseSkill"},
};
assert_enum(actioni, UseSkill)