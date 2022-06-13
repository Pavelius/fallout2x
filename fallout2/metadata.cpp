#include "bsreq.h"
#include "condition.h"
#include "control.h"
#include "function.h"
#include "script.h"
#include "main.h"

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

NOBSDATA(point)

BSDATAC(character, 256)
BSDATAC(control, 2048)
BSDATAC(drawable, 4096*8)
BSDATAC(perki, 256)
BSDATAC(prototype, 32)
BSDATAC(list, 128)
BSDATAC(stati, 64)
BSDATAD(variant)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSMETA(control) = {
	BSREQ(type), BSREQ(x), BSREQ(y),
	BSREQ(size), BSREQ(data),
	BSREQ(text), BSREQ(command), BSREQ(format),
	BSREQ(normal), BSREQ(disabled), BSREQ(pressed),
	{}};
BSMETA(decorator) = {
	BSREQ(id),
	{}};
BSMETA(list) = {
	BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(perki) = {
	BSREQ(id),
	BSREQ(use),
	{}};
BSMETA(point) = {
	BSREQ(x),
	BSREQ(y),
	{}};
BSMETA(prototype) = {
	BSREQ(id),
	BSDST(stats, stati),
	{}};
BSMETA(stati) = {
	BSREQ(id),
	BSREQ(formula),
	BSREQ(minimum), BSREQ(maximum),
	{}};
BSMETA(gradei) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"Condition", VRSTD(conditioni), 1},
	{"Control", VRSTD(control), 3},
	{"Function", VRSTD(function), 1},
	{"Grade", VRSTD(gradei), 1},
	{"List", VRSTD(list), 1},
	{"Perk", VRSTD(perki), 1},
	{"Prototype", VRSTD(prototype), 1},
	{"Resource", VRSTD(residi), 1},
	{"Script", VRSTD(script), 1},
	{"Stat", VRSTD(stati), 1},
};
BSDATAF(varianti)