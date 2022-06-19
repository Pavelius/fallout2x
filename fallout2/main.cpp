#include "bsreq.h"
#include "dialog.h"
#include "draw.h"
#include "main.h"

extern const void* current_tool;

void add_locale_names(const char* id, bool required);
void initialize_adventure();
void initialize_script();
void initialize_dialog();
void character_generate();
void main_util();
void editor();
void choose_scenery();
int start_application(fnevent proc, fnevent afterread);

static void start() {
	character::last = character::add("Narg");
	//choose_scenery();
	loc.set(0, 30, 100, 100, 9);
	current_tool = bsdata<walli>::elements + 466;
	draw::scene(editor);
	//character_generate();
}

static void initialize() {
	initialize_adventure();
	initialize_script();
	initialize_dialog();
	bsreq::read("rules/Perks.txt");
	bsreq::read("rules/Prototype.txt");
	character::initialize();
	dialog::initialize();
	main_util();
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