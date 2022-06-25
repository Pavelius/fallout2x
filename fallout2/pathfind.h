#pragma once

namespace pathfind {
typedef unsigned short indext;
const indext Blocked = 0xFFFF;
const indext NotCalculatedMovement = 0xFFF0;
typedef indext(*fnto)(indext index, int direction);
extern int		maxcount;
extern int		maxdir;
extern fnto		to;
void			addwave(indext i);
void			blocknearest(indext index, indext cost);
void			blockrange(int range);
void			blockzero();
void			clearpath();
bool			isinitializated();
indext			getfarest(const indext* source);
indext			getnearest(const indext* source);
indext			getmove(indext i);
void			getmove(indext* destination);
unsigned		getpath(indext start, indext goal, indext* result, unsigned maximum);
indext			getwave();
void			makewave(indext start_index);
void			makewavex();
void			setmove(const indext* source);
void			setmove(indext i, indext v);
}