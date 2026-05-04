#include <regex>
#include <iostream>

int main() {
  // HTTP 请求行格式： GET /test/login?user=xiaoming&passwd=123123 HTTP/1.1\r\n
  // std::string url = "GET /test/login?user=xiaoming&passwd=123123 HTTP/1.1";
  std::string url = "GET /test/login?user=xiaoming&passwd=123123 HTTP/1.1\r\n";

  std::smatch matches;

  // 请求方法对匹配 GET HEAD POST PUT DELETE ......
  // std::regex e("(GET|HEAD|POST|PUT|DELETE) .*");
  // std::regex e("(GET|HEAD|POST|PUT|DELETE) ([^?]*).*");
  // std::regex e("(GET|HEAD|POST|PUT|DELETE) ([^?]*)\\?(.*) .*");
  // std::regex e("(GET|HEAD|POST|PUT|DELETE) ([^?]*)\\?(.*) (HTTP/1\\.[01])");
  std::regex e("(GET|HEAD|POST|PUT|DELETE) ([^?]*)(?:\\?(.*))? (HTTP/1\\.[01])(?:\n|\r\n)?");
  
  bool ret = std::regex_match(url, matches, e);
  if (ret == false) {
    return -1;
  }

  for (const auto& s : matches) {
    std::cout << s << std::endl;
  }

  return 0;
}