#include <iostream>
#include <string>

#include <vector>

#include <stdexcept>

int main() {
  std::cout.setf(std::ios::boolalpha);
  std::cout << "=== Syux V13 Type System Test ===" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "1. Integer:" << std::endl;
  int x = 42;
  std::cout << x << std::endl;
  std::cout << "" << std::endl;
  std::cout << "2. Float:" << std::endl;
  double pi = 3.14159;
  std::cout << pi << std::endl;
  std::cout << "" << std::endl;
  std::cout << "3. String:" << std::endl;
  std::string msg = "Hello, Syux!";
  std::cout << msg << std::endl;
  std::cout << "" << std::endl;
  std::cout << "4. Boolean (true/false):" << std::endl;
  bool flag1 = true;
  bool flag2 = false;
  std::cout << flag1 << std::endl;
  std::cout << flag2 << std::endl;
  std::cout << "" << std::endl;
  std::cout << "5. Boolean aliases (on/off):" << std::endl;
  bool active = true;
  bool inactive = false;
  std::cout << active << std::endl;
  std::cout << inactive << std::endl;
  std::cout << "" << std::endl;
  std::cout << "6. Arrays:" << std::endl;
  std::vector<int> nums = {10, 20, 30};
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
  std::cout << "" << std::endl;
  std::cout << "7. For-each loop:" << std::endl;
  for (int n : nums) {
    std::cout << n << std::endl;
  }
  std::cout << "" << std::endl;
  std::cout << "8. Classic for loop:" << std::endl;
  for (int i = 0; (i < 3); i++) {
    std::cout << ([&](){ int __idx = i; if(__idx >= 0 && __idx < static_cast<int>(nums.size())) return nums[__idx]; throw std::runtime_error("Index out of bounds"); }()) << std::endl;
  }
  std::cout << "" << std::endl;
  std::cout << "9. While loop:" << std::endl;
  int counter = 0;
  while ((counter < 3)) {
    std::cout << counter << std::endl;
    counter = (counter + 1);
  }
  std::cout << "" << std::endl;
  std::cout << "=== All V13 Tests Passed ===" << std::endl;
  return 0;
}
