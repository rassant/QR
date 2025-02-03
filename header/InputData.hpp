#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;


// ------------------------------------------------------------------------ PHOTOGRAPHERS ------
class PhotographerCollection 
{
private:
    std::unordered_map<std::string,std::vector<std::filesystem::path>> photographers_;
public:
    PhotographerCollection () = delete;
    explicit PhotographerCollection (const json & input);
    void Show () const;
    /*[[nodiscard]] auto GetPaths (const std::string & name) -> std::vector<std::string>;*/
    [[nodiscard]] auto GetData  ( ) const -> std::unordered_map<std::string,std::vector<std::filesystem::path>> {return photographers_;}
};



// ------------------------------------------------------------------------ WORKfOLDERS -------- 
class WorkDirectoryManager 
{
private:
    std::filesystem::path archive_;
    std::filesystem::path server_;
    std::filesystem::path qr_code_;
    std::filesystem::path photoshop_;
public:
    WorkDirectoryManager() = delete;
    explicit WorkDirectoryManager (const json & input);

    [[nodiscard]] auto GetArchive   () const -> std::filesystem::path { return archive_;  };
    [[nodiscard]] auto GetServer    () const -> std::filesystem::path { return server_;   };
    [[nodiscard]] auto GetQrcode    () const -> std::filesystem::path { return qr_code_;  };
    [[nodiscard]] auto GetPhotoshop () const -> std::filesystem::path { return photoshop_;};

    void Show () const;
};



// ------------------------------------------------------------------------ SERVER ----------- 
class ServerConfig 
{
private:
    int port_;
    std::string name_;
public:
    ServerConfig () = delete;
    explicit ServerConfig (const json & input);
    void Show () const;
};



// ------------------------------------------------------------------------ DATA ----------- 
class ApplicationData {
private:
        std::shared_ptr<PhotographerCollection> photographers_;
	std::shared_ptr<WorkDirectoryManager>   work_dirs_;
	std::shared_ptr<ServerConfig>           server_;

public:
	ApplicationData () = delete;
        explicit ApplicationData(const std::filesystem::path &input);

        auto GetPhotographer () -> std::shared_ptr<PhotographerCollection> { return photographers_ ;}
	auto GetWorkDir      () -> std::shared_ptr<WorkDirectoryManager>   { return work_dirs_ ;}
	auto GetServerConfig () -> std::shared_ptr<ServerConfig>           { return server_ ;}
};

