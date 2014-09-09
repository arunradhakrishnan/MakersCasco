#include "measureTime.h"

unsigned long millis()
{
	clock_t ticks;
	unsigned long millis;
	static unsigned long clk_per_milli = CLOCKS_PER_SEC/1000;
	ticks = clock();
	millis = ticks / clk_per_milli;
	return millis;
}
