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



#include "TestConfig.hpp"
#include "ParallelLaunch.hpp"
#include "DefaultHarnessTests.hpp"
#include "RContainer.hpp"
#include "SGLQueue.hpp"

#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <sstream>

using namespace std;

Rideable* GlobalTestConfig::allocRideable(){
	Rideable* r = rideableFactories[rideableType]->build(this);
	allocatedRideables.push_back(r);
	return r;
}

void GlobalTestConfig::printargdef(){
	int i;
	fprintf(stderr, "usage: %s [-m <test_mode>] [-r <rideable_test_object>] [-a <affinity_file>] [-i <interval>] [-p <num_procs>][-t <num_threads>] [-o <output_csv_file>] [-w <warm_up_MBs>] [-d <env_variable>=<value>] [-z] [-v] [-h]\n", argv0);
	for(i = 0; i< rideableFactories.size(); i++){
		fprintf(stderr, "Rideable %d : %s\n",i,rideableNames[i].c_str());
	}
	for(i = 0; i< tests.size(); i++){
		fprintf(stderr, "Test Mode %d : %s\n",i,testNames[i].c_str());
	}
}

void GlobalTestConfig::parseCommandLine(int argc, char** argv){
	int c;
	argv0 = argv[0];

	// if no args, print help
	if(argc==1){
			printargdef();
			throw 0;
	}

	if(tests.size()==0){
		errexit("No test options provided.  Use GlobalTestConfig::addTestOption() to add.");
	}


	// Read command line
	while ((c = getopt (argc, argv, "d:w:o:i:t:m:a:r:vhz")) != -1){
		switch (c) {
			case 'i':
				this->interval = atoi(optarg);
			 	break;
			case 'v':
			 	this->verbose = 1;
			 	break;
			case 'w':
				this->warmup = atoi(optarg);
			 	break;
			case 't':
				this->task_num = atoi(optarg);
				break;
			case 'm':
				this->testType = atoi(optarg);
				if(testType>=tests.size()){
					fprintf(stderr, "Invalid test mode (-m) option.\n");
					printargdef();
					throw 0;
				}
				break;
			case 'r':
				this->rideableType = atoi(optarg);
				if(rideableType>=rideableFactories.size()){
					fprintf(stderr, "Invalid rideable (-r) option.\n");
					printargdef();
					throw 0;
				}
				break;
			case 'a':
				this->affinityFile = std::string(optarg);
				break;
			case 'h':
				printargdef();
				throw 0;
				break;
			case 'o':
				this->outFile = std::string(optarg);
				break;
			case 'z':
				this->timeOut = false;
				break;
			case 'd':
				string s = std::string(optarg);
				string k,v;
				std::string::size_type pos = s.find('=');
				if (pos != std::string::npos){
					k = s.substr(0, pos);
					v = s.substr(pos+1, std::string::npos);
				}
				else{
				  k = s; v = "1";
				}
				if(v=="true"){v="1";}
				if(v=="false"){v="0";}
				environment[k]=v;
				break;
	     	}			
	}
	num_procs = numCores();
	test = tests[testType];

	if(affinityFile.size()==0){
		affinityFile = "";
		affinityFile += "../cpp_harness/affinity/"+machineName()+".aff";
	}
	readAffinity();


	recorder = new Recorder(task_num);
	recorder->reportGlobalInfo("datetime",Recorder::dateTimeString());
	recorder->reportGlobalInfo("threads",task_num);
	recorder->reportGlobalInfo("cores",num_procs);
	recorder->reportGlobalInfo("rideable",getRideableName());
	recorder->reportGlobalInfo("affinity",affinityFile);
	recorder->reportGlobalInfo("test",getTestName());
	recorder->reportGlobalInfo("interval",interval);
	recorder->reportGlobalInfo("language","C++");
	recorder->reportGlobalInfo("machine",machineName());
	recorder->reportGlobalInfo("archbits",archBits());
	recorder->reportGlobalInfo("preheated(MBs)",warmup);
	recorder->reportGlobalInfo("notes","");
	recorder->addThreadField("ops",&Recorder::sumInts);
	recorder->addThreadField("ops_stddev",&Recorder::stdDevInts);
	recorder->addThreadField("ops_each",&Recorder::concat);


	string env ="";
	for(auto it = environment.cbegin(); it != environment.cend(); ++it){
		 env = env+ it->first + "=" + it->second + ":";
	}
	if(env.size()>0){env.pop_back();}
	recorder->reportGlobalInfo("environment",env);

	if(verbose && environment.size()>0){
		cout<<"Using flags:"<<endl;
		for(auto it = environment.cbegin(); it != environment.cend(); ++it){
			 std::cout << it->first << " = \"" << it->second << "\"\n";
		}
	}



	if(environment["printAffinity"]=="1"){
		cout<<"Affinity: ";
		for(int i = 0; i<task_num; i++){
			cout<<"["<<i<<":"<<affinities[i]<<"]";
			if(i!=task_num-1){cout<<',';}
		}
		cout<<endl;
	}

}


void GlobalTestConfig::buildOrderedAffinity(){
	for(int i = 0; i<task_num; i++){
		affinities[i]=i%num_procs;
	}
}
void GlobalTestConfig::buildSingleAffinity(){
	for(int i = 0; i<task_num; i++){
		affinities[i]=0;
	}
}
void GlobalTestConfig::buildEvenOddAffinity(){
	int c = 0;
	int j = 0;
	for(int i = 0; i<task_num; i++){
		j = i%num_procs;
		if(j*2<num_procs){
			c=j*2;
		}
		else{
			c=(j-num_procs/2)*2+1;
		}
		affinities[i]=c%num_procs;
	}
}

void GlobalTestConfig::buildEvenOddLoHiAffinity(){
	int c = 0;
	int j = 0;
	for(int i = 0; i<task_num; i++){
		j = i%num_procs;
		if(j<num_procs/4){
			c=j*2;
		}
		else if(j<num_procs/2){
			c=(j-num_procs/4)*2+1;
		}
		else if(j<3*num_procs/4){
			c=(j-num_procs/2)*2+num_procs/2;
		}
		else{
			c=(j-num_procs/2)*2+1;
		}
		affinities[i]=c%num_procs;
	}
}

void GlobalTestConfig::readAffinity(){

	if(( access( affinityFile.c_str(), F_OK ) == -1 )){
		cerr<<"Missing affinity file: "<<affinityFile<<endl;
		errexit("Affinity file does not exist.");
	}
	std::ifstream f(affinityFile.c_str());
	std::string input;
	if(f.bad()){
	   errexit("Unable to open affinity file.");
	}
	std::getline(f, input);

	affinities.resize(task_num);
	if(input=="ORDERED"){
		buildOrderedAffinity();
	}
	else if(input=="SINGLE"){
		buildSingleAffinity();
	}
	else if(input=="EVEN_ODDS"){
		buildEvenOddAffinity();
	}
	else if(input=="EVEN_ODDS_LOW_HI"){
		buildEvenOddLoHiAffinity();
	}
	else{
		std::istringstream ss(input);
		std::string token;
		int i = 0;
		while(std::getline(ss, token, ',') && i<task_num) {
			if(isInteger(token)){
				affinities[i]=atoi(token.c_str());
			}
			else{
				f.close();
				errexit("Affinity file contains illegal value.");
			}
			i++;
		}
		if(i!=task_num){
			f.close();
			errexit("Affinity file is not long enough for all threads.");
		}
	}

	for(int i = 0; i<task_num; i++){
		affinities[i] = affinities[i]%num_procs;
	}

	f.close();

}



GlobalTestConfig::GlobalTestConfig():
	rideableFactories(),
	rideableNames(),
	tests(),
	testNames(),
	outFile(),
	allocatedRideables(){
}

GlobalTestConfig::~GlobalTestConfig(){
	delete recorder;
	delete test;
	for(int i = 0; i< rideableFactories.size(); i++){
		delete rideableFactories[i];
	}
	for(int i = 0; i< tests.size(); i++){
		delete tests[i];
	}
}


void GlobalTestConfig::addRideableOption(RideableFactory* h, const char name[]){
	rideableFactories.push_back(h);
	string s = string(name);
	rideableNames.push_back(s);
}

void GlobalTestConfig::addTestOption(Test* t, const char name[]){
	tests.push_back(t);
	string s = string(name);
	testNames.push_back(s);
}

std::string GlobalTestConfig::getRideableName(){
	return rideableNames[this->rideableType];
}
std::string GlobalTestConfig::getTestName(){
	return testNames[this->testType];
}



void GlobalTestConfig::runTest(){
	if(warmup!=0){
		warmMemory(warmup);
	}

	parallelWork(this);

	if(outFile.size()!=0){
		recorder->outputToFile(outFile);
		if(verbose){std::cout<<"Stored test results in: "<<outFile<<std::endl;}
	}
	if(verbose){std::cout<<recorder->getCSV()<<std::endl;}
}













