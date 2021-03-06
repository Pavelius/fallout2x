#include "command.h"
#include "draw.h"
#include "dialog.h"
#include "filelist.h"
#include "main.h"

static char result_text[512];
static stringbuilder sb(result_text);
static filelist files;
static itemlist items;
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

static void opendialog() {
	opendialog(gui.id);
}

static void character_delete() {
	last_list_current = -1;
	files.choose("characters/premade", "*.chr", true);
	if(!opendialog(gui.id))
		return;
	//if(message(getnm("ReallyWantDelete"), getedit()))
}

static void character_load() {
	last_list_current = -1;
	files.choose("characters/premade", "*.chr", true);
	if(!opendialog(gui.id))
		return;
	character::last->read(getedit());
	draw::buttonok();
}

static void character_save() {
	last_list_current = -1;
	files.choose("characters/premade", "*.chr", true);
	if(!opendialog(gui.id))
		return;
	character::last->write(getedit());
	draw::buttonok();
}

static void character_export() {
	last_list_current = -1;
	files.choose("characters/export", "*.txt", true);
	if(!opendialog(gui.id))
		return;
	character::last->exporting(getedit());
	draw::buttonok();
}

static void game_exit() {
}

static void change_weapon() {
	character::last->changeweapon();
}

static void turn_right() {
	character::last->turn(1);
}

static void turn_left() {
	character::last->turn(-1);
}

static void test_shoot() {
	character::last->useaction();
	character::last->wait();
}

BSDATA(command) = {
	{"AddNumber", '+', add_number},
	{"AddStat", '+', add_stat},
	{"AddTag", KeySpace, add_tag},
	{"AddTrait", KeySpace, add_trait},
	{"Adventure", 0, opendialog},
	{"Back", KeyEscape, buttoncancel},
	{"Cancel", KeyEscape, buttoncancel},
	{"ChangeWeapon", 'W', change_weapon},
	{"CharacterAge", 'A', opendialog},
	{"CharacterDelete", 'D', character_delete},
	{"CharacterExport", 'E', character_export},
	{"CharacterGender", 'G', opendialog},
	{"CharacterGenerator", 'E', opendialog},
	{"CharacterLoad", 'L', character_load},
	{"CharacterName", 'N', opendialog},
	{"CharacterPipboy", 'P', opendialog},
	{"CharacterPrint", 'P', opendialog},
	{"CharacterSave", 'S', character_save},
	{"CharacterSheet", 'C', opendialog},
	{"CharacterSkill", 'S', opendialog},
	{"CharacterInventory", 'I', opendialog},
	{"GameExit", 'E', game_exit},
	{"GameLoad", 'L', opendialog},
	{"GameMap", 'M', opendialog},
	{"GameSave", 'S', opendialog},
	{"GameOptions", KeyEscape, opendialog},
	{"GameSetting", 'O', opendialog},
	{"Next", KeyEnter, buttonok},
	{"Options", 'O', opendialog},
	{"SubNumber", '-', sub_number},
	{"SubStat", '-', sub_stat},
	{"TestShoot", 'T', test_shoot},
	{"TurnRight", '.', turn_left},
	{"TurnLeft", ',', turn_right},
};
BSDATAF(command)

static const char* character_name() {
	return character::last->getname();
}

static void character_gender(const control& e) {
	gui.title = "???";
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
	sb.clear(); sb.add("%1i%%", gui.number);
	gui.value = sb.begin();
}

static void character_stat(const control& e) {
	sb.clear(); sb.add("%1i", gui.number);
	gui.value = sb.begin();
}

static void character_stat_percent(const control& e) {
	sb.clear(); sb.add("%1i%%", gui.number);
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

static void character_wears(const control& e) {
	items.select(character::last);
	gui.linklist(items.data, items.count);
}

static void file_list(const control& e) {
	static int auto_complete;
	gui.linklist(files.data, files.count);
	gui.pgetname = filelist_getname;
	if(last_list_current == -1 || last_list_current != auto_complete) {
		auto_complete = last_list_current;
		if(last_list_current != -1) {
			if(last_list_current < (int)files.count)
				setedit(files.data[last_list_current]);
		}
	}
}

void draw::messagev(const char* format, const char* format_param) {
	sb.clear(); sb.addv(format, format_param);
	opendialog("Message");
}

static void pbefore(const control& e) {
	auto pd = character::last;
	auto& ei = e.data.geti();
	if(ei.source)
		gui.object = ei.source->ptr(e.data.value);
	if(e.data.iskind<stati>()) {
		gui.link(pd->stats[e.data.value]);
		gui.number = pd->stats[e.data.value];
	} else if(e.data.iskind<globalstati>()) {
		gui.link(bsdata<globalstati>::elements[e.data.value].value);
		gui.number = bsdata<globalstati>::elements[e.data.value].value;
	} else if(e.data.iskind<script>()) {
		auto p = bsdata<script>::elements + e.data.value;
		gui.data = p; gui.size = 0;
		p->proc(e.data.counter, p->param);
		gui.number = last_value;
	} else if(e.data.iskind<perki>()) {
		auto v = e.data.value;
		gui.link(pd->perks.getbyte(v), pd->perks.getmask(v));
		gui.number = pd->perks.is(v) ? 1 : 0;
	} else if(e.data.iskind<weari>()) {
		auto v = e.data.value;
		gui.link(pd->wear[v]);
		gui.number = pd->wear[v].geti().avatar.inventory;
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
	{"Wears", character_wears},
};
BSDATAF(decorator)