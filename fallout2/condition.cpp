#include "bsreq.h"
#include "condition.h"

BSDATAC(conditioni, 128)
BSMETA(conditioni) = {
	BSREQ(id), BSREQ(param)
};

conditioni* conditioni::add(const char* id, fnproc proc, int param) {
	if(!proc)
		return 0;
	auto p = (conditioni*)bsdata<conditioni>::source.addfind(id);
	p->proc = proc;
	p->param = param;
	return p;
}