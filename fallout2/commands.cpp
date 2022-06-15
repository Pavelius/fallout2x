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
	if(character::last->basic.getplayerstats() > 0) {
		gui.set(gui.get() + 1);
		character::last->update();
	}
}

static void add_trait() {
	auto id = (perki*)gui.object - bsdata<perki>::elements;
	if(gui.number)
		//gui.set((gui.get() & (~gui.mask)));
		character::last->perks.remove(id);
	else {
		auto maximum = 2;
		auto total = character::last->getperkstotal();
		if(total < maximum)
			//gui.set((gui.get() | gui.mask));
			character::last->perks.set(id);
		else
			draw::message(getnm("ErrorTraitLimit"));
	}
	character::last->update();
}

static void add_tag() {
	auto id = (stat_s)((stati*)gui.object - bsdata<stati>::elements);
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
	auto id = (stat_s)((stati*)gui.object - bsdata<stati>::elements);
	if(character::last->basic.stats[id] > 1) {
		character::last->basic.stats[id]--;
		character::last->update();
	}
}

static void add_number() {
	auto i = gui.get();
	if(i < 100)
		gui.set(i + 1);
}

static void sub_number() {
	auto i = gui.get();
	if(i > 1)
		gui.set(i - 1);
}

static void open_dialog() {
	dialog::open(gui.id);
}

static void character_delete() {
	last_list_current = -1;
	filelistsource.choose("characters/premade", "*.chr", true);
	if(!dialog::open(gui.id))
		return;
	//if(message(getnm("ReallyWantDelete"), getedit()))
}

static void character_load() {
	last_list_current = -1;
	filelistsource.choose("characters/premade", "*.chr", true);
	if(!dialog::open(gui.id))
		return;
	character::last->read(getedit());
	draw::buttonok();
}

static void character_save() {
	last_list_current = -1;
	filelistsource.choose("characters/premade", "*.chr", true);
	if(!dialog::open(gui.id))
		return;
	character::last->write(getedit());
	draw::buttonok();
}

static void character_export() {
	last_list_current = -1;
	filelistsource.choose("characters/export", "*.txt", true);
	if(!dialog::open(gui.id))
		return;
	character::last->exporting(getedit());
	draw::buttonok();
}

void character_generate() {
	dialog::open("CharacterGenerator");
}

BSDATA(command) = {
	{"AddNumber", '+', add_number},
	{"AddStat", '+', add_stat},
	{"AddTag", KeySpace, add_tag},
	{"AddTrait", KeySpace, add_trait},
	{"Adventure", 0, open_dialog},
	{"Cancel", KeyEscape, buttoncancel},
	{"CharacterAge", 'A', open_dialog},
	{"CharacterDelete", 'D', character_delete},
	{"CharacterExport", 'E', character_export},
	{"CharacterGender", 'G', open_dialog},
	{"CharacterGenerator", 'E', character_generate},
	{"CharacterLoad", 'L', character_load},
	{"CharacterName", 'N', open_dialog},
	{"CharacterSave", 'S', character_save},
	{"Next", KeyEnter, buttonok},
	{"Options", 'O', open_dialog},
	{"SubNumber", '-', sub_number},
	{"SubStat", '-', sub_stat},
};
BSDATAF(command)

static const char* character_name() {
	return character::last->getname();
}

static void character_gender(const control& e) {
	gui.title = "Ìóæ";
}

static void character_age(const control& e) {
	auto v = character::last->stats[e.data.value];
	sb.clear(); sb.add("%1i %-Years", v);
	gui.title = sb.begin();
}

static void character_trait(const control& e) {
	gui.checked = gui.number;
}

static void character_skill(const control& e) {
	auto id = e.data.value;
	gui.checked = character::last->istagged(id);
	sb.clear(); sb.add("%1i%%", gui.get());
	gui.value = sb.begin();
}

static void character_stat(const control& e) {
	sb.clear(); sb.add("%1i", gui.get());
	gui.value = sb.begin();
}

static void character_stat_percent(const control& e) {
	sb.clear(); sb.add("%1i%%", gui.get());
	gui.value = sb.begin();
}

static void character_stat_leveled(const control& e) {
	fore = getcolor(ColorDisable);
}

static void character_name(const control& e) {
	gui.title = getnm(character::last->id);
}

static void character_grade(const control& e) {
	size_t value = gui.get();
	if(value < 0)
		value = 0;
	else if(value > bsdata<gradei>::source.getcount())
		value = bsdata<gradei>::source.getcount() - 1;
	gui.title = getnmsh(bsdata<gradei>::elements[value].id);
}

static void character_primary_total(const control& e) {
	gui.number = character::last->basic.getplayerstats();
}

static void character_tagged_total(const control& e) {
	gui.number = character::last->get(SkillTagPoints) - character::last->gettaggedskills();
}

static void text_message(const control& e) {
	gui.title = result_text;
	gui.flags = AlignCenterCenter;
}

static void filelist_getname(const void* object, stringbuilder& sb) {
	sb.add(*((const char**)object));
}

static void file_list(const control& e) {
	static int auto_complete;
	gui.pgetname = filelist_getname;
	gui.data = filelistsource.data;
	gui.size = sizeof(filelistsource.data[0]);
	gui.count = filelistsource.count;
	if(last_list_current == -1 || last_list_current != auto_complete) {
		auto_complete = last_list_current;
		if(last_list_current != -1) {
			if(last_list_current < (int)filelistsource.count)
				setedit(filelistsource.data[last_list_current]);
		}
	}
}

void draw::messagev(const char* format, const char* format_param) {
	sb.clear(); sb.addv(format, format_param);
	dialog::open("Message");
}

static void pbefore(const control& e) {
	auto pd = character::last;
	auto& ei = e.data.geti();
	if(ei.source)
		gui.object = ei.source->ptr(e.data.value);
	if(e.data.iskind<stati>()) {
		gui.link(pd->stats[e.data.value]);
		gui.number = pd->stats[e.data.value];
	} else if(e.data.iskind<perki>()) {
		auto v = e.data.value;
		gui.link(pd->perks.getbyte(v), pd->perks.getmask(v));
		gui.number = pd->perks.is(v) ? 1 : 0;
	}
}

void initialize_dialog() {
	control::pbefore = pbefore;
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