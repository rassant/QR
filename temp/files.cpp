#include <filesystem>
#include <iostream>
#include <vector>

void parsePath(const std::filesystem::path& path) {

    for (const auto& part : path) {
        std::cout << part << std::endl;
    }

}

int main() {
    std::filesystem::path my_path = "/home/user/documents/projects";
    
    std::cout << my_path.relative_path() << std::endl;
    std::cout << my_path.parent_path() << std::endl;

    return 0;
}
