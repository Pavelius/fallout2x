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

const int tile_width = 80; // Width of isometric tile
const int tile_height = 36; // Height of isometric tile

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
enum lightf : short unsigned {
	NorthSouth = 0,
	EastWest = 0x0800,
	NorthCorner = 0x1000,
	SouthCorner = 0x2000,
	EastCorner = 0x4000,
	WestCorner = 0x8000
};
enum actionf {
	KneelDownWhenUsing = 0x0001,
	CanBeUsed = 0x0008,
	UseOnSomething = 0x0010,
	CanLookAt = 0x0020,
	CanTalkWith = 0x0040,
	CanPickUp = 0x0080,
};
enum objectf : unsigned {
	Flat = 0x00000008,
	NoBlock = 0x00000010, // Doesn't block the tile
	MultiHex = 0x00000800,
	NoHighlight = 0x00001000, // Doesn't highlight the border; used for containers
	TransRed = 0x00004000,
	TransNone = 0x00008000, // Opaque
	TransWall = 0x00010000,
	TransGlass = 0x00020000,
	TransSteam = 0x00040000,
	TransEnergy = 0x00080000,
	LightThru = 0x20000000,
	ShootThru = 0x80000000
};
enum {
	ColorDisable = 0x60, ColorText = 0xD7, ColorCheck = 0x03, ColorInfo = 0xE4, ColorButton = 0x3D,
	ColorState = 0x90,
	ColorRed = 254
};
typedef flagable<16>	perka;
typedef flagable<4>		skilla;
struct nameable {
	const char*			id;
	const char*			getname() const { return getnm(id); }
};
struct lighti : nameable {
	unsigned			value;
};
struct objectfi : lighti {
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
struct sceneryi : nameable {
	short				frame, index;
	material_s			material;
	unsigned			light, object, action;
	void				getinfoed(stringbuilder& sb) const;
	bool				is(objectf v) const { return (object & v) != 0; }
	void				paint() const;
	static const sceneryi* last;
};
struct tilei : nameable {
	short				frame, index;
	material_s			material;
	void				paint() const;
	static const tilei*	last;
};
struct walli : nameable {
	short				frame, index;
	material_s			material;
	unsigned			light, object;
	static walli*		findindex(short i);
	void				getinfoed(stringbuilder& sb) const;
	bool				is(lightf v) const { return (light & v) != 0; }
	bool				is(objectf v) const { return (object & v) != 0; }
	const walli*		next() const;
	static short		next(short i);
	void				paint() const;
	static void			set(point h, short i);
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
inline point			i2t(indext i) { return {(short)(i % ((mps * 2)) / 2), (short)(i / ((mps * 2)) / 2)}; }
inline point			i2h(indext i) { return {(short)(i % (mps * 2)), (short)(i / (mps * 2))}; }
extern point			h2s(point v);
indext					h2i(point pt);
inline point			h2t(point v) { return {(short)(v.x / 2), (short)(v.y / 2)}; }
extern point			s2h(point pt);
extern point			s2t(point v);
inline indext			t2i(point v) { return v.y * mps + v.x; }
extern point			t2s(point v);
inline point			t2h(point v) { return {(short)(v.x * 2), (short)(v.y * 2)}; }