#include "color.h"
#include "crt.h"
#include "flagable.h"
#include "point.h"
#include "rect.h"
#include "resid.h"
#include "script.h"
#include "variant.h"
#include "widget.h"

#pragma once

enum stat_s : unsigned char {
	Strenght, Perception, Endurance, Charisma, Intellegence, Agility, Luck,
	HP, HPCur, AP, APCur, AC,
	DamageMelee, DamageResistance, CarryWeight,
	CriticalHit, CriticalHitTable,
	CriticalMiss, CriticalMissTable,
	Sequence,
	PerkRate, SkillRate, HealingRate, PartyLimit,
	PoisonResistance, RadiationResistance, Age, Level,
	PrimaryPoints, SkillTagPoints, SkillPoints, PoisonPoints, RadiationPoints,
	SmallGuns, BigGuns, EnergyWeapon, Unarmed, MeleeWeapon, Throwing
};
enum {
	ColorDisable = 0x60, ColorText = 0xD7, ColorCheck = 0x03, ColorInfo = 0xE4, ColorButton = 0x3D,
	ColorState = 0x90
};
typedef flagable<16>	perka;
typedef flagable<4>		skilla;
struct nameable {
	const char*			id;
	const char*			getname() const { return getnm(id); }
};
//class nameidable {
//	short unsigned		name_id;
//public:
//	void				act(const char* format, ...) const { actv(format, xva_start(format)); }
//	void				actv(const char* format, const char* format_param) const;
//	const char*			getname() const;
//};
struct list : nameable {
	variants			elements;
};
struct stati {
	const char*			id;
	variants			formula;
	short				minimum, maximum;
};
struct perki {
	const char*			id;
	variants			use;
};
struct gradei {
	const char*			id;
};
struct statable {
	typedef unsigned short valuet;
	typedef slice<valuet> list;
	static list			skill_list;
	static list			trait_list;
	static list			primary_list;
	static valuet		first_skill;
	short				stats[64];
	void				load(const statable& v);
	int					getplayerstats() const;
	int					total(const list& source) const;
};
struct prototype : nameable, statable {
};
struct character : prototype {
	statable			basic;
	perka				perks;
	skilla				tags;
	int					experience;
	static character*	last;
	static character*	add(const char* id);
	void				clear() { memset(this, 0, sizeof(*this)); }
	int					get(stat_s v) const { return stats[v]; }
	int					getperkstotal() const;
	int					gettaggedskills() const;
	bool				istagged(valuet v) const;
	static void			initialize();
	bool				open(const char* id);
	void				set(stat_s v, int i) { stats[v] = i; }
	void				settag(valuet v, int i);
	void				update();
};
struct drawable {
	point				position;
	void*				data;
	unsigned short		frame, frame_stop;
	void				paint() const;
};
namespace draw {
void					messagev(const char* format, const char* format_param, int mode = 1);
inline void				message(const char* format, ...) { messagev(format, xva_start(format), 1); }
}
extern color			getcolor(unsigned char i);
extern int				last_value, last_stat;
extern unsigned long	current_tick;