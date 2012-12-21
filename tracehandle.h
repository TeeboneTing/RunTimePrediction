/*
	TraceHandle:
	a class that handle trace file I/O and test drive for Prediction class
	
	Teebone Ding
	2012 DEC 14
	Graduate Institute of Networking and Multimedia
	National Taiwan University
	Taipei, Taiwan

*/
#include <fstream>
#include "prediction.h"

class TraceHandle{
public:
	TraceHandle(char* trainfilename, char* testfilename, Prediction *pred); // Constructor of TraceHandle with input file stream argument
	void parseTrace();					// parse input log into application information data structure.
	void inputTest();					// predict application trun time from test file
	~TraceHandle();						// Destructor of TraceHandle class
private:
	std::ifstream fin_; // File input stream
	std::ifstream ftestin_; // Test file input stream
	Prediction *predict_; // A pointer to Prediction class

	std::string parseExecutable(std::string str){
		// INPUT: original string
		// OUTPUT: cut paths, reserve only executable name.
		std::stringstream ss(str+' ');
    	std::string item;
    	std::vector<std::string> elems;
    	while(std::getline(ss, item, '/'))
        	elems.push_back(item);
    	return elems.back();
	}

};