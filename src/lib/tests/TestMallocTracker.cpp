/*****************************************************
             PROJECT  : numaprof
             VERSION  : 1.1.2
             DATE     : 11/2020
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include "../core/PageTable.hpp"
#include "../core/MallocTracker.hpp"

/***************** USING NAMESPACE ******************/
using namespace numaprof;

/*******************  FUNCTION  *********************/
TEST(MallocTracker,constructor)
{
	PageTable table;
	MallocTracker tracker(&table, 1);
}
