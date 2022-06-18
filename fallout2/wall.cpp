#include "main.h"

const walli* walli::last;

BSDATA(walli::doori) = {
	{38, 3},
	{41, 3},
	{44, 3, true},
	{47, 3, true},
};

short walli::next(short i) {
	switch(i) {
	case 1: return 3;
	case 18: return 21;
	case 48: return 46;
	default: return i + 1;
	}
}