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



#ifndef DEFAULT_HARNESS_TESTS_HPP
#define DEFAULT_HARNESS_TESTS_HPP

#ifndef _REENTRANT
#define _REENTRANT		/* basic 3-lines for threads */
#endif

#include "Harness.hpp"
#include "RAllocator.hpp"
#include "RMap.hpp"
#include "RContainer.hpp"
#include <vector>
#include <list>

class InsertRemoveTest : public Test{
public:
	RContainer* q;
	void init(GlobalTestConfig* gtc);
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc);
	void cleanup(GlobalTestConfig* gtc);
};


class NothingTest : public Test{
public:
	Rideable* r;
	void init(GlobalTestConfig* gtc){r = gtc->allocRideable();}
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc){return 0;}
	void cleanup(GlobalTestConfig* gtc){}
};



class FAITest :  public Test{
public:
	unsigned long int fai_cntr;
	void init(GlobalTestConfig* gtc);
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc);
	void cleanup(GlobalTestConfig* gtc){}
};


class NearEmptyTest :  public Test{
public:
	RContainer* q;
	void init(GlobalTestConfig* gtc);
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc);
	void cleanup(GlobalTestConfig* gtc){}
};

class AllocatorChurnTest :  public Test{
public:
	RAllocator* bp;
	std::vector<std::list<void*>*> leftovers;
	void init(GlobalTestConfig* gtc);
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc);
	void cleanup(GlobalTestConfig* gtc);
};

class SequentialTest : public Test{
public:

	virtual int execute(GlobalTestConfig* gtc)=0;
	virtual void init(GlobalTestConfig* gtc)=0;
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc);
	virtual void cleanup(GlobalTestConfig* gtc)=0;

};

class SequentialUnitTest : public SequentialTest{
public:

	bool passed=true;


	virtual int execute(GlobalTestConfig* gtc)=0;
	virtual void init(GlobalTestConfig* gtc)=0;
	void cleanup(GlobalTestConfig* gtc);
	virtual void clean(GlobalTestConfig* gtc)=0;
	void verify(bool stmt);

};


class UIDGenerator{
	
	int taskNum;
	int tidBits;
	uint32_t inc;

public:
	UIDGenerator(int taskNum);
	uint32_t initial(int tid);
	uint32_t next(uint32_t prev, int tid);

	uint32_t count(uint32_t val);
	uint32_t id(uint32_t val);


};

class QueueVerificationTest : public Test{
public:
	RContainer* q;
	std::atomic<bool> passed;
	UIDGenerator* ug;

	void init(GlobalTestConfig* gtc);
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc);
	void cleanup(GlobalTestConfig* gtc);
};

class StackVerificationTest : public Test{
public:
	RStack* q;
	std::atomic<bool> passed;
	std::atomic<bool> done;
	UIDGenerator* ug;
	pthread_barrier_t pthread_barrier;
	int opsPerPhase;
	std::atomic<int> phaseCount;


	void barrier();
	void init(GlobalTestConfig* gtc);
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc);
	void cleanup(GlobalTestConfig* gtc){}
};


class MapUnmapTest : public Test{
public:
	RMap* q;
	UIDGenerator* ug;
	void init(GlobalTestConfig* gtc);
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc);
	void cleanup(GlobalTestConfig* gtc){}
};

class MapVerificationTest : public Test{
public:
	RMap* q;
	std::atomic<bool> passed;
	UIDGenerator* ug;

	void init(GlobalTestConfig* gtc);
	int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc);
	void cleanup(GlobalTestConfig* gtc);
};





#endif
