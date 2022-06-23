#include "bsreq.h"
#include "dialog.h"
#include "draw.h"
#include "main.h"

extern const void* current_tool;

void add_locale_names(const char* id, bool required);
void add_locale_descriptions(const char* id);
void initialize_adventure();
void initialize_script();
void initialize_dialog();
void main_util();
void editor();
void choose_scenery();
int start_application(fnevent proc, fnevent afterread);

static void start() {
	auto is = sizeof(item);
	point t = {10, 6};
	point h = t2h(t);
	indext i = t2i(t);
	point t1 = i2t(i);
	indext i1 = loc.tot(i, Up);
	point t2 = i2t(i);
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
	character::last->wearable::additem("N5mmAP");
	character::last->wearable::additem("N5mmAP");
	character::last->wearable::additem("N5mmAP");
	character::last->wearable::additem("IceChest");
	character::last->wearable::additem("Dynamite");
	character::last->wearable::additem("StealthBoy");
	character::last->wearable::additem("BigBookOfScience");
	character::last->wearable::additem("N10mmAP");
	character::last->wearable::additem("N10mmAP");
	character::last->update();
	//draw::scene(editor);
	//draw::scene(adventure);
	draw::opendialog("CharacterGame");
}

static void initialize() {
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