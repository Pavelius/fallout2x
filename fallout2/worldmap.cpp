#include "draw.h"
#include "main.h"

using namespace draw;

const int world_map_tile_width = 50; // Width of world map tile
const int world_map_tile_height = 50; // Height of world map tile
const int dx = 350;
const int dy = 300;

int worldmapi::get(point w) const {
	if(w.x < 0 || w.x >= width)
		return 0;
	if(w.y < 0 || w.y >= height)
		return 0;
	return data[w.y][w.x];
}

void worldmapi::set(point w, int value) {
	if(w.x < 0 || w.x >= width)
		return;
	if(w.y < 0 || w.y >= height)
		return;
	if(get(w) > value)
		return;
	data[w.y][w.x] = value;
}

inline point w2s(point w) {
	return{(short)(w.x * world_map_tile_width), (short)(w.y * world_map_tile_height)};
}

inline point s2w(point w) {
	return{(short)(w.x / world_map_tile_width), (short)(w.y / world_map_tile_height)};
}

namespace {
class adventurer {
	point		start, target;
	unsigned	timestart;
public:
	adventurer() : timestart(0) {
		stop();
		explore();
	}
	void explore() {
		auto w = s2w(getposition());
		game.set(w, 2);
		game.set({(short)(w.x - 1), (short)(w.y - 1)}, 1);
		game.set({(short)(w.x - 0), (short)(w.y - 1)}, 1);
		game.set({(short)(w.x + 1), (short)(w.y - 1)}, 1);
		game.set({(short)(w.x - 1), (short)(w.y + 0)}, 1);
		game.set({(short)(w.x + 1), (short)(w.y + 0)}, 1);
		game.set({(short)(w.x - 1), (short)(w.y + 1)}, 1);
		game.set({(short)(w.x - 0), (short)(w.y + 1)}, 1);
		game.set({(short)(w.x + 1), (short)(w.y + 1)}, 1);
	}
	bool canmove() const {
		return getposition() != target;
	}
	bool iscar() const {
		return false;
	}
	void stop() {
		start = getposition();
		target = start;
	}
	int getspeed() const {
		return iscar() ? 10 : 2;
	}
	rect getrect() const {
		auto pos = getposition();
		return {pos.x - 4, pos.y - 4, pos.x + 4, pos.y + 4};
	}
	point getposition() const {
		return game.position;
	}
	void setposition(point v) {
		game.position = v;
	}
	void update() {
		if(!timestart || (current_tick - timestart) > 1000)
			timestart = current_tick;
		unsigned hours = (current_tick - timestart) / 60;
		int dx = target.x - start.x;
		int dy = target.y - start.y;
		int lenght_total = isqrt(dx * dx + dy * dy);
		int lenght_passed = getspeed() * hours;
		if(lenght_passed >= lenght_total) {
			setposition(target);
			explore();
			stop();
		} else {
			auto v = getposition();
			v.x = start.x + dx * lenght_passed / lenght_total;
			v.y = start.y + dy * lenght_passed / lenght_total;
			setposition(v);
			explore();
		}
	}
	void painting(point camera) const {
		auto ps = gres(res::INTRFACE);
		if(!ps)
			return;
		auto pos = getposition();
		if(target != pos) {
			auto pt = pos - camera;
			image(pt.x, pt.y, ps, ps->ganim(138, 0), 0);
			pt = target - camera;
			image(pt.x - 5, pt.y - 5, ps, ps->ganim(139, 0), ImageNoOffset);
		} else {
			auto fr = ps->ganim(168, 0);
			auto& pf = ps->get(fr);
			auto pt = pos - camera;
			image(pt.x - pf.sx / 2, pt.y - pf.sy, ps, fr, ImageNoOffset);
		}
	}
	void moveto(point pos) {
		start = getposition();
		target = pos;
		timestart = current_tick;
	}
};
}

// Вся карта состоит из тайлов 50 на 50 пикселей.
// Тайлы группируются блоками - один рисунок 7 тайлов на 6 тайлов.
static void draw_map(const rect& screen, point camera, adventurer& player, sprite* ps, bool show_settlement) {
	auto fr = ps->ganim(339, 0);
	auto push_clip = clipping;
	clipping = screen;
	point origin = {(short)screen.x1, (short)screen.y1};
	// Нарисуем фоновые текстуры
	for(int iy = 0; iy < 5; iy++) {
		int y1 = dy * iy + screen.y1 - camera.y;
		for(int ix = 0; ix < 4; ix++) {
			int x1 = dx * ix + screen.x1 - camera.x;
			image(x1, y1, ps, fr + iy * 4 + ix, ImageNoOffset);
		}
	}
	// Нарисуем поселения и локации
	if(show_settlement) {
		auto old_palt = palt;
		color pallette[256];
		memcpy(pallette, palt, sizeof(pallette));
		//	draw::modify(pallette, 0, ColorText, 16);
		palt = pallette;
		//	for(auto& e : settlement_data) {
		//		if(!e.position)
		//			continue;
		//		if(getwm(e.position.x / world_map_tile_width, e.position.y / world_map_tile_height) < 1)
		//			continue;
		//		auto pt = e.position - camera + origin;
		//		auto fr = ps->ganim(336 + e.size, 0);
		//		auto pf = ps->get(fr);
		//		draw::image(pt.x - pf.sx / 2, pt.y - pf.sy / 2, ps, fr, ImageNoOffset, 128);
		//		const char* name = e.name;
		//		draw::text(pt.x - draw::textw(name) / 2, pt.y + pf.sy / 2 + 1, name);
		//	}
		palt = old_palt;
	}
	// Нарисуем туман войны
	int y2 = (camera.y + screen.height()) / world_map_tile_height + 1;
	for(short y = camera.y / world_map_tile_height; y < y2; y++) {
		if(y < 0 || y >= worldmapi::height)
			continue;
		int x2 = (camera.x + screen.width()) / world_map_tile_width + 1;
		for(short x = camera.x / world_map_tile_width; x < x2; x++) {
			auto pt = w2s({x, y}) - camera + origin;
			auto value = game.get({x, y});
			//if(!value)
			//	draw::rectf({pt.x, pt.y, pt.x + world_map_tile_width, pt.y + world_map_tile_height}, getcolor(0));
			//else if(value == 1)
			//	draw::rectf({pt.x, pt.y, pt.x + world_map_tile_width, pt.y + world_map_tile_height}, getcolor(0), 128);
		}
	}
	player.painting(camera - origin);
	if(ishilite(screen)) {
		if(hot.key == MouseLeft && hot.pressed)
			player.moveto(hot.mouse + camera - origin);
	}
	clipping = push_clip;
}

static void draw_cities(int x, int y, sprite* ps) {
	//rect rc = {x, y, x + 122, y + 184};
	//setclip(rc);
	//rectb(rc, colors::red);
	//draw::image(rc.x1 - 12, rc.y1, ps, ps->ganim(364, 0), ImageNoOffset);
}

//void gamei::worldmap() {
//	unsigned timestart = 0;
//	adventurer party;
//	while(ismodal()) {
//		auto ps = gres(INTRFACE);
//		if(!ps)
//			return;
//		if(party.canmove()) {
//			unsigned tm = getuitime();
//			if(!timestart)
//				timestart = tm;
//			if(tm >= timestart) {
//				timestart += 1000 / 8;
//				game.passtime(60);
//				party.update();
//				point pt = {451 / 2, 448 / 2};
//				camera = party.getposition() - pt;
//			}
//		} else
//			timestart = 0;
//		// Откорректируем позицию камеры
//		if(camera.x < 0)
//			camera.x = 0;
//		if(camera.x > dx * 4 - 451)
//			camera.x = dx * 4 - 451;
//		if(camera.y < 0)
//			camera.y = 0;
//		if(camera.y > dy * 5 - 446)
//			camera.y = dy * 5 - 446;
//		// Нарисуем экран
//		draw_map({21, 21, 21 + 451, 21 + 446}, camera, party, ps, true);
//		draw_cities(502, 132, ps);
//		// Рамка
//		image(0, 0, ps, ps->ganim(136, 0), ImageNoOffset);
//		// Смена дня и ночи
//		image(532, 48, ps, ps->ganim(365, game.gethour()+12), ImageNoOffset);
//		// Способ путишествия
//		if(party.iscar()) {
//			auto x = 500, y = 330;
//			image(x + 16, y + 10, ps, ps->ganim(433, game.getround() / 60), ImageNoOffset);
//			image(x, y, ps, ps->ganim(363, 0), ImageNoOffset);
//		} else
//			image(494, 331, ps, ps->ganim(366, 0), ImageNoOffset);
//		auto x = 487, y = 12;
//		numbersm(x, y, 2, game.getday(), 0);
//		monthname(x + 26, y+1, game.getmonth());
//		numbersm(x + 62, y, 4, game.getyear(), 0);
//		numbersm(x + 107, y, 4, game.gethour() * 100 + game.getminute(), 0);
//		domodal();
//		switch(hot.key) {
//		case KeyEscape: return;
//		case KeyRight: camera.x += 8; break;
//		case KeyLeft: camera.x -= 8; break;
//		case KeyUp: camera.y -= 8; break;
//		case KeyDown: camera.y += 8; break;
//		}
//	}
//}