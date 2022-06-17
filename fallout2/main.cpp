#include "bsreq.h"
#include "dialog.h"
#include "draw.h"
#include "main.h"

void add_locale_names(const char* id, bool required);
void initialize_adventure();
void initialize_script();
void initialize_dialog();
void character_generate();
void main_util();
void adventure();
void choose_scenery();
int start_application(fnevent proc, fnevent afterread);

static void start() {
	character::last = character::add("Narg");
	//choose_scenery();
	draw::scene(adventure);
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
	add_locale_names("Scenery", true);
	//bsreq::read("rules/Scenery.txt");
}

int main(int argc, char* argv[]) {
	return start_application(start, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}