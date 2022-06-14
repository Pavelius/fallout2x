#include "main.h"

BSDATAC(drawable, 2048)
static adat<drawable*, 1024> drawables;

static void prepare_drawables() {
}

static void cleanup_drawables() {
}

static void paint_drawables() {
	for(auto p : drawables)
		p->paint();
}

void drawable::paint() const {

}