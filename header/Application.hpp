#pragma once
#include <string>
#include <vector>


class Application 
{
private:
	int argc_;
	std::vector<std::string> argv_;

	std::string from_source_;
	std::string to_destination_;
	unsigned int second_wait_{};

public:
	Application(int argc,  char ** argv);

	void ParseArguments ( ); 
	void ProcessUserRequest ( );
	void Run ( ) ;
	static void PrintUsage ( );
};
