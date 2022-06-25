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
	Strenght, Perception, Endurance, Charisma, Intelligence, Agility, Luck,
	HP, HPCur, AP, APCur, AC,
	DamageMelee, EnemyDamageResistance, DamageResistance, CarryWeight,
	CriticalHit, CriticalHitTable,
	CriticalMiss, CriticalMissTable,
	Sequence,
	PerkRate, SkillRate, HealingRate, PartyLimit,
	PoisonResistance, RadiationResistance, Age, Level,
	OneHandedWeaponBonus, TwoHandedWeaponBonus,
	PrimaryPoints, SkillTagPoints, SkillPoints, PoisonPoints, RadiationPoints,
	SmallGuns, BigGuns, EnergyWeapon, Unarmed, MeleeWeapon, Throwing,
	FirstAid, Doctor,
	Sneak, Lockpick, Steal, Traps,
	Science, Repair, Speech, Barter, Gambling, Outdoorsman
};
enum perk_s : unsigned char {
	FastMetabolism, Bruiser, SmallFrame, OneHanded, Finesse, Kamikaze, HeavyHanded, FastShoot,
	BloodyMess, Jinxed, GoodNatured, ChemReliant, ChemResistant, SexAppeal, Skilled, Gifted
};
enum wear_s : unsigned char {
	BodyArmor, RightHandItem, LeftHandItem, Backpack,
};
enum material_s : unsigned char {
	Glass, Metal, Plastic, Wood, Dirt, Stone, Cement, Leather
};
enum itemkind_s : unsigned char {
	Armor, Container, Drug, Weapon, Ammo, Misc, LockKey
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
	// Weapon Block (Animate Knife)
	FirstWeaponAnimate,
	AnimateWeaponTakeOn = FirstWeaponAnimate, AnimateWeaponStand, AnimateWeaponTakeOff,
	AnimateWeaponWalk, AnimateWeaponDodge,
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
enum actionof {
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
enum specie_s : unsigned char {
	Men, Women, Children,
	SuperMutants, Ghouls,
	Brahmin, Radscorpions, Rats,
	Floaters, Centaurs, Robots, Dogs, Manti,
	DeathClaws, Plants, Geckos, Aliens, GiantAnts,
	BigBadBoss
};
enum damage_s : unsigned char {
	Phisycal, Laser, Fire, Plasma, Electrical, EMP, Explosive,
};
enum actionf : unsigned char {
	Aimed,
};
enum action_s : unsigned char {
	NoAction,
	ThrowPunch, ThrowPunchAimed, KickLeg,
	Swing, SwingAimed, Thrust, ThrustAimed, Throw,
	FireSingle, FireSingleAimed, FireBurst, Reload,
	Drop, Look, Talk, Turn, Unload,
	Use, UseOnObject, UseSkill,
};
enum itemf : unsigned char {
	TwoHanded, Usable, UsableOnSomething,
};
enum characterf : unsigned char {
	NeedNewAnimation,
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
struct actioni : nameable {
	short				frame;
	action_s			base;
	unsigned			flags;
	bool				is(actionf v) const { return (flags & FG(Aimed)) != 0; }
};
typedef adat<action_s, 16> actiona;
struct weari : nameable {
};
struct itemfi : nameable {
};
struct lighti : nameable {
	unsigned			value;
};
struct objectfi : lighti {
};
struct directioni : nameable {
};
struct animationi : nameable {
	animate_s			next;
};
struct materiali : nameable {
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
struct damagei : nameable {
};
struct gradei {
	const char*			id;
};
struct itemi : nameable {
	struct ammoi {
		short unsigned	ammo;
		unsigned char	count;
		char			ac, dr;
		short			dam_bonus;
	};
	struct armori {
		char			ac;
		res				male, female;
		char			threshold[Explosive + 1];
		char			resistance[Explosive + 1];
	};
	struct imagei {
		short			inventory;
		short			ground;
		short			animation;
		short			projectile;
	};
	struct weaponi {
		unsigned char	min, max;
		unsigned char	strenght;
		damage_s		type;
		unsigned char	ap, range;
		short unsigned	ammo;
		unsigned char	ammo_count;
		unsigned char	burst;
		short unsigned	critical_fail; // Number of critical failure table
		action_s		mode1, mode2;
	};
	int					size, weight, cost;
	imagei				avatar;
	material_s			material;
	weaponi				weapon;
	armori				armor;
	ammoi				ammo;
	stat_s				use;
	variants			effect;
	unsigned			flags;
};
struct character;
struct item {
	unsigned short		type;
	unsigned short		count;
	unsigned short		ammo_type;
	unsigned short		ammo_count;
	item() = default;
	item(unsigned short type, unsigned short count = 1) : type(type), count(count), ammo_type(0), ammo_count(0) {}
	item(const itemi* pi);
	item(const char* id) : item(bsdata<itemi>::find(id)) {}
	constexpr operator bool() const { return count != 0; }
	void				clear() { memset(this, 0, sizeof(*this)); }
	void				dropdown();
	actiona				getactions() const;
	item				getclip() const { return item(ammo_type, ammo_count); }
	int					getcount() const { return count; }
	const itemi&		geti() const { return bsdata<itemi>::elements[type]; }
	void				getinfo(stringbuilder& sb) const;
	itemkind_s			getkind() const;
	const char*			getname() const { return geti().getname(); }
	character*			getowner() const;
	wear_s				getownerslot() const;
	void				join(item& v);
	bool				is(itemf v) const { return geti().flags & FG(v); }
	bool				isallow(wear_s id) const;
	void				transfer(item& from, item& to);
};
struct statable {
	typedef unsigned short valuet;
	typedef slice<valuet> list;
	static list			skill_list;
	static list			trait_list;
	static list			primary_list;
	static valuet		first_skill;
	short				stats[Outdoorsman + 1];
	bool				iswoman() const { return false; }
	void				load(const statable& v);
	int					getplayerstats() const;
	int					getactionpoints(action_s v, const item& it) const;
	int					total(const list& source) const;
};
struct prototype : nameable, statable {
	const stati*		tags[3];
	perka				perks;
};
struct itemwear : item {
	character*			owner;
};
struct wearable : nameable, statable {
	item				wear[LeftHandItem + 1];
	char				action_index[2];
	void				additem(item v);
	actiona				getactions() const { return wear[RightHandItem].getactions(); }
	void				nextaction() { action_index[0]++; updateaction(); }
	void				updateaction();
};
struct itemlist : adat<item*, 256> {
	void				select(const wearable* pv);
};
struct spriteable : drawable {
	res					resource;
	void				clear() { memset(this, 0, sizeof(*this)); }
	void				paint() const;
	void				set(res r, short cicle);
};
struct order;
struct animable : wearable, spriteable {
	res					naked;
	direction_s			direction;
	static animable*	last;
	order*				addanimate(animate_s a);
	void				appear(point h);
	void				clearanimate();
	void				changeweapon();
	void				focusing() const;
	static animate_s	getbase(animate_s v, int* w);
	int					getdelay() const;
	static direction_s	getdirection(point s, point d);
	static short		getframe(direction_s v);
	static short		getframe(animate_s v, int weapon_index = 0);
	res					getlook() const;
	int					getweaponindex() const;
	void				moveto(indext i);
	void				nextanimate();
	void				paint() const;
	void				removeanimate();
	void				setanimate(animate_s v);
	void				turn(int d);
	void				wait();
	static void			waitall();
};
struct character : animable {
	statable			basic;
	perka				perks;
	skilla				tags;
	int					experience;
	specie_s			species;
	flagable<4>			flags;
	static character*	last;
	static character*	add(const char* id);
	void				clear() { memset(this, 0, sizeof(*this)); }
	void				exporting(const char* id) const;
	int					get(stat_s v) const { return stats[v]; }
	int					getperkstotal() const;
	int					gettaggedskills() const;
	bool				istagged(valuet v) const;
	bool				is(characterf v) const { return flags.is(v); }
	bool				is(perk_s v) const { return perks.is(v); }
	static void			initialize();
	void				read(const char* id);
	void				set(stat_s v, int i) { stats[v] = i; }
	void				settag(valuet v, int i);
	void				update();
	void				use(action_s v) const;
	void				write(const char* id) const;
private:
	void				apply_equipment();
};
struct anminfo {
	unsigned short		fps, frame_count, frame_act;
	point				shift[6];
	point				delta[6];
	static const anminfo* get(const sprite* p);
	static const anminfo* get(res rid) { return get(gres(rid)); }
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
struct tilegroup {
	struct element {
		int				count;
		point			shift;
	};
	const char*			id;
	short				start;
	size_t				count;
	element				elements[32];
	const element*		begin() const { return elements; }
	const element*		end() const { return elements + count; }
	int					getbeginid() const { return start; }
	int					getendid() const;
	void				getinfoed(stringbuilder& sb) const;
	void				getinfolist(stringbuilder& sb) const;
};
struct terrain : nameable {
	short				blocks[LeftUp + 1];
	short				alternate[4];
	static void			correct();
	static void			correct(indext i);
	short				correct(short t, unsigned char c) const;
	static void			correctaround(indext i);
	bool				isalternate(short v) const;
	bool				iscentral(short v) const;
	static const terrain* find(short v);
	bool				have(short v) const;
	short				random() const;
};
struct tilei : nameable {
	short				frame, index;
	material_s			material;
	void				paint() const;
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
struct order {
	point				position;
	animable*			object;
	animate_s			animate;
	constexpr explicit operator bool() const { return object != 0; }
	void				clear() { memset(this, 0, sizeof(*this)); }
	void				update();
	static void			updateall();
};
namespace draw {
void					messagev(const char* format, const char* format_param);
inline void				message(const char* format, ...) { messagev(format, xva_start(format)); }
int						opendialog(const char* id);
}
extern spriteable		cursor;
extern areai			loc;
extern int				last_value, last_stat;
extern unsigned long	current_tick;

inline int				d100() { return rand() % 100; }
extern color			getcolor(unsigned char i);
extern const char*		getedit();
extern point			h2s(point v);
indext					h2i(point pt);
extern point			s2h(point pt);
extern point			s2t(point v);
extern point			t2s(point v);