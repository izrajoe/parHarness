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



#ifndef TEST_CONFIG_HPP
#define TEST_CONFIG_HPP

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <string.h>
#include <vector>
#include <map>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>
#include <math.h>
#include <assert.h>
#include <malloc.h>
#include <sys/time.h>	
#include <sys/resource.h>


#include "Rideable.hpp"
#include "Recorder.hpp"

// forward declarations to resolve circular dependencies
class Rideable;
class RideableFactory;

// affinity constants
//enum {SINGLE,ORDERED,EVEN_ODDS_LOW_HI,EVEN_ODDS,AFFINITY_NUM};
//static const char* affinity_name[] = {"SINGLE", "ORDERED","EVEN_ODDS_LOW_HI","EVEN_ODDS"};

class Test;

// for command line parsing
extern char *optarg;
// global test configuration, holds all command line arguments
class GlobalTestConfig{
public:
	int task_num = 4;  // number of threads
	struct timeval start, finish; // timing structures
	long unsigned int interval = 2;  // number of seconds to run test

	std::string affinityFile; // affinity file name
	std::vector<int> affinities; // map from tid to CPU id
	int num_procs=24;
	Test* test=NULL;
	int testType=0;
	int rideableType=0;
	int verbose=0;
	int warmup = 3; // MBs of data to warm
	bool timeOut = true; // whether to abort on infinte loop

	Recorder* recorder = NULL;
	std::vector<RideableFactory*> rideableFactories;
	std::vector<std::string> rideableNames;
	std::vector<Test*> tests;
	std::vector<std::string> testNames;
	std::string outFile;
	std::map<std::string,std::string> environment;
	std::vector<Rideable*> allocatedRideables;

	long int total_operations=0;


	
	GlobalTestConfig();
	~GlobalTestConfig();

	// for tests to access rideable objects
	Rideable* allocRideable();


	// for configuration set up
	void parseCommandLine(int argc, char** argv);
	std::string getRideableName();
	void addRideableOption(RideableFactory* r, const char name[]);
	std::string getTestName();
	void addTestOption(Test* t, const char name[]);
	void buildOrderedAffinity();
	void buildEvenOddAffinity();
	void buildSingleAffinity();
	void buildEvenOddLoHiAffinity();
	void readAffinity();


	// Run the test
	void runTest();

private:
	void createTest();
	char* argv0;
	void printargdef();

};

// local test configuration, one per thread
class LocalTestConfig{
public:
	int tid;
	unsigned int seed;
	int cpu;
};


class CombinedTestConfig{
public:
	GlobalTestConfig* gtc;
	LocalTestConfig* ltc;
};

class Test{
public:
	// called by one (master) thread
	virtual void init(GlobalTestConfig* gtc)=0;
	virtual void cleanup(GlobalTestConfig* gtc)=0;

	// called by all threads in parallel
	// runs the test
	// returns number of operations completed by that thread
	virtual int execute(GlobalTestConfig* gtc, LocalTestConfig* ltc)=0;
};

#endif
