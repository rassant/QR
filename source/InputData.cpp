#include <filesystem>
#include <nlohmann/json.hpp>
#include "../header/InputData.hpp"
#include <iostream>
#include <memory>
#include <fstream>

using json = nlohmann::json;


// ------------------------------------------------------------------------ PHOTOGRAPHERS ------
PhotographerCollection::
PhotographerCollection (const json & input) 
{
	for (const auto & name : input.at("Photographers").items() )
	{
		for (const auto & path: name.value())
		{
			photographers_[name.key()].push_back(path);
		}
	}
}


void PhotographerCollection::
Show () const
{
	for (const auto &[name, paths]: photographers_)
	{
		std::cout << name << ": \n";
		for (const auto &path: paths)
		{
			std::cout << "[ " << path << " ]\n";
		}
		std::cout << '\n';
	}
}


/*auto PhotographerCollection::*/
/*GetPaths (const std::string & name) -> std::vector<std::filesystem::path>*/
/*{*/
/*	return photographers_.at(name);	*/
/*}*/



// ------------------------------------------------------------------------ WORKfOLDERS -------- 
WorkDirectoryManager::WorkDirectoryManager (const json & input)
{
    for (const auto &folder : input.at("Folders").items() )
    {
	if      (folder.key() == "Archive"  ) { archive_   = folder.value().get<std::string>(); }
	else if (folder.key() == "TempWork" ) { temp_work_ = folder.value().get<std::string>(); }
	else if (folder.key() == "Server"   ) { server_    = folder.value().get<std::string>(); }
	else if (folder.key() == "QRCode"   ) { qr_code_   = folder.value().get<std::string>(); }
	else if (folder.key() == "Photoshop") { photoshop_ = folder.value().get<std::string>(); }
    }
}



void WorkDirectoryManager::
Show () const{
	std::cout << "Archive"  << archive_   << '\n';
	std::cout << "TempWork" << temp_work_ << '\n';
	std::cout << "Server"   << server_    << '\n';
	std::cout << "QRCode"   << qr_code_   << '\n';
	std::cout << "Photoshop"<< photoshop_ << '\n'; 
}



// ------------------------------------------------------------------------ SERVER ----------- 
ServerConfig::ServerConfig (const json & input)
{
	for (const auto &server: input.at("Server").items() )
	{
		if (server.key() == "Port") { port_ = server.value().get<int>(); }	
		if (server.key() == "Name") { name_ = server.value().get<std::string>(); }	
	}
}


void ServerConfig::Show () const
{
	std::cout << "Name: " << name_ << '\n';
	std::cout << "Port: " << port_ << '\n';
}


ApplicationData::ApplicationData (const std::filesystem::path & input)
{
    json data;
    std::ifstream file(input);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open input file");
    }
    file >> data;

    photographers_ = std::make_shared<PhotographerCollection> (data);
	work_dirs_     = std::make_shared<WorkDirectoryManager>   (data);
	server_        = std::make_shared<ServerConfig>           (data);
}

