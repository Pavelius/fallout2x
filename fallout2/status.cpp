#include "crt.h"
#include "stringbuilder.h"

static array messages(1);

void statusv(const char* format, unsigned count) {
	int n = messages.getcount();
	if(n == 0) {
		messages.reserve(count + 1);
		auto p = (char*)messages.data;
		memcpy(p, format, count);
		p[count] = 0;
		messages.count += count + 1;
	} else {
		messages.reserve(n + count);
		auto p = (char*)messages.ptr(n - 1);
		memcpy(p, format, count);
		p[count] = 0;
		messages.count += count;
	}
}

void statusv(const char* format, const char* format_param) {
	char temp[2048]; stringbuilder sb(temp);
	if(messages)
		sb.add("\n");
	sb.add("~ ");
	sb.addv(format, format_param);
	statusv(temp, sb.getlenght());
}

const char* getstatusmessage() {
	return (char*)messages.ptr(0);
}