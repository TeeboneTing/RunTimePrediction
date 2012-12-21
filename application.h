/*
	Application:
	a class that define what information an application have
	
	Teebone Ding
	2012 DEC 14
	Graduate Institute of Networking and Multimedia
	National Taiwan University
	Taipei, Taiwan

*/
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
class Application{
public:
	// Constructor of class Application
	Application(std::string user,std::string executable,
				std::vector<std::string> args,std::string server,std::string priority); 
	// Helper functions
	std::string getUser(){ return user_; }
	std::string getExecutable(){ return executable_; }
	std::vector<std::string> getArguments(){ return arguments_; }
	std::string getServer(){ return server_; }
	std::string getPriority(){ return priority_; }
private:
	std::string user_; //user id (uid)
	std::string executable_; // executable name
	std::vector<std::string> arguments_; // command line arguments
	std::string server_; // server name
	std::string priority_; // priority number
};

// A history application have run time information
class HistApplication: public Application{
public:
	// Constructor of class HistApplication
	HistApplication(std::string user,std::string executable,
					std::vector<std::string> args,std::string server,
					std::string priority,double runtime); 

	// Helper functions
	double getRuntime(){ return runtime_; } 
private:
	double runtime_; // execution time

	
};