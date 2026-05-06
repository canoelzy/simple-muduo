#include <cassert>
#include <string>

#include "../log/log.h"
#include "../net/buffer.h"
using namespace sim_muduo;
int main() {
  Logger::Init().EnableConsoleLogStrategy();
  Buffer buf;
  // std::string str;
  // for (size_t i = 0; i < 300; ++i) {
  //   str = "hello" + std::to_string(i) + '\n';
  //   buf.Append(str.c_str(), str.size());
  // }
  // Logger::DEBUG("{}", buf.Peek());
  buf.Append("hello", 5);
  buf.Append("world", 5);
  assert(buf.ReadAbleSize() == 10);
  assert(std::string(buf.Peek(), 10) == "helloworld");
  void* tmp;
  buf.Retrieve(&tmp, 5);
  assert(buf.ReadAbleSize() == 5);
  return 0;
}
