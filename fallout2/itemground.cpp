#include "draw.h"
#include "main.h"

static void update_itemground() {
	for(auto& e : bsdata<itemground>()) {
		auto p = drawable::find(&e);
		if(e) {
			if(!p) {
				auto pt = h2s(i2h(e.index));
				drawable::add(pt, &e);
			}
		} else
			p->clear();
	}
}

static void copy(item& i1, item& i2) {
	i1 = i2;
}

void item::dropdown(indext index) {
	auto p = bsdata<itemground>::add();
	copy(*p, *this);
	clear();
	p->index = index;
	update_itemground();
}

void itemground::paint() const {
	auto pr = gres(res::ITEMS);
	draw::image(pr, pr->ganim(geti().avatar.ground, 0), 0);
}