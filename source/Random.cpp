#include <random>
#include <string>
#include <chrono>
#include "../header/Random.hpp"


using namespace std;


auto GenerateRandomWord(size_t length) -> string {
    mt19937 generator(static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count()));
    
    string query;
    query.resize (length);


    for (char& character : query) {
        const int random_letters = 26; 
        const int rnd = uniform_int_distribution(0, random_letters - 1)(generator);
        character = static_cast<char>('a' + rnd); 
    }
    return query;
}
