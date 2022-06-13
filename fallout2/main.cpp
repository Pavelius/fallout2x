#include "answers.h"
#include "bsreq.h"
#include "dialog.h"
#include "main.h"

int start_application(fnevent proc, fnevent afterread);
void initialize_script();

static void start() {
	auto p1 = character::add("Narg");
	p1->open("CharacterGenerator");
}

static void initialize() {
	initialize_script();
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