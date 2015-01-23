#include <sys/time.h>
#include <stdio.h>

unsigned long long millis()
{
	struct timeval t1;
	gettimeofday(&t1,NULL);
	return (1000 * t1.tv_sec) + (t1.tv_usec/1000);
}
