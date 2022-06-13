#include "bsreq.h"
#include "widget.h"

BSMETA(widget) = {
	BSREQ(id),
	{}};
const widget* widget::last;

void widget::paint() const {
	auto push_last = last;
	last = this; last->proc();
	last = push_last;
}