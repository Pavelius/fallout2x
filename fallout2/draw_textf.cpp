#include "crt.h"
#include "draw.h"

using namespace draw;

int draw::tab_pixels = 0;
static const char* text_start_string;
static int text_start_horiz;
static point maxcaret;

static bool match(const char** string, const char* name) {
	int n = zlen(name);
	if(memcmp(*string, name, n) != 0)
		return false;
	(*string) += n;
	return true;
}

static int gettabwidth() {
	return tab_pixels ? tab_pixels : textw(' ') * 4;
}

static const char* textspc(const char* p, int x1) {
	int tb;
	while(true) {
		switch(p[0]) {
		case ' ':
			p++;
			caret.x += draw::textw(' ');
			continue;
		case '\t':
			p++;
			tb = gettabwidth();
			caret.x = x1 + ((caret.x - x1 + tb) / tb) * tb;
			continue;
		}
		break;
	}
	return p;
}

static const char* wholeline(const char* p) {
	while(*p && *p != 10 && *p != 13)
		p++;
	return p;
}

static const char* word(const char* text) {
	while(((unsigned char)*text) > 0x20)
		text++;
	return text;
}

static void apply_line_feed(int x1, int dy) {
	if(maxcaret.x < caret.x)
		maxcaret.x = caret.x;
	caret.x = x1;
	caret.y += dy;
	if(maxcaret.y < caret.y)
		maxcaret.y = caret.y;
}

static const char* textfln(const char* p, int x1, int x2, color new_fore, const sprite* new_font) {
	auto push_fore = fore;
	auto push_font = font;
	unsigned flags = 0;
	fore = new_fore;
	font = new_font;
	while(true) {
		p = textspc(p, x1);
		const char* p2 = word(p);
		auto w = textw(p, p2 - p);
		if(caret.x + w > x2)
			apply_line_feed(x1, texth());
		text(p, p2 - p, flags);
		caret.x += w;
		p = textspc(p2, x1);
		if(p[0] == 0 || p[0] == 10 || p[0] == 13) {
			p = skipcr(p);
			apply_line_feed(x1, texth());
			break;
		}
	}
	apply_line_feed(caret.x, 0);
	fore = push_fore;
	font = push_font;
	return p;
}

static const char* text_block(const char* p, int x1, int x2) {
	while(p[0]) {
		caret.x = x1; width = x2 - x1;
		if(caret.y < clipping.y1) {
			text_start_string = p;
			text_start_horiz = caret.y - clipping.y1;
		}
		if(match(&p, "###")) // Header 3
			p = textfln(skipsp(p), x1, x2, fore, metrics::h3);
		else if(match(&p, "##")) // Header 2
			p = textfln(skipsp(p), x1, x2, fore, metrics::h2);
		else if(match(&p, "#")) // Header 1
			p = textfln(skipsp(p), x1, x2, fore, metrics::h1);
		else if(match(&p, "---")) { // Line
			p = skipspcr(p);
			auto push_x = caret.x;
			caret.y += 1;
			//caret.x = x1 - metrics::border;
			line(x2, caret.y);
			caret.x = push_x;
			caret.y += 5;
		} else
			p = textfln(p, x1, x2, fore, font);
	}
	return p;
}

void draw::textf(const char* p) {
	auto push_width = width;
	auto push_height = height;
	auto push_tab = tab_pixels;
	maxcaret.clear();
	text_start_string = 0;
	text_start_horiz = 0;
	auto x0 = caret.x; auto y0 = caret.y;
	p = text_block(p, x0, x0 + width);
	maxcaret.x -= x0; maxcaret.y -= y0;
	tab_pixels = push_tab;
	width = push_width;
	height = push_height;
}

void draw::textfs(const char* string) {
	auto push_caret = caret;
	auto push_clipping = clipping;
	clipping.clear(); caret = {};
	textf(string);
	clipping = push_clipping;
	caret = push_caret;
	width = maxcaret.x;
	height = maxcaret.y;
}