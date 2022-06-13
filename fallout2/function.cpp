#include "bsreq.h"
#include "function.h"

BSMETA(function) = {
	BSREQ(id),
	BSREQ(script),
	{}};
BSDATAC(function, 512)

function* function::last;