#include "bsreq.h"
#include "dialog.h"
#include "main.h"

int start_application(fnevent proc, fnevent afterread);
void initialize_script();
void initialize_dialog();
void character_generate();
void main_util();
void choose_scenery();

static void start() {
	main_util();
	character::last = character::add("Narg");
	choose_scenery();
	//character_generate();
}

static void initialize() {
	initialize_script();
	initialize_dialog();
	bsreq::read("rules/Perks.txt");
	bsreq::read("rules/Prototype.txt");
	character::initialize();
	dialog::initialize();
}

int main(int argc, char* argv[]) {
	return start_application(start, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}