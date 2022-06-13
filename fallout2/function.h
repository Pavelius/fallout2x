#include "variant.h"

#pragma once

struct function {
	const char*		id;
	variants		script;
	static function* last;
};
