/*****************************************************
             PROJECT  : numaprof
             VERSION  : 1.1.2
             DATE     : 11/2020
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef ACCESS_MATRIX_HPP
#define ACCESS_MATRIX_HPP

/*******************  HEADERS  **********************/
#include <cassert>
#include "../../../extern-deps/from-htopml/json/ConvertToJson.h"

/*******************  NAMESPACE  ********************/
namespace numaprof
{

/*********************  STRUCT  *********************/
/**
 * Track access from each thead NUMA node to each memory NUMA node.
**/
class AccessMatrix
{
	public:
		AccessMatrix(int numaNodes);
		~AccessMatrix(void);
		void merge(AccessMatrix & value);
		void access(int threadNumaNode,int pageNumaNode);
    void access(int threadNumaNode, int pageNumaNode, size_t size);
		friend void convertToJson(htopml::JsonState& json, const AccessMatrix& value);
	private:
		/** Matrix to store counters **/
		size_t ** matrix;
		/** Number of numa nodes **/
		int numaNodes;
};

inline void AccessMatrix::access(int threadNumaNode, int pageNumaNode, size_t size)
{
  assert(threadNumaNode >= -1 && threadNumaNode < numaNodes); 
	assert(pageNumaNode >= 0 && pageNumaNode < numaNodes); 
  matrix[threadNumaNode+1][pageNumaNode] += size;
}


/*******************  FUNCTION  *********************/
/**
 * Inrement counter for the current access from given thread NUMA node
 * to target memory NUMA node.
 * @param threadNumaNode Define the source of the access, -1 if the thread is not binded.
 * @param pageNumaNode Define the NUMA node of the accesses memory page.
**/
inline void AccessMatrix::access(int threadNumaNode,int pageNumaNode) 
{
	assert(threadNumaNode >= -1 && threadNumaNode < numaNodes); 
	assert(pageNumaNode >= 0 && pageNumaNode < numaNodes); 
	matrix[threadNumaNode+1][pageNumaNode]++;
}

}

#endif //ACCESS_MATRIX_HPP
