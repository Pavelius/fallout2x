#include "command.h"
#include "draw.h"
#include "dialog.h"
#include "filelist.h"
#include "main.h"

static char result_text[512];
static stringbuilder sb(result_text);
static filelist filelistsource;
static array any_source;
extern int last_list_current;

void setedit(const char* format, ...);

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
		else
			draw::message(getnm("ErrorTraitLimit"));
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
		else
			draw::message(getnm("ErrorTagLimit"));
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
	dialog::open(control::last->command->id, 1);
}

static void character_delete() {
	dialog::open(control::last->command->id, 1);
}

static void character_load() {
	last_list_current = -1;
	filelistsource.choose("characters/premade", "*.chr", true);
	dialog::open(control::last->command->id, 1);
	if(!dialog::open(control::last->command->id, 1))
		return;
}

static void character_save() {
	last_list_current = -1;
	filelistsource.choose("characters/premade", "*.chr", true);
	dialog::open(control::last->command->id, 1);
	if(!dialog::open(control::last->command->id, 1))
		return;
}

static void character_export() {
	last_list_current = -1;
	filelistsource.choose("characters/export", "*.txt", true);
	if(!dialog::open(control::last->command->id, 1))
		return;
	character::last->exporting(getedit());
}

BSDATA(command) = {
	{"Age", 'A', open_dialog},
	{"AddStat", '+', add_stat},
	{"AddTag", KeySpace, add_tag},
	{"AddTrait", KeySpace, add_trait},
	{"Cancel", KeyEscape, buttoncancel},
	{"CharacterDelete", 'D', character_delete},
	{"CharacterExport", 'E', character_export},
	{"CharacterLoad", 'L', character_load},
	{"CharacterSave", 'S', character_save},
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
	control::view.title = "���";
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
}

static void filelist_getname(const void* object, stringbuilder& sb) {
	sb.add(*((const char**)object));
}

static void file_list(const control& e) {
	static int auto_complete;
	control::view.pgetname = filelist_getname;
	control::view.source.data = filelistsource.data;
	control::view.source.count = filelistsource.count;
	control::view.source.size = sizeof(filelistsource.data[0]);
	if(last_list_current == -1 || last_list_current != auto_complete) {
		auto_complete = last_list_current;
		if(last_list_current != -1) {
			if(last_list_current < (int)filelistsource.count)
				setedit(filelistsource.data[last_list_current]);
		}
	}
}

void draw::messagev(const char* format, const char* format_param, int mode) {
	sb.clear(); sb.addv(format, format_param);
	dialog::open("Message", mode);
}

BSDATA(decorator) = {
	{"Age", character_age},
	{"Name", character_name},
	{"FileList", file_list},
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