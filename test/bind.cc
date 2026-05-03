#include <functional>
#include <iostream>
#include <string>

void print(const std::string& str, int num) { std::cout << str << num << std::endl; }

int main() {
  using Task = std::function<void()>;
  std::vector<Task> array;
  array.push_back(std::bind(print, "hello", 10));
  array.push_back(std::bind(print, "hello!", 20));
  array.push_back(std::bind(print, "hello!!", 30));
  array.push_back(std::bind(print, "hello!!!", 40));

  for (const auto& e : array) {
    e();
  }
  // print("hello");
  // auto func = std::bind(print, "hello", std::placeholders::_1);
  // func(10);
  // func(20);
  return 0;
}