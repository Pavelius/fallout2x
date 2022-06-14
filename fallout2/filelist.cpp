#include "filelist.h"
#include "io_stream.h"

void filelist::choose(const char* folder, const char* filter) {
	if(!filter)
		filter = "*.*";
	for(io::file::find find(folder); find; find.next()) {
		auto p = find.name();
		if(p[0] == '.')
			continue;
		if(!szpmatch(p, filter))
			continue;
		add(p);
	}
}