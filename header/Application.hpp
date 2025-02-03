#pragma once
#include <memory>
#include <string>
#include "./InputData.hpp"


class Application 
{
private:
	std::string json_path_;
	int threads_;
	std::shared_ptr<ApplicationData> app_data_;

	static void CopyPhotographerFlash (const std::shared_ptr<PhotographerCollection> & flash_paths);

public:
	explicit Application(const std::string & json_path);
	Application (int, char **);

	/*void ParseArguments ( ); */
	/*void ProcessUserRequest ( );*/
	void Run ( ) ;
	static void PrintUsage ( );
};


