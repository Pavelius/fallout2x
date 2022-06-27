#include "crt.h"
#include "stringbuilder.h"

static array messages(1);

void statusv(const char* format, unsigned count) {
	int n = messages.getcount() - 1;
	if(n < 0)
		n = 0;
	messages.reserve(n + count + 1);
	memcpy(messages.ptr(n), format, count);
	((char*)messages.ptr(n))[count] = 0;
	messages.count += count + 1;
}

void statusv(const char* format, const char* format_param) {
	char temp[2048]; stringbuilder sb(temp);
	if(messages)
		sb.add("\n");
	sb.addv(format, format_param);
	statusv(temp, sb.getlenght());
}

const char* getstatusmessage() {
	return (char*)messages.ptr(0);
}