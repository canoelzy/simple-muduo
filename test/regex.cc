#include <iostream>
#include <regex>

int main() {
  std::string str = "/numbers/1234";

  std::regex e("/numbers/(\\d+)");
  std::smatch matches;
  bool ret = std::regex_match(str, matches, e);
  if (ret == false) {
    return -1;
  }
  for (auto& s : matches) {
    std::cout << s << "\n";
  }
  return 0;
}