#include "filelist.h"
#include "io_stream.h"

void filelist::choose(const char* folder, const char* filter, bool not_extensions) {
	if(!filter)
		filter = "*.*";
	clear();
	for(io::file::find find(folder); find; find.next()) {
		auto p = find.name();
		if(p[0] == '.')
			continue;
		if(!szpmatch(p, filter))
			continue;
		char temp[260];
		if(not_extensions)
			p = szfnamewe(temp, p);
		add(p);
	}
}