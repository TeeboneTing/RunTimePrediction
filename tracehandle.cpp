#include "tracehandle.h"
//#include "prediction.h"
#include <iostream>
#include <string>
#include <sstream>  //for std::istringstream
#include <cmath>

// Constructor of TraceHandle with input file stream object
TraceHandle::TraceHandle(char* trainfilename, char* testfilename, Prediction *pred){
	fin_.open(trainfilename);
	ftestin_.open(testfilename);
	predict_ = pred;
}

// Destructor of TraceHandle class
TraceHandle::~TraceHandle(){
	fin_.close();
}

// parse input log into application information data structure.
void TraceHandle::parseTrace(){
	// while not the end of file...
	char line[32768];
	while(fin_.getline(line,32768)){
		// for each line, getline and parse into HistApplication class
		std::vector<std::string> arrayTokens;
		char* chars_array = strtok(line, "\t ");
		while(chars_array){
			arrayTokens.push_back(std::string(chars_array));
			chars_array = strtok(NULL, "\t ");
		}
    	
    	// Parse data into HistApplication
    	std::string user = arrayTokens[8];
    	std::string executable = parseExecutable(arrayTokens[11]);
    	std::vector<std::string>::iterator ai = arrayTokens.begin()+ 12;
    	std::vector<std::string> args(ai,arrayTokens.end());
    	std::string server = arrayTokens[9];
    	std::string priority = arrayTokens[3];
    	double runtime; std::istringstream (arrayTokens[10]) >> runtime;
    	runtime = runtime/60.0;
		
    	// Initialize a new history application.
    	HistApplication newAPP(user,executable,args,server,priority,runtime);
    	// Insert into prediction class data structure.
    	predict_ -> insertOneHistApp(newAPP);
    	
	}
}

// predict application trun time from test file
void 
TraceHandle::inputTest(){
	char line[32768];
	double errors = 0.0, avg = 0.0; int line_cnt=0;
	while(ftestin_.getline(line,32768)){
		// for each line, getline and parse into HistApplication class
		std::vector<std::string> arrayTokens;
		char* chars_array = strtok(line, "\t ");
		while(chars_array){
			arrayTokens.push_back(std::string(chars_array));
			chars_array = strtok(NULL, "\t ");
		}
    	
    	// Parse data into HistApplication
    	std::string user = arrayTokens[8];
    	std::string executable = parseExecutable(arrayTokens[11]);
    	std::vector<std::string>::iterator ai = arrayTokens.begin()+ 12;
    	std::vector<std::string> args(ai,arrayTokens.end());
    	std::string server = arrayTokens[9];
    	std::string priority = arrayTokens[3];
    	double runtime; std::istringstream (arrayTokens[10]) >> runtime;
    	runtime = runtime/60.0;
		
    	// Initialize a new test application.
    	Application newAPP(user,executable,args,server,priority);
    	// Insert into prediction class data structure.
    	double estimate;
    	estimate = predict_ -> predictRunTime(newAPP);
    	std::cout << "real: " << runtime << " predict: " << estimate << std::endl;
    	errors += abs(estimate - runtime); avg += runtime;
    	line_cnt ++;
	}
	avg = avg/line_cnt;  errors = errors/line_cnt;
	std::cout << "run time mean errors: " << errors << std::endl;
	std::cout << "mean run time: " << avg << std::endl;
	std::cout << "percentage of mean run time: " << 100*errors/avg << "\%" << std::endl;
}

// Main function for test drive
int main(int argc, char* argv[]){
	// Define log characters
	Characters chars;
	chars["user"] = 8;
	chars["server"] = 9;
	chars["executable"] = 11;
	//chars["arguments"] = 12;
	chars["priority"] = 2;
	
	
	// Template test cases
	Prediction predict(chars);
	predict.printCharacters();
	std::cout << std::endl;
	/*
	predict.addOneCharacter("test",9);
	predict.testDumpTempsetCateset();
	std::cout << std::endl;

	Temp newTemp;
	newTemp["user"] = 1;
	newTemp["server"] = 2;
	predict.addTemp(newTemp);

	Temp newTemp2;
	newTemp2["executable"] = 3;
	newTemp2["arguments"] = 4;
	predict.addTemp(newTemp2);
	predict.testDumpTempsetCateset();
	std::cout<< std::endl;

	// Category test cases
	Cate cat1;
	cat1.push_back("teebone");
	cat1.push_back("linux10");
	
	Cate cat2;
	cat2.push_back("roy");
	cat2.push_back("linux5");
	
	Cate cat3;
	cat3.push_back("teebone");
	cat3.push_back("linux10");
	cat3.push_back("svm-train");
	
	Cate cat4;
	cat4.push_back("oscar");
	cat4.push_back("linux15");

	predict.insertOneCate(cat1,2);
	predict.insertOneCate(cat2,2);
	predict.insertOneCate(cat3,3);
	predict.insertOneCate(cat4,4);
	predict.testDumpTempsetCateset();
	std::cout<< std::endl;
	*/

	// Trace handle test case
	// argv[1] for train file name!
	// argv[2] for test file name!
	TraceHandle th(argv[1],argv[2],&predict);
	th.parseTrace();

	// Prediction test case
	predict.constructFromHistory();
	
	th.inputTest();
	//predict.greedyTemplateSearch();
	//predict.printTemplates();
	//predict.printCategories();

	return 0;
}