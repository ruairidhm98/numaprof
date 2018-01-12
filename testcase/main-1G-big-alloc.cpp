/*****************************************************
             PROJECT  : numaprof
             VERSION  : 0.0.0-dev
             DATE     : 05/2017
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstdio>
#include <cstdlib>
#include <cstring>

/*******************  FUNCTION  *********************/
int main(int argc, char ** argv)
{
	int repeat = 100;
	if (argc == 1)
		repeat = atoi(argv[1]);
	size_t size = 1024*1024*1024;
	for (int i = 0 ; i < repeat ; i++)
	{
		char * array = new char[size];
		memset(array,0,size);
		delete [] array;
	}
	return EXIT_SUCCESS;
}
