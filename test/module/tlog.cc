#include "log/log.h"
// using namespace sim_muduo;
int main() {
  sim_muduo::Logger::Init().EnableConsoleLogStrategy();
  sim_muduo::INFO("console test: {}", 42);

  sim_muduo::Logger::Init().EnableFileLogStrategy();
  sim_muduo::DEBUG("file test: {}", "hello");

  return 0;
}
