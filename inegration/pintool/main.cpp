#include <thread>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

int func()
{
	char * buffer = (char*)malloc(128);
	sprintf(buffer,"Hellow world!");
	printf("%s\n",buffer);
	free(buffer);
}

int main()
{
	printf("ok\n");
	std::thread a(func);
	sleep(1);
	std::thread b(func);
	a.join();
	b.join();
}