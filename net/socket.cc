#include "socket.h"

#include <cstring>

#include "../log/log.h"

namespace sim_muduo {
Socket::~Socket() { Close(); }
int Socket::Fd() { return sockfd_; }

// 创建套接字
bool Socket::CreateSocket() {
  sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd_ < 0) {
    ERROR("创建套接字失败");
    return false;
  }
  return true;
}
// 绑定地址信息
bool Socket::Bind(uint16_t port, const std::string& ip) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip.c_str());
  int ret = bind(sockfd_, (const struct sockaddr*)&addr, sizeof(addr));
  if (ret < 0) {
    ERROR("bind error");
    return false;
  }
  return true;
}
// 监听
bool Socket::Listen(int backlog) {
  int ret = listen(sockfd_, backlog);
  if (ret < 0) {
    ERROR("listen errror");
    return false;
  }
  return true;
}
// 向服务器发起连接
bool Socket::Connect(uint16_t port, const std::string& ip) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip.c_str());
  int ret = connect(sockfd_, (const struct sockaddr*)&addr, sizeof(addr));
  if (ret < 0) {
    ERROR("connect error");
    return false;
  }
  return true;
}
// 获取新连接
int Socket::Accept() {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  int fd = accept(sockfd_, (struct sockaddr*)&addr, &len);
  if (fd < 0) {
    ERROR("accept error");
    return -1;
  }
  return fd;
}
// 接收数据
ssize_t Socket::Recv(void* buf, size_t len, int flag) {
  int ret = recv(sockfd_, buf, len, flag);
  if (ret <= 0) {
    if (errno == EAGAIN || errno == EINTR) {
      WARNING("没有数据");
      return 0;
    }
    ERROR("recv error");
    return -1;
  }
  return ret;
}
// 发送数据
ssize_t Socket::Send(const void* buf, size_t len, int flag) {
  int ret = send(sockfd_, buf, len, flag);
  if (ret < 0) {
    ERROR("send error");
    return -1;
  }
  return ret;
}
// 关闭套接字
void Socket::Close() {
  close(sockfd_);
  sockfd_ = -1;
}
// 创建一个服务端连接
bool Socket::CreateServer(uint16_t port, const std::string& ip) {
  if (CreateSocket() == false) return false;
  setReuseAddr();
  setReusePort();
  setTcpNoDelay();
  if (Bind(port, ip) == false) return false;
  if (Listen() == false) return false;
  DEBUG("create server success");
  return true;
}
// 创建一个客户端连接
bool Socket::CreateClient(uint16_t port, const std::string& ip) {
  if (CreateSocket() == false) return false;
  if (Connect(port, ip) == false) return false;
  return true;
}
// 设置地址复用
void Socket::setReuseAddr() {
  int optval = 1;
  setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
}
// 禁用 Nagle 算法
void Socket::setTcpNoDelay() {
  int optval = 1;
  setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval)));
}
// 设置端口复用
void Socket::setReusePort() {
  int optval = 1;
  setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval)));
}
}  // namespace sim_muduo