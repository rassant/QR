#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include "input_data.hpp"
using json = nlohmann::json;





// -----------------------------------------------------------------------------------------------------
auto main () -> int
{
    std::ifstream input_json ("./input.json");
    json input;
    input_json >> input;

    Photographers photographers (input);
    photographers.Show();

    WorkFolders work_folders (input);
    work_folders.Show();

    Server server (input);
    server.Show();

    return 0;
}



