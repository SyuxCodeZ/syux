#include <iostream>
#include <string>

#include <vector>

#include <stdexcept>

class User {
public:
  std::string name = "unknown";
  User(auto n) {
    name = n;
    std::cout << "User created" << std::endl;
  }
  ~User() {
    std::cout << "User destroyed" << std::endl;
  }
};

auto sumArray(auto arr) {
  int total = 0;
  for (auto x : arr) {
    total = (total + x);
  }
  return total;
}

int main() {
  std::cout.setf(std::ios::boolalpha);
  std::cout << "=== Syux V12 Demo ===" << std::endl;
  std::vector<int> nums = {5, 10, 15, 20};
  std::cout << "Array length:" << std::endl;
  std::cout << static_cast<int>(nums.size()) << std::endl;
  std::cout << "First element:" << std::endl;
  std::cout << ([&](){ int __idx = 0; if(__idx >= 0 && __idx < static_cast<int>(nums.size())) return nums[__idx]; throw std::runtime_error("Index out of bounds"); }()) << std::endl;
  do {
    int __idx = 1;
    if(__idx < 0 || __idx >= static_cast<int>(nums.size())) throw std::runtime_error("Index out of bounds");
    nums[__idx] = 99;
  } while(false);
  std::cout << "Modified second element:" << std::endl;
  std::cout << ([&](){ int __idx = 1; if(__idx >= 0 && __idx < static_cast<int>(nums.size())) return nums[__idx]; throw std::runtime_error("Index out of bounds"); }()) << std::endl;
  std::cout << "Classic for loop:" << std::endl;
  for (int i = 0; (i < static_cast<int>(nums.size())); i++) {
    std::cout << ([&](){ int __idx = i; if(__idx >= 0 && __idx < static_cast<int>(nums.size())) return nums[__idx]; throw std::runtime_error("Index out of bounds"); }()) << std::endl;
  }
  std::cout << "For-each loop:" << std::endl;
  for (int x : nums) {
    std::cout << x << std::endl;
  }
  auto total = sumArray(nums);
  std::cout << "Sum of array:" << std::endl;
  std::cout << total << std::endl;
  auto big = (total > 50);
  if (big) {
    std::cout << "Total is greater than 50" << std::endl;
  } else {
    std::cout << "Total is 50 or less" << std::endl;
  }
  std::string name = "guest";
  std::cout << "Enter your name:" << std::endl;
  std::getline(std::cin, name);
  std::cout << "Hello," << std::endl;
  std::cout << name << std::endl;
  User u(name);
  std::cout << "End of program" << std::endl;
  return 0;
}
