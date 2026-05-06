#include <cstring>
#include <iostream>

#include "log/log.h"
#include "net/socket.h"
using namespace sim_muduo;

int main() {
  Socket st;
  if (!st.CreateServer(8081)) {
    ERROR("服务端启动失败");
    return -1;
  }
  std::cout << "服务端已启动，等待连接..." << std::endl;

  int fd = st.Accept();
  if (fd < 0) {
    ERROR("accept error");
    return -1;
  }
  std::cout << "客户端已连接，fd = " << fd << std::endl;

  Socket client(fd);
  char buffer[1024];
  int n = client.Recv(buffer, sizeof(buffer));
  if (n > 0) {
    buffer[n] = '\0';
    INFO("收到数据: {}", buffer);
    const char* reply = "服务器收到了你的消息\n";
    client.Send(reply, strlen(reply));
  }
  return 0;
}