/*****************************************************
             PROJECT  : numaprof
             VERSION  : 2.3.0
             DATE     : 05/2017
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include "ProcessTracker.hpp"
#include "ThreadTracker.hpp"
#include "../common/Helper.hpp"
#include "../portability/OS.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <fstream>

/*******************  NAMESPACE  ********************/
namespace numaprof
{

/*******************  FUNCTION  *********************/
ProcessTracker::ProcessTracker(void)
{

}

/*******************  FUNCTION  *********************/
ThreadTracker * ProcessTracker::createThreadTracker(int threadId)
{
	//lock
	mutex.lock();

	//check if exist
	ThreadTracker * ret = NULL;
	ThreadTrackerMap::iterator it = threads.find(threadId);
	if (it == threads.end())
		threads[threadId] = ret = new ThreadTracker(this);
	else
		ret = it->second;
	
	//unlock
	mutex.unlock();

	//ret
	return ret;
}

/*******************  FUNCTION  *********************/
void ProcessTracker::mergeInstruction(InstrInfoMap & stats)
{
	mutex.lock();
	for (InstrInfoMap::iterator it = stats.begin() ; it != stats.end() ; ++it)
		instructions[it->first].merge(it->second);
	mutex.unlock();
}

/*******************  FUNCTION  *********************/
void ProcessTracker::mergeAllocInstruction(InstrInfoMap & stats)
{
	mutex.lock();
	for (InstrInfoMap::iterator it = stats.begin() ; it != stats.end() ; ++it)
		allocStats[it->first].merge(it->second);
	mutex.unlock();
}

/*******************  FUNCTION  *********************/
int ProcessTracker::getNumaAffinity(void)
{
	return topo.getCurrentNumaAffinity();
}

/*******************  FUNCTION  *********************/
int ProcessTracker::getNumaAffinity(cpu_set_t * mask)
{
	return topo.getCurrentNumaAffinity(mask);
}

/*******************  FUNCTION  *********************/
NumaTopo & ProcessTracker::getNumaTopo(void)
{
	return topo;
}

/*******************  FUNCTION  *********************/
PageTable * ProcessTracker::getPageTable(void)
{
	return &pageTable;
}

/*******************  FUNCTION  *********************/
void ProcessTracker::onExit(void)
{
	//flush local data
	for (ThreadTrackerMap::iterator it = threads.begin() ; it != threads.end() ; ++it)
		it->second->flush();
	
	//extract symbols
	registry.loadProcMap();

	#ifdef NUMAPROG_CALLSTACK
		for (InstrInfoMap::iterator it = instructions.begin() ; it != instructions.end() ; ++it)
			for (int i = 0 ; i < NUMAPROG_MINI_STACk_SIZE ; i++)
				if (it->first.stack[i] != NULL)
					registry.registerAddress((void*)(it->first.stack[i]));
		for (InstrInfoMap::iterator it = allocStats.begin() ; it != allocStats.end() ; ++it)
			for (int i = 0 ; i < NUMAPROG_MINI_STACk_SIZE ; i++)
				if (it->first.stack[i] != NULL)
					registry.registerAddress((void*)(it->first.stack[i]));
	#else
		for (InstrInfoMap::iterator it = instructions.begin() ; it != instructions.end() ; ++it)
			registry.registerAddress((void*)(it->first));
		for (InstrInfoMap::iterator it = allocStats.begin() ; it != allocStats.end() ; ++it)
			registry.registerAddress((void*)(it->first));
	#endif
	registry.solveNames();

	//prep filename
	char buffer[64];
	sprintf(buffer,"numaprof-%d.json",OS::getPID());

	//open & dump
	std::ofstream out;
	out.open(buffer);
	htopml::convertToJson(out,*this,true);
	out.close();
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const ProcessTracker& value)
{
	json.openStruct();
		json.openFieldStruct("infos");
			json.printField("formatVersion",1);
			json.printField("tool","numaprof");
			json.printField("exe",OS::getExeName());
			json.printField("command",OS::getCmdLine());
			json.printField("hostname",OS::getHostname());
			json.printField("date",OS::getDateTime());
		json.closeFieldStruct("infos");
		json.printField("threads",value.threads);
		json.printField("symbols",value.registry);
		json.printField("instructions",value.instructions);
		json.printField("allocs",value.allocStats);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const ThreadTrackerMap& value)
{
	json.openArray();
		for (ThreadTrackerMap::const_iterator it = value.begin() ; it != value.end() ; ++it)
			json.printValue(*(it->second));
	json.closeArray();
}

}