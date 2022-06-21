#include "main.h"

BSDATA(actioni) = {
	{"NoAction", 252},
	{"Examine", 258},
	{"DropItem", 254},
	{"ReloadWeapon"},
	{"UseItem", 256},
	{"UseSkillOnItem"},
};
assert_enum(actioni, UseSkillOnItem)