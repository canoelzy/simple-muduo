#ifndef SIMPLE_MUDUO_BUFFER_H
#define SIMPLE_MUDUO_BUFFER_H

#include <cstdint>
#include <vector>

namespace sim_muduo {
constexpr uint64_t kBufferSize = 1024;

class Buffer {
 public:
  Buffer() : buffer_(kBufferSize) {}
  char* Begin();
  char* WritePosition();                // 获取当前写入起始地址
  char* ReadPosition();                 // 获取当前读取起始地址
  uint64_t TailIdleSize();              // 获取缓冲区末尾空闲空间大小--写偏移之后的空闲空间
  uint64_t HeadIdleSize();              // 获取缓冲区起始空闲空间大小--写偏移之前的空闲空间
  uint64_t ReadAbleSize();              // 获取可读数据大小
  void MoveReadOffset(uint64_t len);    // 将读偏移向后移动
  void MoveWriteOffset(uint64_t len);   // 将写偏移向后移动
  void EnsureWriteSpace(uint64_t len);  // 确保可写空间足够(整体空间够了就移动数据，否则就扩容)
  void Append(const char* data, uint64_t len);  // 写入数据
  void Retrieve(void* buf, uint64_t len);       // 读取数据
  const char* Peek();                           // 查看缓冲区内的数据
  void Clear();                                 // 清空缓冲区
 private:
  std::vector<char> buffer_;
  uint64_t read_index_{0};
  uint64_t write_index_{0};
};
}  // namespace sim_muduo

#endif