#include "tracehandle.h"
//#include "prediction.h"
#include <iostream>
#include <string>
#include <sstream>  //for std::istringstream
#include <cmath>

// Constructor of TraceHandle with input file stream object
TraceHandle::TraceHandle(char* trainfilename, char* testfilename, char* wildcards, Prediction *pred){
	fin_.open(trainfilename);
	ftestin_.open(testfilename);
	fwc_.open(wildcards);
	predict_ = pred;
}

// Destructor of TraceHandle class
TraceHandle::~TraceHandle(){
	fin_.close();
}

// store wild card arguments into Prediction
void
TraceHandle::storeWildCards(){
	char line[32768];
	while(fwc_.getline(line,32768)){
		std::string wc(line);
		predict_->addOneWildCard(wc);
	}// End of while getline()
}


// parse input log into application information data structure.
void
TraceHandle::parseTrace(){
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
		std::string arguments = executable;
		for(int i=0;i<args.size();i++) { arguments = arguments + args[i] + " "; }

    	// Initialize a new history application.
    	HistApplication newAPP(user,executable,arguments,server,priority,runtime);
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
    	std::string arguments =  executable;
		for(int i=0;i<args.size();i++) { arguments = arguments + args[i] + " "; }
		
    	// Initialize a new test application.
    	Application newAPP(user,executable,arguments,server,priority);
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
	chars.push_back("user");
	chars.push_back("server");
	chars.push_back("executable");
	chars.push_back("priority");
	chars.push_back("arguments");
	
	// Template test cases
	Prediction predict(chars);
	predict.printCharacters();
	std::cout << std::endl;

	// Trace handle test case
	// argv[1] for train file name!
	// argv[2] for test file name!
	// argv[3] for wildcard command line arguments categories
	TraceHandle th(argv[1],argv[2],argv[3],&predict);
	th.parseTrace();
	th.storeWildCards();
	// Prediction test case
	//predict.greedyTemplateSearch();
	predict.constructFromHistory();
	th.inputTest();
	//predict.printTemplates();
	//predict.printCategories();


	return 0;
}