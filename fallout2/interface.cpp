#include "bsreq.h"
#include "draw.h"
#include "main.h"
#include "io_stream.h"
#include "log.h"

using namespace draw;

unsigned long current_tick, last_tick;

extern "C" int system(const char* command);

void apply_pallette_cicle(unsigned char* pal, unsigned dwCurrentTime);
void check_translation();
void initialize_translation(const char* locale);
void focus_beforemodal();
void focus_finish();
void load_pallette(unsigned char* p1, int koeff = 4);
void theme_inititalize();

static color default_palette[256];

color getcolor(unsigned char i) {
	return default_palette[i];
}

static void openerror() {
	char dir[512]; char temp[512];
	io::file::getdir(dir, sizeof(dir));
	stringbuilder sb(temp);
	sb.add("notepad \"%1\\errors.txt\"", dir);
	system(temp);
}

static void update_tick() {
	auto tick = getcputime();
	if(last_tick && tick > last_tick)
		current_tick += tick - last_tick;
	last_tick = tick;
}

static void beforemodal() {
	update_tick();
	focus_beforemodal();
	apply_pallette_cicle((unsigned char*)palt, current_tick);
	cursor.set(res::INTRFACE, 267);
}

static void finish() {
	focus_finish();
}

static void tips() {
	cursor.position = hot.mouse;
	caret = cursor.position;
	cursor.paint();
}

int start_application(fnevent proc, fnevent afterread) {
	if(!proc)
		return -1;
	palt = default_palette;
	load_pallette((unsigned char*)palt);
	bsreq::read("rules/Basic.txt");
	initialize_translation("ru");
	if(afterread)
		afterread();
	check_translation();
	if(log::geterrors()) {
		log::stoplogging();
		openerror();
		return -1;
	}
	pbeforemodal = beforemodal;
	//pbackground = paint;
	//answers::beforepaint = answers_beforepaint;
	//answers::paintcell = menubt;
	pfinish = finish;
	ptips = tips;
	metrics::border = 6;
	metrics::padding = 2;
	initialize(getnm("AppTitle"));
	syscursor(false);
	settimer(40);
	theme_inititalize();
	setnext(proc);
	start();
	return 0;
}