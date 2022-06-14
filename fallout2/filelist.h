#include "stringlist.h"

#pragma once

class filelist : public stringlist {
public:
	void		choose(const char* source, const char* filter);
};
