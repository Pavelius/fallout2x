#include "chat.h"
#include "logparse.h"
#include "script.h"

BSDATAD(speech)
BSDATAD(chat)

const char* chat::folder;

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

static const char* read_prompt(const char* p) {
	return p;
}

void chat::read() {
	auto p = log::read(getfilename(id), true);
	if(!p)
		return;
	elements.start = bsdata<speech>::source.getcount();
	while(*p)
		p = read_prompt(p);
	elements.count = bsdata<speech>::source.getcount() - elements.start;
	log::close();
}