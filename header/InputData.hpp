#pragma once
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;


// ------------------------------------------------------------------------ PHOTOGRAPHERS ------
class Photographers 
{
private:
    std::unordered_map<std::string,std::vector<std::string>> photographers_;
public:
    explicit Photographers (const json & input);
    void Show () const;
    [[nodiscard]] auto GetPaths (const std::string & name) -> std::vector<std::string>;
};



// ------------------------------------------------------------------------ WORKfOLDERS -------- 
class WorkFolders 
{
private:
    std::filesystem::path archive_;
    std::filesystem::path server_;
    std::filesystem::path qr_code_;
    std::filesystem::path photoshop_;
public:
    WorkFolders() = delete;
    explicit WorkFolders (const json & input);

    [[nodiscard]] auto GetArchive   () const -> std::filesystem::path { return archive_;  };
    [[nodiscard]] auto GetServer    () const -> std::filesystem::path { return server_;   };
    [[nodiscard]] auto GetQrcode    () const -> std::filesystem::path { return qr_code_;  };
    [[nodiscard]] auto GetPhotoshop () const -> std::filesystem::path { return photoshop_;};

    void Show () const;
};



// ------------------------------------------------------------------------ SERVER ----------- 
class Server 
{
private:
    int port_;
    std::string name_;
public:
    Server () = delete;
    explicit Server (const json & input);
    void Show () const;
};
