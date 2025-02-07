#include <iostream>
using namespace std;

int main ()
{
    std::string a = "hello man good dqy";
    std::string b = "hello dom";
    std::string c = "hello man so dqy";

    std::cout << a.compare(0, 10, b, 0, 10) << std::endl;
    std::cout << c.compare(0, 10, c, 0, 10) << std::endl; // строки равны

}
