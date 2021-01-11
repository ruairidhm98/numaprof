/*****************************************************
             PROJECT  : numaprof
             VERSION  : 1.1.2
             DATE     : 11/2020
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

/*******************  HEADERS  **********************/
#include "MallocTracker.hpp"
#include "ProcessTracker.hpp"

/*******************  NAMESPACE  ********************/
namespace numaprof
{

/*******************  FUNCTION  *********************/
/**
 * Constructor og the tracker.
 * @param pageTable Pointer to the page table to register allocation regerences.
 * @param nNodes number of nodes in the topology. Used to instantiate allocMatrix
**/
MallocTracker::MallocTracker(PageTable * pT, int nNodes)
  : pageTable(pT)
  , allocMatrix(nNodes)
  , numaRegion(-1)
{}

/*******************  FUNCTION  *********************/
/**
 * To be called in the constructor of ThreadTracker. The passed in parameter
 * will be the numa region the MallocTracker is on, provided that the thread
 * is pinned. This avoids unnecessary syscalls to discover the location of the
 * thread making the allocation. 
 * @param node the reegion the malloc tracker resides on
 *
**/  
void MallocTracker::setNode(int node)
{
  numaRegion = node;
}

/*******************  FUNCTION  *********************/
/**
 * To be called by thread to register the allocation into the page table
 * and point the instruction site corresponding the malloc site.
 * Remark, we store an instruction map on every thread, this increase
 * the memory usage but avoid locking. The instructino map are
 * merged into the process tracker at the end of the execution.
 * @param ip Instruction of call stack defining the malloc call site.
 * @param ptr Address of the allocated segement
 * @param size size of the allocated segement.
**/
void MallocTracker::onAlloc(StackIp & ip,size_t ptr, size_t size)
{
	//printf("On alloc : (%p) %p => %lu\n",(void*)ip,(void*)ptr,size);
	//get mini stack

	//allocate info
	MallocInfos * infos = new MallocInfos;
	infos->ptr = ptr;
	infos->size = size;
	infos->stats = &(instructions[ip]);

	//debug
	#ifdef NUMAPROF_TRACE_ALLOCS
		printf("TRACE: tracker.onAlloc(ip,%p,%lu);\n",(void*)ptr,size);
	#endif
	
	//reg to page table
	pageTable->regAllocPointer(ptr,size,infos);

  Page& page = pageTable->getPage(ptr);
  int pageNode = page.numaNode;
  Helper::updateMatrix(allocMatrix, numaRegion, pageNode);
}

/*******************  FUNCTION  *********************/
/**
 * To be called when allocated segments are freed by the application.
 * It unregister the segement from the page table and free the temporary
 * structure.
 * @param ptr Pointer of the allocation.
**/
void MallocTracker::onFree(size_t ptr)
{
	//trivial
	if (ptr == 0)
		return;

	//get infos
	Page & page = pageTable->getPage(ptr);
	MallocInfos * infos = (MallocInfos *)page.getAllocPointer(ptr);

	//not ok
	if (infos == NULL || infos->ptr != ptr)
		return;

	//debug
	#ifdef NUMAPROF_TRACE_ALLOCS
		printf("TRACE: tracker.onFree(%p);\n",(void*)ptr);
	#endif
	
	//free into page table
	pageTable->freeAllocPointer(ptr,infos->size,infos);

	//free mem
	delete infos;
}

/*******************  FUNCTION  *********************/
/**
 * At the end of the program execution dump the per thread
 * counters into the global process one.
**/
void MallocTracker::flush(class ProcessTracker * process)
{
	process->mergeAllocInstruction(instructions);
}

}
