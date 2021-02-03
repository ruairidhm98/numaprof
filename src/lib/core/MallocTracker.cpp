/*****************************************************
             PROJECT  : numaprof
             VERSION  : 1.1.2
             DATE     : 11/2020
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

/*******************  HEADERS  **********************/

#include <unistd.h>
#include <sys/syscall.h>

#include "MallocTracker.hpp"
#include "ProcessTracker.hpp"

#if defined(ALLOCATION_LOCALITY)

#define MPOL_F_NODE    (1<<0)   /* return next il node or node of address */
				/* Warning: MPOL_F_NODE is unsupported and
				   subject to change. Don't use. */
#define MPOL_F_ADDR     (1<<1)  /* look up vma using address */

#endif
/*******************  NAMESPACE  ********************/
namespace numaprof
{
#if defined(ALLOCATION_LOCALITY)
/*******************  FUNCTION  *********************/
/**
 * Constructor og the tracker.
 * @param pageTable Pointer to the page table to register allocation regerences.
 * @param region the region the thread that owns this resides on, determines if pinned or not
 * @param nRegions number of reegions in the topology
**/
MallocTracker::MallocTracker(PageTable * pTable, int nRegions)
  : pageTable(pTable)
  , allocMatrix(nRegions)
  , numaRegion(-1)
{}

/*******************  FUNCTION  *********************/
/**
 * Called in the constructor of ThreadTracker, to determine if pinned or not
 * @param region the malloc tracker resides on (associated with threadTracker) 
**/ 
void MallocTracker::setNumaRegion(int region)
{
  numaRegion = region;
}

/*******************  FUNCTION  *********************/
/**
 * Return a const reference to the allocation matrix so it can be converted to JSON
**/ 
AccessMatrix const& MallocTracker::getAllocMatrix() const
{
  return allocMatrix;
}
#else
/*******************  FUNCTION  *********************/
/**
 * Constructor of the tracker.
 * @param pageTable Pointer to the page table to register allocation regerences.
**/
MallocTracker::MallocTracker(PageTable * pTable)
  : pageTable(pTable)
{}
#endif
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

#if defined(ALLOCATION_LOCALITY)
	//reg to page table
	pageTable->regAllocPointer(ptr,size,infos);
  // Record the allocation into the matrix
  Page& page = pageTable->getPage(ptr);
  if (page.numaNode >= 0)
  {
    allocMatrix.access(numaRegion, page.numaNode);
  }
  // Else, we syscall to locate the page
  else
  {
    int numaNode;
		syscall(__NR_get_mempolicy, &numaNode, NULL, 0, (void*)ptr, MPOL_F_NODE | MPOL_F_ADDR);
    allocMatrix.access(numaRegion, numaNode);
  }
#endif
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
