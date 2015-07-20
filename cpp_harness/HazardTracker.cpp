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



#include "HazardTracker.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>   

using namespace std;


HazardTracker::HazardTracker(int task_num, RAllocator* mem, int slotsPerThread, int emptyFreq, bool collect){
	this->task_num = task_num;
	this->slotsPerThread = slotsPerThread;
	this->freq = emptyFreq;
	this->mem = mem;
	slots = new paddedAtomic<void*>[task_num*slotsPerThread];
	for (int i = 0; i<task_num*slotsPerThread; i++){
		slots[i]=NULL;
	}
	retired = new padded<list<void*>>[task_num];
	cntrs = new padded<int>[task_num];
	for (int i = 0; i<task_num; i++){
		cntrs[i]=0;
		retired[i].ui = list<void*>();
	}
	this->collect = collect;
}

HazardTracker::HazardTracker(int task_num, RAllocator* mem, int slotsPerThread, int emptyFreq){
	this->task_num = task_num;
	this->slotsPerThread = slotsPerThread;
	this->freq = emptyFreq;
	this->mem = mem;
	slots = new paddedAtomic<void*>[task_num*slotsPerThread];
	for (int i = 0; i<task_num*slotsPerThread; i++){
		slots[i]=NULL;
	}
	retired = new padded<list<void*>>[task_num];
	cntrs = new padded<int>[task_num];
	for (int i = 0; i<task_num; i++){
		retired[i].ui = list<void*>();
		cntrs[i]=0;
	}
	this->collect = true;
}

void HazardTracker::reserve(void* ptr, int slot, int tid){
	slots[tid*slotsPerThread+slot] = ptr;
}
void HazardTracker::clearSlot(int slot, int tid){
	slots[tid*slotsPerThread+slot] = NULL;
}
void HazardTracker::clearAll(int tid){
	for(int i = 0; i<slotsPerThread; i++){
		slots[tid*slotsPerThread+i] = NULL;
	}
}

void HazardTracker::retire(void* ptr, int tid){
	if(ptr==NULL){return;}
	list<void*>* myTrash = &(retired[tid].ui);
	assert(find(myTrash->begin(), myTrash->end(), ptr)==myTrash->end());  
	myTrash->push_back(ptr);	
	if(collect && cntrs[tid]==freq){
		cntrs[tid]=0;
		empty(tid);
	}
	cntrs[tid].ui++;
}

void HazardTracker::empty(int tid){
	list<void*>* myTrash = &(retired[tid].ui);
	for (std::list<void*>::iterator iterator = myTrash->begin(), end = myTrash->end(); iterator != end; ) {
		bool danger = false;
		void* ptr = *iterator;
		for (int i = 0; i<task_num*slotsPerThread; i++){
			if(ptr == slots[i].ui){
				danger = true;
				break;
			}
		}
		if(!danger){
			mem->freeBlock(ptr,tid);
			iterator = myTrash->erase(iterator);
		}
		else{++iterator;}
	}

	return;
}

