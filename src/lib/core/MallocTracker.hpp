/*****************************************************
             PROJECT  : numaprof
             VERSION  : 1.1.2
             DATE     : 11/2020
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

/*******************  HEADERS  **********************/
#include "../common/Helper.hpp"
#include "AccessMatrix.hpp"
#include "PageTable.hpp"
#include "Stats.hpp"

/*******************  NAMESPACE  ********************/
namespace numaprof
{

/********************  MACROS  **********************/
//#define NUMAPROF_TRACE_ALLOCS	

/*******************  STRUCT  ***********************/
/**
 * Malloc informations to store for every allocation.
**/
struct MallocInfos
{
	/** base pointer of the allocation. **/
	size_t ptr;
	/** Size of the allocation to know wich range to cleanup on removal. **/
	size_t size;
	/** Pointer to the stat counters (the one related to the malloc call site) **/
	Stats * stats;
};

/*********************  TYPES  **********************/
/** 
 * Define the index of the std::map storing the data.
 * Depend on availability of stack information of just call site.
**/
#ifdef NUMAPROF_CALLSTACK
	typedef MiniStack StackIp;
#else
	typedef size_t StackIp;
#endif

/*******************  FUNCTION  *********************/
/**
 * Object use to track the allocation statistics. Should create one instance per thread.
**/
class MallocTracker
{
  using AllocationMatrix = AccessMatrix;
	public:
		MallocTracker(PageTable * pageTable, int nNodes);
		void onAlloc(StackIp & ip,size_t ptr, size_t size);
		void onFree(size_t ptr);
		void flush(class ProcessTracker * process);
    void setNode(int node);
	private:
    /** Pointer to the page table to know where to register the allocations **/
		PageTable * pageTable;
		/** Instruction map to store the per malloc call site counters **/
		InstrInfoMap instructions;
    /** Store the allocation counts per region */
    AllocationMatrix allocMatrix;
    /** if this is tracked by a pin thread, then store the region it belongs to. This will be -1 if not */
		int numaRegion;
};

}
