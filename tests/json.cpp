#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

auto main() -> int {
    json j = {
        {"name", "John"},
        {"age", 30}
    };
    json j2 = {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {"answer", {
            {"everything", 42}
        }},
        {"list", {1, 0, 2}},
        {"object", {
            {"currency", "USD"},
            {"value", 42.99}
        }}
    };
    std::cout << j.dump(4) << '\n';  // Форматированный вывод JSON         
    /*std::cout << j2.dump(4) << '\n';;*/

    /*std::cout << j2.is_number() << '\n';*/

for (json::iterator it = j.begin(); it != j.end(); ++it) {
  std::cout << it.key() << " : " << it.value() << "\n";
}


    return 0;
}


