#include "bsreq.h"
#include "dialog.h"
#include "draw.h"
#include "main.h"
#include "pathfind.h"

extern const void* current_tool;

void add_locale_names(const char* id, bool required);
void add_locale_descriptions(const char* id);
long distance(point p1, point p2);
void initialize_adventure();
void initialize_script();
void initialize_dialog();
void main_util();
void editor();
void choose_scenery();
int start_application(fnevent proc, fnevent afterread);
void test_animate();

static int backward(int w) {
	switch(w) {
	case 0: return 3;
	case 1: return 4;
	case 2: return 5;
	case 3: return 0;
	case 4: return 1;
	default: return 2;
	}
}

static bool test_map() {
	for(short x = 0; x < mps * 2; x++) {
		for(short y = 0; y < mps * 2; y++) {
			auto i0 = h2i({x, y});
			for(auto i = 0; i < 6; i++) {
				auto i1 = pathfind::to(i0, i);
				if(i1 == Blocked)
					continue;
				auto i2 = pathfind::to(i1, backward(i));
				if(i0 != i2)
					return false;
			}
		}
	}
	return true;
}

static bool test_map2() {
	const auto m = 32;
	for(short x = 10; x < mps * 2 - 10; x++) {
		for(short y = 10; y < mps * 2 - 10; y++) {
			auto i0 = h2i({x, y});
			auto p0 = h2s({x, y});
			for(auto i = 0; i < 6; i++) {
				auto i1 = pathfind::to(i0, i);
				if(i1 == Blocked)
					return false;
				auto h1 = i2h(i1);
				auto p1 = h2s(h1);
				auto dx = p0.x - p1.x;
				auto dy = p0.y - p1.y;
				auto n1 = distance(p0, p1);
				if(n1 > m)
					return false;
			}
		}
	}
	return true;
}

static bool test_animation() {
	auto pa = anminfo::get(res::HMLTHR);
	auto& e1 = pa[AnimateWalk * 6];
	auto& e2 = pa[AnimateRun * 6];
	auto& e3 = pa[AnimateStand * 6];
	return true;
}

static void start() {
	status("Start game...");
	if(!test_animation())
		return;
	//if(!test_map2())
	//	return;
	auto is = sizeof(item);
	point t = {10, 6};
	point h = t2h(t);
	indext i = t2i(t);
	point t1 = i2t(i);
	indext i1 = loc.tot(i, Up);
	point t2 = i2t(i);
	character::last = character::add("Farmer");
	character::last->appear({35, 35});
	character::last = character::add("Narg");
	//choose_scenery();
	loc.set(0, 90, 100, 100, 4);
	//current_tool = bsdata<walli>::elements + 466;
	character::last->appear({28, 28});
	character::last->focusing();
	character::last->wear[BodyArmor] = "LeatherArmor";
	character::last->wear[LeftHandItem] = "Knife";
	character::last->wear[RightHandItem] = "N10mmPistol";
	character::last->wearable::additem("Crowbar");
	character::last->wearable::additem("Crowbar");
	character::last->wearable::additem("Spear");
	character::last->wearable::additem("N10mmSMG");
	character::last->wearable::additem("N5mmAP");
	character::last->wearable::additem("N5mmAP");
	character::last->wearable::additem("N5mmAP");
	character::last->wearable::additem("IceChest");
	character::last->wearable::additem("Dynamite");
	character::last->wearable::additem("StealthBoy");
	character::last->wearable::additem("BigBookOfScience");
	character::last->wearable::additem("N10mmAP");
	character::last->wearable::additem("N10mmAP");
	character::last->wearable::additem("Minigun");
	character::last->update();
	//draw::scene(editor);
	draw::opendialog("CharacterGame");
	//test_animate();
}

static void initialize() {
	status("Initialization...");
	initialize_adventure();
	initialize_script();
	initialize_dialog();
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Perks.txt");
	bsreq::read("rules/Prototype.txt");
	bsreq::read("rules/Terrain.txt");
	character::initialize();
	dialog::initialize();
#ifdef _DEBUG
	main_util();
#endif // _DEBUG
	add_locale_names("ItemNames", true);
	add_locale_descriptions("ItemDescriptions");
	add_locale_names("Tiles", true);
	add_locale_names("Scenery", true);
	add_locale_names("Walls", true);
	//bsreq::read("rules/Scenery.txt");
}

int main(int argc, char* argv[]) {
	return start_application(start, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}