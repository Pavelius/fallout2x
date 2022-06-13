#include "command.h"
#include "draw.h"
#include "dialog.h"
#include "main.h"

static char result_text[512];
static stringbuilder sb(result_text);

using namespace draw;

static void add_stat() {
	auto id = control::last->getvalue();
	if(character::last->basic.getplayerstats() > 0) {
		character::last->basic.stats[id]++;
		character::last->update();
	}
}

static void add_trait() {
	auto id = control::last->getvalue();
	if(character::last->perks.is(id))
		character::last->perks.remove(id);
	else {
		auto maximum = 2;
		auto total = character::last->getperkstotal();
		if(total < maximum)
			character::last->perks.set(id);
	}
	character::last->update();
}

static void add_tag() {
	auto id = control::last->getvalue();
	if(character::last->istagged(id))
		character::last->settag(id, 0);
	else {
		auto maximum = character::last->get(SkillTagPoints);
		auto total = character::last->gettaggedskills();
		if(total < maximum)
			character::last->settag(id, 1);
	}
	character::last->update();
}

static void sub_stat() {
	auto id = control::last->getvalue();
	if(character::last->basic.stats[id] > 1) {
		character::last->basic.stats[id]--;
		character::last->update();
	}
}

static void open_dialog() {
	draw::message(getnm("ErrorTagLimit"));
}

BSDATA(command) = {
	{"Age", 'A', open_dialog},
	{"AddStat", '+', add_stat},
	{"AddTag", KeySpace, add_tag},
	{"AddTrait", KeySpace, add_trait},
	{"Cancel", KeyEscape, buttoncancel},
	{"Form"}, // Form control flow 
	{"Gender", 'G', open_dialog},
	{"Name", 'N', open_dialog},
	{"Next", KeyEnter, buttonok},
	{"Options", 'O', open_dialog},
	{"SubStat", '-', sub_stat},
};
BSDATAF(command)

static const char* character_name() {
	return character::last->getname();
}

static void character_gender(const control& e) {
	control::view.title = "Ìóæ";
}

static void character_age(const control& e) {
	auto v = character::last->stats[e.data.value];
	sb.clear(); sb.add("%1i %-Years", v);
	control::view.title = sb.begin();
}

static void character_trait(const control& e) {
	control::view.checked = character::last->perks.is(e.getvalue());
}

static void character_skill(const control& e) {
	control::view.checked = character::last->istagged(e.getvalue());
	sb.clear(); sb.add("%1i%%", character::last->stats[e.data.value]);
	control::view.value = sb.begin();
}

static void character_stat(const control& e) {
	sb.clear(); sb.add("%1i", character::last->stats[e.data.value]);
	control::view.value = sb.begin();
}

static void character_stat_percent(const control& e) {
	sb.clear(); sb.add("%1i%%", character::last->stats[e.data.value]);
	control::view.value = sb.begin();
}

static void character_stat_leveled(const control& e) {
	fore = getcolor(ColorDisable);
}

static void character_name(const control& e) {
	control::view.title = getnm(character::last->id);
}

static void character_grade(const control& e) {
	auto p = character::last;
	auto id = e.getvalue();
	size_t value = p->stats[id];
	if(value < 0)
		value = 0;
	else if(value > bsdata<gradei>::source.getcount())
		value = bsdata<gradei>::source.getcount() - 1;
	control::view.title = getnmsh(bsdata<gradei>::elements[value].id);
}

static void character_primary_total(const control& e) {
	control::view.number = character::last->basic.getplayerstats();
}

static void character_tagged_total(const control& e) {
	control::view.number = character::last->get(SkillTagPoints) - character::last->gettaggedskills();
}

static void text_message(const control& e) {
	control::view.title = result_text;
	control::view.flags = AlignCenterCenter;
	//fore = getcolor(ColorButton);
}

void draw::messagev(const char* format, const char* format_param, int mode) {
	sb.clear(); sb.addv(format, format_param);
	dialog::open("Message", mode);
}

BSDATA(decorator) = {
	{"Age", character_age},
	{"Name", character_name},
	{"Gender", character_gender},
	{"Grade", character_grade},
	{"Skill", character_skill},
	{"TagsTotal", character_tagged_total},
	{"Stat", character_stat},
	{"StatPercent", character_stat_percent},
	{"StatLeveled", character_stat_leveled},
	{"PrimaryStatsTotal", character_primary_total},
	{"Trait", character_trait},
	{"TextMessage", text_message},
};
BSDATAF(decorator)