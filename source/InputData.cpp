#include <nlohmann/json.hpp>
#include "../header/InputData.hpp"
#include <iostream>

using json = nlohmann::json;


// ------------------------------------------------------------------------ PHOTOGRAPHERS ------
Photographers::
Photographers (const json & input) 
{
	for (const auto & name : input.at("Photographers").items() )
	{
		for (const auto & path: name.value())
		{
			photographers_[name.key()].push_back(path);
		}
	}
}


void Photographers::
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


auto Photographers::
GetPaths (const std::string & name) -> std::vector<std::string>
{
	return photographers_.at(name);	
}



// ------------------------------------------------------------------------ WORKfOLDERS -------- 
WorkFolders::WorkFolders (const json & input)
{
    for (const auto &folder : input.at("Folders").items() )
    {
	if      (folder.key() == "Archive"  ) { archive_   = folder.value().get<std::string>(); }
	else if (folder.key() == "Server"   ) { server_    = folder.value().get<std::string>(); }
	else if (folder.key() == "QRCode"   ) { qr_code_   = folder.value().get<std::string>(); }
	else if (folder.key() == "Photoshop") { photoshop_ = folder.value().get<std::string>(); }
    }
}



void WorkFolders::
Show () const{
	std::cout << "Archive"  << archive_   << '\n';
	std::cout << "Server"   << server_    << '\n';
	std::cout << "QRCode"   << qr_code_   << '\n';
	std::cout << "Photoshop"<< photoshop_ << '\n'; 
}



// ------------------------------------------------------------------------ SERVER ----------- 
Server::Server (const json & input)
{
	for (const auto &server: input.at("Server").items() )
	{
		if (server.key() == "Port") { port_ = server.value().get<int>(); }	
		if (server.key() == "Name") { name_ = server.value().get<std::string>(); }	
	}
}


void Server::Show () const
{
	std::cout << "Name: " << name_ << '\n';
	std::cout << "Port: " << port_ << '\n';
}
