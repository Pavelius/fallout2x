#include "bsreq.h"
#include "condition.h"
#include "control.h"
#include "function.h"
#include "script.h"
#include "main.h"

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

NOBSDATA(itemi::ammoi)
NOBSDATA(itemi::armori)
NOBSDATA(itemi::imagei)
NOBSDATA(itemi::weaponi)
NOBSDATA(point)

BSDATAC(character, 256)
BSDATAC(control, 2048)
BSDATAC(drawable, 4096*8)
BSDATAC(globalstati, 128)
BSDATAC(itemi, 2048)
BSDATAC(itemwear, 4096)
BSDATAC(itemground, 4096)
BSDATAC(list, 128)
BSDATAC(perki, 256)
BSDATAC(prototype, 32)
BSDATAC(terrain, 16)
BSDATAD(variant)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(chat) = {
	BSREQ(id),
	{}};
BSMETA(control) = {
	BSREQ(type), BSREQ(x), BSREQ(y),
	BSREQ(size), BSREQ(data),
	BSREQ(text), BSREQ(command), BSREQ(format),
	BSREQ(normal), BSREQ(pressed), BSREQ(lines),
	{}};
BSMETA(damagei) = {
	BSREQ(id),
	{}};
BSMETA(directioni) = {
	BSREQ(id),
	{}};
BSMETA(decorator) = {
	BSREQ(id),
	{}};
BSMETA(globalstati) = {
	BSREQ(id),
	BSREQ(value),
	{}};
BSMETA(gradei) = {
	BSREQ(id),
	{}};
BSMETA(itemi::ammoi) = {
	BSREQ(ammo),
	BSREQ(count),
	BSREQ(ac), BSREQ(dr), BSREQ(dam_bonus),
	{}};
BSMETA(itemi::imagei) = {
	BSREQ(inventory),
	BSREQ(ground),
	BSREQ(animation),
	{}};
BSMETA(itemi::weaponi) = {
	BSREQ(min), BSREQ(max),
	BSENM(mode1, actioni),
	BSENM(mode2, actioni),
	BSREQ(strenght),
	BSENM(type, damagei),
	BSREQ(ap), BSREQ(range),
	BSREQ(ammo), BSREQ(ammo_count),
	BSREQ(burst), BSREQ(critical_fail),
	{}};
BSMETA(itemi::armori) = {
	BSREQ(ac),
	BSENM(male, residi), BSENM(female, residi),
	BSREQ(threshold), BSREQ(resistance),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(size), BSREQ(weight), BSREQ(cost),
	BSENM(material, materiali),
	BSREQ(avatar),
	BSREQ(weapon),
	BSREQ(armor),
	BSREQ(ammo),
	BSENM(use, stati),
	BSFLG(flags, itemfi),
	//BSFLG(ground_flags, objectfi),
	{}};
BSMETA(itemfi) = {
	BSREQ(id),
	{}};
BSMETA(list) = {
	BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(materiali) = {
	BSREQ(id),
	{}};
BSMETA(modifier) = {
	BSREQ(id),
	{}};
BSMETA(objectfi) = {
	BSREQ(id),
	{}};
BSMETA(perki) = {
	BSREQ(id),
	BSREQ(use),
	BSREQ(avatar), BSREQ(order),
	{}};
BSMETA(point) = {
	BSREQ(x),
	BSREQ(y),
	{}};
BSMETA(prototype) = {
	BSREQ(id),
	BSDST(stats, stati),
	BSREQ(tags),
	BSFLG(perks, perki),
	BSENM(naked, residi),
	BSREQ(chat),
	{}};
BSMETA(sceneryi) = {
	BSREQ(id),
	{}};
BSMETA(stati) = {
	BSREQ(id),
	BSREQ(formula), BSREQ(avatar),
	BSREQ(minimum), BSREQ(maximum),
	{}};
BSMETA(terrain) = {
	BSREQ(id),
	BSDST(blocks, directioni),
	BSREQ(alternate),
	{}};
BSMETA(tilei) = {
	BSREQ(id),
	{}};
BSMETA(walli) = {
	BSREQ(id),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Chat", VRSTD(chat), 1},
	{"Condition", VRSTD(conditioni), 1},
	{"Control", VRSTD(control), 3},
	{"Function", VRSTD(function), 1},
	{"Global", VRSTD(globalstati), 1},
	{"Grade", VRSTD(gradei), 1},
	{"Item", VRSTD(itemi), 1},
	{"List", VRSTD(list), 1},
	{"Modifier", VRSTD(modifier), 1},
	{"Perk", VRSTD(perki), 1},
	{"Prototype", VRSTD(prototype), 1},
	{"Resource", VRSTD(residi), 1},
	{"Scenery", VRSTD(sceneryi)},
	{"Script", VRSTD(script), 1},
	{"Stat", VRSTD(stati), 1},
	{"Terrain", VRSTD(terrain), 1},
	{"Tile", VRSTD(tilei)},
	{"Wall", VRSTD(walli)},
	{"Wear", VRSTD(weari), 1},
};
BSDATAF(varianti)