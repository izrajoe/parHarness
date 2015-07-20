/*

Copyright 2015 University of Rochester

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 

*/



#ifndef HAZARD_TRACKER_HPP
#define HAZARD_TRACKER_HPP

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <queue>
#include <list>
#include <vector>
#include <atomic>
#include "ConcurrentPrimitives.hpp"
#include "RAllocator.hpp"

class HazardTracker{
private:
	int task_num;
	int slotsPerThread;
	int freq;
	bool collect;

	RAllocator* mem;

	paddedAtomic<void*>* slots;
	padded<int>* cntrs;
	padded<std::list<void*>>* retired; // TODO: use different structure to prevent malloc locking....

public:
	~HazardTracker(){};
	HazardTracker(int task_num, RAllocator* mem, int slotsPerThread, int emptyFreq, bool collect);
	HazardTracker(int task_num, RAllocator* mem, int slotsPerThread, int emptyFreq);

	void reserve(void* ptr, int slot, int tid);
	void clearSlot(int slot, int tid);
	void clearAll(int tid);

	void retire(void* ptr, int tid);
	void empty(int tid);

	bool collecting(){return collect;}
	
};


#endif
