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

//static void answer_button(int x, int& y, const char* title, int id, unsigned key) {
//	rect rc;
//	if(buttonp(x, y, 221, 222, title, key, &rc))
//		execute(buttonparam, id);
//	y += rc.height() + 2;
//}
//
//int answers::choose() const {
//	screenshoot screen;
//	openform();
//	while(ismodal()) {
//		screen.restore();
//		int x, y, sx, sy;
//		window(x, y, sx, sy, 220);
//		x += 14; y += 16;
//		auto index = 0;
//		for(auto& e : elements) {
//			if(index > 5)
//				break;
//			answer_button(x, y, e.text, e.id, '1' + index);
//			index++;
//		}
//		if(index < 5)
//			answer_button(x, y, variant(Cancel), 0, KeyEscape);
//		domodal();
//	}
//	closeform();
//	return getresult();
//}