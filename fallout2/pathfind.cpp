#include "crt.h"
#include "pathfind.h"

using namespace pathfind;

static indext stack[256 * 256];
static indext* push_counter;
static indext* pop_counter;
static indext movement_rate[64 * 64];

int	pathfind::maxcount = 64 * 64;
int pathfind::maxdir = 6;
fnto pathfind::to;

void pathfind::clearpath() {
	for(auto i = 0; i < maxcount; i++)
		movement_rate[i] = NotCalculatedMovement;
	push_counter = stack;
	pop_counter = stack;
}

indext pathfind::getmove(indext i) {
	return movement_rate[i];
}

void pathfind::getmove(indext* destination) {
	if(maxcount)
		memcpy(destination, movement_rate, maxcount * sizeof(movement_rate[0]));
}

void pathfind::setmove(const indext* source) {
	if(maxcount)
		memcpy(movement_rate, source, maxcount * sizeof(movement_rate[0]));
}

void pathfind::setmove(indext i, indext v) {
	movement_rate[i] = v;
}

indext pathfind::getfarest(const indext* source) {
	auto nd = 0;
	auto sd = 64000;
	auto ni = Blocked;
	for(auto i = 0; i < maxcount; i++) {
		if(source[i] == Blocked || movement_rate[i] == Blocked)
			continue;
		if(source[i] <= sd && movement_rate[i] >= nd) {
			sd = source[i];
			nd = movement_rate[i];
			ni = i;
		}
	}
	return ni;
}

indext pathfind::getnearest(const indext* source) {
	auto nd = 64000;
	auto sd = 64000;
	auto ni = Blocked;
	for(auto i = 0; i < maxcount; i++) {
		if(source[i] == Blocked || movement_rate[i] == Blocked)
			continue;
		if(source[i] <= sd && movement_rate[i] <= nd) {
			sd = source[i];
			nd = movement_rate[i];
			ni = i;
		}
	}
	return ni;
}

unsigned pathfind::getpath(indext start, indext goal, indext* result, unsigned maximum) {
	auto pb = result;
	auto pe = result + maximum;
	auto curr = goal;
	auto cost = Blocked;
	while(pb < pe && curr != start) {
		auto next = curr;
		for(auto i = 0; i < maxdir; i++) {
			auto i1 = to(curr, i);
			if(i1 == Blocked)
				continue;
			if(i1 == start) {
				next = i1;
				break;
			}
			auto c1 = movement_rate[i1];
			if(c1 >= cost)
				continue;
			next = i1;
			cost = c1;
		}
		if(next == curr || next == start)
			break;
		if(curr == goal) {
			*pb++ = goal;
			if(pb >= pe)
				break;
		}
		*pb++ = next;
		curr = next;
	}
	return pb - result;
}

void pathfind::blockzero() {
	for(indext i = 0; i < maxcount; i++) {
		if(movement_rate[i] >= NotCalculatedMovement)
			movement_rate[i] = Blocked;
	}
}

void pathfind::blocknearest(indext index, indext cost) {
	for(int d = 0; d < maxdir; d++) {
		auto i1 = to(index, d);
		auto c1 = movement_rate[i1];
		if(i1 == Blocked || c1 == Blocked)
			continue;
		if(c1 && c1 < cost)
			continue;
		movement_rate[i1] = cost;
	}
}

void pathfind::addwave(indext i) {
	*push_counter++ = i;
	if(push_counter >= stack + sizeof(stack) / sizeof(stack[0]))
		push_counter = stack;
}

indext pathfind::getwave() {
	auto index = *pop_counter++;
	if(pop_counter >= stack + sizeof(stack) / sizeof(stack[0]))
		pop_counter = stack;
	return index;
}

void pathfind::makewavex() {
	while(pop_counter != push_counter) {
		auto index = getwave();
		auto cost = movement_rate[index] + 1;
		for(int d = 0; d < maxdir; d++) {
			auto i1 = to(index, d);
			if(i1 == Blocked)
				continue;
			auto c1 = movement_rate[i1];
			if(c1 == Blocked || (c1 < NotCalculatedMovement && c1 < cost))
				continue;
			movement_rate[i1] = cost;
			addwave(i1);
		}
	}
	blockzero();
}

void pathfind::makewave(indext start_index) {
	if(!isinitializated())
		return;
	movement_rate[start_index] = 0;
	addwave(start_index);
	makewavex();
}

void pathfind::blockrange(int range) {
	for(indext i = 0; i < maxcount; i++) {
		auto v = getmove(i);
		if(v == Blocked)
			continue;
		if(v > range)
			setmove(i, Blocked);
	}
}

bool pathfind::isinitializated() {
	return maxcount > 0 && to && maxdir > 0;
}