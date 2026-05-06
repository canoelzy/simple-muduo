#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdint>
#include <string>
namespace sim_muduo {
constexpr int kMaxListen = 1024;
class Socket {
 public:
  Socket() = default;
  Socket(int sockfd) : sockfd_(sockfd) {}
  ~Socket();
  int Fd();

  // 创建套接字
  bool CreateSocket();
  // 绑定地址信息
  bool Bind(uint16_t port, const std::string& ip);
  // 监听
  bool Listen(int backlog = kMaxListen);
  // 向服务器发起连接
  bool Connect(uint16_t port, const std::string& ip);
  // 获取新连接
  int Accept();
  // 接收数据
  ssize_t Recv(void* buf, size_t len, int flag = 0);
  // 发送数据
  ssize_t Send(const void* buf, size_t len, int flag = 0);
  // 关闭套接字
  void Close();
  // 创建一个服务端连接
  bool CreateServer(uint16_t port, const std::string& ip = "0.0.0.0");
  // 创建一个客户端连接
  bool CreateClient(uint16_t port, const std::string& ip);
  // 设置地址复用
  void setReuseAddr();
  // 禁用 Nagle 算法
  void setTcpNoDelay();
  // 设置端口复用
  void setReusePort();

 private:
  int sockfd_ = -1;
};
}  // namespace sim_muduo