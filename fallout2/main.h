#include "area.h"
#include "color.h"
#include "crt.h"
#include "drawable.h"
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
	DamageMelee, EnemyDamageResistance, DamageResistance, CarryWeight,
	CriticalHit, CriticalHitTable,
	CriticalMiss, CriticalMissTable,
	Sequence,
	PerkRate, SkillRate, HealingRate, PartyLimit,
	PoisonResistance, RadiationResistance, Age, Level,
	OneHandedWeaponBonus, TwoHandedWeaponBonus,
	PrimaryPoints, SkillTagPoints, SkillPoints, PoisonPoints, RadiationPoints,
	SmallGuns, BigGuns, EnergyWeapon, Unarmed, MeleeWeapon, Throwing
};
enum material_s : unsigned char {
	Glass, Metal, Plastic, Wood, Dirt, Stone, Cement, Leather
};
enum animate_s : unsigned char {
	AnimateStand, AnimateWalk, AnimatePickup, AnimateUse, AnimateDodge,
	AnimateDamaged, AnimateDamagedRear,
	AnimateUnarmed1, AnimateUnarmed2, AnimateThrown, AnimateRun,
	AnimateKnockOutBack, AnimateKnockOutForward,
	// Kill animation
	AnimateKilledSingle, AnimateKilledBurst, AnimateKilledBurstAuto, AnimateKilledBlowup, AnimateKilledMelt,
	AnimateBloodedBack, AnimateBloodedForward,
	AnimateStandUpBack, AnimateStandUpForward,
	// Dead body (1 frame animation)
	AnimateDeadBackNoBlood, AnimateDeadForwardNoBlood,
	AnimateDeadSingle, AnimateDeadBurst, AnimateDeadBurstAuto, AnimateDeadBlowup, AnimateDeadMelt,
	AnimateDeadBack, AnimateDeadForward,
	// Weapon Block
	FirstWeaponAnimate,
	AnimateWeaponTakeOn = FirstWeaponAnimate, AnimateWeaponStand, AnimateWeaponTakeOff, AnimateWeaponWalk, AnimateWeaponDodge,
	AnimateWeaponThrust, AnimateWeaponSwing,
	AnimateWeaponAim,
	AnimateWeaponSingle, AnimateWeaponBurst, AnimateWeaponFlame,
	AnimateWeaponThrow, AnimateWeaponAimEnd,
	// Weapon Animate
	AnimateClub,
	AnimateHammer = AnimateClub + 13,
	AnimateSpear = AnimateHammer + 13,
	AnimatePistol = AnimateSpear + 13,
	AnimateSMG = AnimatePistol + 13,
	AnimateRifle = AnimateSMG + 13,
	AnimateHeavyGun = AnimateRifle + 13,
	AnimateMachineGun = AnimateHeavyGun + 13,
	AnimateRocketLauncher = AnimateMachineGun + 13,
	LastAnimation = AnimateRocketLauncher + 13
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
struct animationi {
	const char*			id;
	animate_s			next, next_dead;
};
struct materiali {
	const char*			id;
};
struct list : nameable {
	variants			elements;
};
struct stati {
	const char*			id;
	int					avatar;
	variants			formula;
	short				minimum, maximum;
};
struct globalstati {
	const char*			id;
	short				value;
};
struct perki {
	const char*			id;
	variants			use;
	short				avatar, order;
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
	const stati*		tags[3];
	perka				perks;
};
struct character : nameable, statable {
	statable			basic;
	perka				perks;
	skilla				tags;
	int					experience;
	static character*	last;
	character*			boss;
	static character*	add(const char* id);
	void				clear() { memset(this, 0, sizeof(*this)); }
	void				exporting(const char* id) const;
	int					get(stat_s v) const { return stats[v]; }
	int					getperkstotal() const;
	int					gettaggedskills() const;
	bool				istagged(valuet v) const;
	static void			initialize();
	bool				open(const char* id);
	void				read(const char* id);
	void				set(stat_s v, int i) { stats[v] = i; }
	void				settag(valuet v, int i);
	void				update();
	void				write(const char* id) const;
};
struct spriteable : drawable {
	res					resource;
	void				clear() { memset(this, 0, sizeof(*this)); }
	void				paint() const;
	void				set(res r, short cicle);
};
struct anminfo {
	unsigned short		fps, frame_count, frame_act;
	point				offset[6];
	point				delta[6];
	static const anminfo* get(const sprite* p);
	static const anminfo* get(res rid);
	static const point	getoffset(const sprite* p, int frame);
	int					getfps() const { return fps ? fps : 10; }
};
struct scenery : nameable {
	short				frame, index;
	void				getinfo(stringbuilder& sb) const;
	void				paint() const;
	void				painted() const;
	static const scenery* last;
};
namespace draw {
void					messagev(const char* format, const char* format_param);
inline void				message(const char* format, ...) { messagev(format, xva_start(format)); }
}
extern spriteable		cursor;
extern areai			loc;
extern int				last_value, last_stat;
extern unsigned long	current_tick;

extern color			getcolor(unsigned char i);
extern const char*		getedit();