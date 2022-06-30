#include "chat.h"
#include "io_stream.h"
#include "logparse.h"
#include "script.h"

using namespace log;

BSDATAD(speech)
BSDATAD(chat)

const char* chat::folder;
chat* chat::last;

static bool isevent(const char* p) {
	return p[0] == '#' && isnum(p[1]);
}

static bool isanswer(const char* p) {
	return isnum(*p);
}

static const char* read_string(const char* p, stringbuilder& result) {
	result.clear();
	if(p[0] == '#' || isnum(p[0]))
		return p;
	while(*p) {
		if(*p == '\n' || *p == '\r') {
			p = skipwscr(p);
			if(p[0] == '#' || isanswer(p))
				break;
			result.addch('\n');
		} else
			result.addch(*p++);
	}
	return p;
}

static const char* getfilename(const char* id) {
	static char temp[260];
	stringbuilder sb(temp);
	if(chat::folder) {
		sb.add(chat::folder);
		sb.add("/");
	}
	sb.add("%1.txt", id);
	return temp;
}

static void add(variants& e, variant v) {
	if(!e.count)
		e.start = bsdata<variant>::source.getcount();
	auto p = (variant*)bsdata<variant>::source.add();
	*p = v;
	e.count++;
}

static const char* read_variants(const char* p, stringbuilder& result, variants& source, speech* pe) {
	while(allowparse && ischa(*p)) {
		p = readidn(p, result);
		p = skipws(p);
		auto pn = result.begin();
		int bonus; p = readbon(p, bonus);
		p = skipws(p);
		variant v = (const char*)result.begin();
		if(!v)
			log::error(p, "Can't find variant `%1`", result.begin());
		else
			v.counter = bonus;
		add(source, v);
	}
	return p;
}

static const char* read_event(const char* p, short& parent, stringbuilder& sb) {
	if(!allowparse)
		return p;
	p = stringbuilder::read(skipws(p), parent);
	auto pe = bsdata<speech>::add(); pe->clear();
	pe->index = parent;
	pe->next = -1;
	p = read_variants(skipws(p), sb, pe->tags, pe);
	p = read_string(skipwscr(p), sb);
	pe->text = getstring(sb);
	return p;
}

static const char* read_answers(const char* p, short parent, stringbuilder& sb) {
	while(allowparse && isanswer(p)) {
		auto pe = bsdata<speech>::add(); pe->clear();
		pe->index = parent;
		p = stringbuilder::read(p, pe->next);
		p = read_variants(skipws(p), sb, pe->tags, pe);
		if(!checksym(p, ')'))
			break;
		p = read_string(skipws(p + 1), sb);
		pe->text = getstring(sb);
	}
	return p;
}

const speech* chat::find(short index) {
	auto push_last = last;
	for(auto& e : elements) {
		if(e.index != index)
			continue;
		if(e.isanswer())
			continue;
		if(e.isallow()) {
			last = push_last;
			return &e;
		}
	}
	last = push_last;
	return 0;
}

void chat::read() {
	auto p = log::read(getfilename(id), true);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	elements.start = bsdata<speech>::source.getcount();
	allowparse = true;
	while(allowparse && *p) {
		if(!isevent(p)) {
			log::error(p, "Expected symbol `#` followed by event number");
			break;
		}
		short event_parent = -1; sb.clear();
		p = read_event(p + 1, event_parent, sb);
		p = read_answers(p, event_parent, sb);
	}
	elements.count = bsdata<speech>::source.getcount() - elements.start;
	log::close();
}

void chat::readall() {
	if(!folder)
		return;
	for(io::file::find file(folder); file; file.next()) {
		auto pn = file.name();
		if(pn[0] == '.')
			continue;
		if(!szpmatch(pn, "*.txt"))
			continue;
		char temp[260];
		auto p = bsdata<chat>::add();
		p->id = szdup(szfnamewe(temp, pn));
		p->read();
	}
}