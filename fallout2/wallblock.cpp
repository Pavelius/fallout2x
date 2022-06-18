#include "draw.h"
#include "main.h"
#include "my_initialize_list.h"

using namespace draw;

struct wallblock_old {
	const char*		id;
	short unsigned	start;
	std::initializer_list<point> points;
	void			paint(bool show_center) const;
};

enum direction_s : unsigned char {
	Hor, Ver, HorSkip, VerSkip,
};

struct wallblock {
	struct element {
		direction_s	d;
		char		c;
	};
	const char*		id;
	short unsigned	start;
	element			blocks[4];
	void			paint(bool show_center) const;
};

BSDATA(wallblock) = {
	{"TableSE", 1, {{Ver, 5}, {Hor, 5}}},
};

BSDATA(wallblock_old) = {
	{"TableSE", 1, {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 4}, {2, 5}, {3, 4}, {4, 5}, {5, 4}}},
	{"TableES", 11, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {5, 2}, {5, 3}, {5, 4}, {6, 4}}},
	//{"CaveWallW", 24, {{0, 0}, {1, -1}}},
	{"CaveWallW1", 26, {{0, 0}, {-1, -1}, {-2, 0}, {-3, -1}}},
	{"CaveWallW2", 30, {{0, 0}, {1, -1}, {2, 0}, {3, -1}}},
	{"CaveWallNE", 34, {{0, 0}, {0, 1}, {1, 1}, {2, 2}, {3, 1}, {3, 3}}},
	{"CaveWallE0", 40, {{0, 0}, {0, 1}, {0, 2}, {0, 3}}},
	{"CaveWallE1", 44, {{0, 0}, {0, 1}, {0, 2}, {0, 3}}},
	//{"CaveWall—1", 48, {{0, 0}, {0, 1}}},
	//{"CaveWall—2", 50, {{0, 0}, {0, 1}}},
	{"CaveWallCL1", 52, {{0, 0}, {2, 0}, {2, 1}}},
	{"CaveWallCL2", 55, {{0, 0}, {0, 1}, {0, -2}}},
	//{"CaveWallCL3", 58, {{0, 0}, {1, 0}}},
	{"CaveWallCL4", 60, {{0, 0}, {1, 0}, {2, 1}, {3, 1}}},
	{"CaveWallCL4", 64, {{0, 0}, {1, 0}, {2, 1}}},
	{"CaveWall5", 67, {{0, 0}, {1, -1}, {2, 0}}},
	{"CaveWallC1", 70, {{0, 0}, {0, 1}, {2, 0}, {2, 1}}},
	{"CaveWallNL", 74, {{0, 0}, {1, -1}, {2, 0}, {3, -1}}},
	//{"CaveWallNL", 78, {{0, 0}, {1, -1}}},
	//{"CaveWallNL", 80, {{0, 0}, {0, -1}}},
	//{"CaveWallNL", 82, {{0, 0}, {0, -1}}},
	//{"CaveWallNL", 84, {{0, 0}}},
	//{"CaveWallNL", 85, {{0, 0}}},
	//{"CaveWallNL", 86, {{0, 0}}},
	//{"CaveWallNL", 87, {{0, 0}, {1, 0}}},
	//{"CaveWallNL", 89, {{0, 0}, {1, 0}}},
	//{"CaveWallNL", 91, {{0, 0}}},
	//{"CaveColumn", 92, {{0, 0}}},
	//{"CaveColumn", 93, {{0, 0}}},
	{"CaveBrick", 95, {{0, 0}, {1, 0}, {0, -1}}},
	//{"CaveCorner", 98, {{0, 0}}},
	//{"CaveCorner", 99, {{0, 0}}},
	//{"CaveCorner", 100, {{0, 0}}},
	//{"CaveCorner", 101, {{0, 0}}},
	{"HanharWest", 102, {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9}, {0, 10}, {1, 10}, {2, 10}, {2, 11}, {3, 10}, {3, 11}, {4, 11}}},
	{"HangerEast", 119, {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}}},
	{"Car", 132, {{0, 0}, {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}}},
	{"CarWall2", 139, {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9}, {0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {0, 15}, {0, 16}, {0, 17}, {0, 18}, {1, 17}, {2, 17}, {2, 18}}},
	{"CarWall3", 161, {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9}, {0, 10}, {0, 11}, {0, 12}, {1, 11}, {1, 12}, {2, 12}}},
	{"CarWall4", 177, {{0, 0}, {0, 1}, {1, 0}, {2, 1}, {3, 0}, {4, 1}, {5, 0}, {6, 1}, {7, 0}, {8, 1}, {9, 0}, {10, 1}, {11, 0}, {12, 1}, {13, 0}, {14, 1}, {15, 0}}},
	{"TrackBarell", 194, {{0, 0}, {1, -1}, {2, 0}, {3, -1}, {4, 0}, {5, -1}, {6, 0}, {7, -1}, {8, 0}}},
	{"CarWall5", 203, {{0, 0}, {0, 1}, {1, 0}, {2, 1}, {3, 0}, {4, 1}, {5, 0}, {6, 1}, {7, 0}, {8, 0}}},
	{"CarWall6", 213, {{0, 0}, {1, -1}, {1, 0}, {2, 0}}},
	{"Fence", 217, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0}}},
	{"Wood", 231, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}}},
	{"BrickCorner", 243, {{0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 2}}},
	{"Wood", 248, {{0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 2}}},
	//{"WallVaultDoor", 253, {{0, 0}, {2, 0}}},
	//{"Door", 255, {{0, 0}, {2, 0}}},
	//{"Door", 257, {{0, 0}, {2, 0}}},
	{"WallVault", 259, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}}},
	{"WallVault", 272, {{0, 0}, {0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, 0}}},
	{"WallVault", 280, {{0, 0}, {0, 1}, {1, 0}, {2, 1}, {3, 0}, {3, 0}, {4, 1}, {4, 2}}},
	{"WallVault", 288, {{0, 0}, {0, 1}, {1, 0}}},
	{"WallDoor", 291, {{0, 0}, {1, -1}, {3, -1}}},
	{"VaultDoorNS", 294, {{0, 0}, {0, 2}, {0, 3}}},
	{"VaultDoorWE", 297, {{0, 0}, {1, -1}, {3, -1}}},
	//{"VaultDoorNSH", 300, {{0, 0}, {0, 2}}},
	{"WallVault", 302, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}}},
	{"VaultWall", 312, {{0, 0}, {1, -1}, {2, 0}, {3, -1}, {4, 0}}},
	//{"VaultWall", 317, {{0, 0}, {0, 1}}},
	{"Fence", 319, {{0, 0}, {0, -1}, {0, -2}, {2, 0}, {1, -1}, {4, 0}, {4, -3}, {4, -2}, {0, -4}, {2, -4}, {3, -4}, {4, -4}}},
	//{"Fence", 331, {{0, 0}, {0, -1}}},
	//{"Grid", 333, {{0, 0}, {0, 1}}},
	{"Grid", 335, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {7, 0}, {9, 0}, {11, 0}, {13, 0}}},
	//{"Grid", 345, {{0, 0}, {1, 0}}},
	{"Grid", 347, {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}}},
	//{"Grid", 355, {{0, 0}, {0, -1}}},
	{"Fence", 357, {{0, 0}, {1, -1}, {2, 0}, {4, 0}, {0, -2}, {4, -1}}},
	{"MetalBuilding", 363, {{0, 0}, {0, 1}, {0, 2}, {0, 5}, {0, 6}, {0, 7}}},
	//{"StoneDoor", 369, {{0, 0}, {2, 0}}},
	//{"StoneDoor", 371, {{0, 0}, {0, 1}}},
	//{"WoodDoor", 373, {{0, 0}, {2, 0}}},
	//{"WoodWall", 375, {{0, 0}, {1, 0}}},
	{"WoodWall", 377, {{0, 0}, {1, -1}, {2, 0}, {3, -1}, {4, 0}, {5, -1}, {6, 0}}},
	{"WoodWall", 384, {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}}},
	//{"WoodDuche", 393, {{0, 0}, {2, 0}}},
	{"WoodWall", 395, {{0, 0}, {1, -1}, {2, 0}, {3, -1}, {4, 0}, {5, -1}, {6, 0}, {7, -1}}},
	{"DirtWall", 403, {{0, 0}, {1, 0}, {2, 0}, {3, 0}}},
	//{"WoodDoor", 407, {{0, 0}, {0, 2}}},
	//{"WoodDoor", 409, {{0, 0}, {0, 2}}},
	//{"WoodDoor", 411, {{0, 0}, {0, 2}}},
	{"WoodWall", 413, {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}}},
	//{"StoneColumn", 421, {{0, 0}, {1, -1}}},
	{"StoneWall", 423, {{0, 0}, {2, 0}, {4, 0}, {1, -1}, {6, 0}, {8, 0}, {3, -1}}},
	//{"StoneColumn", 430, {{0, 0}}},
	{"StoneColumn", 431, {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}}},
	//{"StoneColumn", 438, {{0, 0}}},
	{"StoneRound", 439, {{0, 0}, {0, 1}, {1, 0}, {2, 1}}},
	{"StoneWall", 443, {{0, 0}, {2, 0}, {4, 0}, {6, 0}, {8, 0}}},
	{"WoodWall", 448, {{0, 0}, {1, -1}, {-1, -1}}},
	//{"StoneRound", 451, {{0, 0}}},
	{"StoneWall", 452, {{0, 0}, {0, -1}, {0, -2}, {0, -3}, {0, -4}}},
	//{"StoneColumn", 457, {{0, 0}}},
	//{"StoneCorner", 458, {{0, 0}}},
	//{"StoneWall", 459, {{0, 0}, {1, 0}}},
	//{"StoneCorner", 461, {{0, 0}}},
	{"StoneWall", 462, {{0, 0}, {0, 1}, {1, 0}, {2, 1}}},
	//{"StoneCorner", 466, {{0, 0}}},
	//{"BrickCorner", 467, {{0, 0}}},
	{"BrickWall", 468, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {5, 0}, {6, 0}, {7, 0}, {9, 0}, {10, 0}}},
	{"BrickWall", 477, {{0, 0}, {0, 1}, {0, 2}, {0, 5}, {0, 4}, {0, 3}, {0, 6}}},
	//{"BrickCorner", 484, {{0, 0}}},
	//{"BrickCorner", 485, {{0, 0}}},
	//{"BrickDoor", 486, {{0, 0}, {2, -1}}},
	//{"BrickWindow", 488, {{0, 0}, {1, -1}}},
	{"BrickWall", 490, {{0, 0}, {2, 0}, {3, -1}, {4, 0}, {5, -1}, {7, -1}, {8, 0}}},
	//{"BrickCorner", 497, {{0, 0}}},
};
BSDATAF(wallblock)

void list_input(int& origin, int perpage, int perline, int maximum);
void add_object(point h, const void* p, short frame);

static void marker() {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = getcolor(ColorRed);
	caret.x -= 4;
	line(caret.x + 8, caret.y);
	caret = push_caret;
	caret.y -= 4;
	line(caret.x, caret.y + 8);
	fore = push_fore;
	caret = push_caret;
}

short find_index(short frame) {
	for(auto& e : bsdata<walli>()) {
		if(e.frame == frame)
			return &e - bsdata<walli>::elements;
	}
	return 0;
}

//void place_wallblock(point h, const wallblock* ps) {
//	auto index = ps->start;
//	for(auto e : ps->points) {
//		auto pt = h + e;
//		auto p = bsdata<walli>::elements + find_index(index);
//		add_object(pt, p, p->frame);
//		index++;
//	}
//}

static point next_point(int i, point h, direction_s d) {
	switch(d) {
	case Ver:
		h.y++;
		break;
	case Hor:
		h.x++;
		break;
	default: break;
	}
	return h;
}

void place_wallblock(point h, const wallblock* ps) {
	auto index = ps->start;
	for(auto& e : ps->blocks) {
		for(auto i = 0; i < e.c; i++) {
			if(index != ps->start)
				h = next_point(i, h, e.d);
			auto p = bsdata<walli>::elements + find_index(index);
			add_object(h, p, p->frame);
			index++;
		}
	}
}

void wallblock::paint(bool show_center) const {
	auto ps = gres(res::WALLS);
	auto index = start;
	point h = {0, 0};
	for(auto e : blocks) {
		if(!e.c)
			break;
		for(auto i = 0; i < e.c; i++) {
			if(index != start)
				h = next_point(i, h, e.d);
			auto pt = caret + h2s(h);
			image(pt.x, pt.y, ps, ps->ganim(bsdata<walli>::elements[find_index(index)].frame, 0), 0);
			if(show_center)
				marker();
			index++;
		}
	}
}

void paint_wallblock(wallblock* p) {
	p->paint(true);
}