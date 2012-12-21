#include "application.h"

// Constructor of class Application
Application::Application(std::string user,std::string executable,
						 std::vector<std::string> args,
						 std::string server,std::string priority){
	user_ = user;
	executable_ = executable;
	arguments_ = args;
	server_ = server;
	priority_ = priority;

}



// Constructor of class HistApplication
HistApplication::HistApplication(std::string user,
								 std::string executable,
								 std::vector<std::string> args,
								 std::string server,
								 std::string priority,
								 double runtime): 
					 Application(user,
						 		 executable,
						 		 args,
								 server,
								 priority){

	runtime_ = runtime;
}