#include "main.h"

static order* find_empthy() {
	for(auto& e : bsdata<order>()) {
		if(!e)
			return &e;
	}
	return 0;
}

order* animable::addanimate(animate_s a, order* parent) {
	if(animate == AnimateStand) {
		setanimate(a);
		return 0;
	}
	auto p = find_empthy();
	if(!p)
		p = bsdata<order>::add();
	p->position.clear();
	p->object = this;
	p->animate = a;
	p->parent = parent;
	return p;
}

static void clean_parent() {
	for(auto& e : bsdata<order>()) {
		if(e.parent && !(*e.parent))
			e.parent = 0;
	}
}

static void clean_last() {
	auto pb = bsdata<order>::begin();
	auto p = bsdata<order>::end() - 1;
	while(p>=pb) {
		if(*p)
			break;
		p--;
	}
	bsdata<order>::source.count = p - pb + 1;
}

void order::update() {
	object->setanimate(animate);
	object->remove(WaitNewAnimation);
	clear();
}

void order::updateall() {
	for(auto& e : bsdata<order>()) {
		if(!e || e.parent || !e.object->animable::is(WaitNewAnimation))
			continue;
		e.update();
	}
	clean_parent();
	clean_last();
}