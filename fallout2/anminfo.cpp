#include "draw.h"
#include "main.h"

const anminfo* anminfo::get(const sprite* ps) {
	if(!ps)
		return 0;
	return (anminfo*)ps->ptr(ps->cicles_offset + ps->cicles * sizeof(sprite::cicle));
}

const point anminfo::getoffset(const sprite* ps, int frame) {
	if(!ps)
		return {};
	return ((point*)ps->ptr(ps->cicles_offset + ps->cicles * sizeof(sprite::cicle) + sizeof(anminfo) * LastAnimation))[frame];
}