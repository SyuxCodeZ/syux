#include <iostream>
#include <string>

struct Point {
public:
  int x = 0;
  int y = 0;
  Point() = default;
};

class Person {
public:
  std::string name = "unknown";
  int age = 0;
  Person(auto n, auto a){
name = n;
age = a;
std::cout << "Person constructed" << std::endl;
  }
  ~Person(){
std::cout << "Person destructed" << std::endl;
  }
};

auto add(auto a, auto b){
return (a + b);
}

int main(){
std::cout.setf(std::ios::boolalpha);
int sum = add(4, 5);
std::cout << sum << std::endl;
int i = 0;
while ((i < 3)) {
std::cout << i << std::endl;
i = (i + 1);
}
Person user("Alex", 16);
Point p{};
std::cout << "End of main is next; destructors run on scope exit" << std::endl;
std::cout << "No malloc/free needed: stack objects + ctor/dtor" << std::endl;
return 0;
}
