#include "buffer.h"

#include <cassert>

#include "../log/log.h"

namespace sim_muduo {

char* Buffer::Begin() { return &*buffer_.begin(); }
// 获取当前写入起始地址
char* Buffer::WritePosition() { return Begin() + write_index_; }
// 获取当前读取起始地址
char* Buffer::ReadPosition() { return Begin() + read_index_; }
// 获取缓冲区末尾空闲空间大小--写偏移之后的空闲空间, 总体空间-写偏移
uint64_t Buffer::TailIdleSize() { return buffer_.size() - write_index_; }
// 获取缓冲区起始空闲空间大小--写偏移之前的空闲空间
uint64_t Buffer::HeadIdleSize() { return buffer_.size() - read_index_; }
// 获取可读数据大小
uint64_t Buffer::ReadAbleSize() { return write_index_ - read_index_; }
// 将读偏移向后移动
void Buffer::MoveReadOffset(uint64_t len) { read_index_ += len; }
// 将写偏移向后移动
void Buffer::MoveWriteOffset(uint64_t len) { write_index_ += len; }
// 确保可写空间足够(整体空间够了就移动数据，否则就扩容)
void Buffer::EnsureWriteSpace(uint64_t len) {
  uint64_t writable = buffer_.size() - write_index_;
  if (writable >= len) {
    return;
  }
  if (len + write_index_ <= writable) {
    uint64_t readable = write_index_ - read_index_;
    std::copy(Begin() + read_index_, Begin() + write_index_, Begin());
    read_index_ = 0;
    write_index_ = readable;
    return;
  } else {
    buffer_.resize(write_index_ + len);
    WARNING("写入空间不足空间扩大至: {}", buffer_.size());
  }
}
// 写入数据
void Buffer::Append(const char* data, uint64_t len) {
  if (len == 0) return;
  // 确保空间足够
  EnsureWriteSpace(len);
  std::copy(data, data + len, WritePosition());
  INFO("写入数据: {}", data);
  MoveWriteOffset(len);
}
// 读取数据
void Buffer::Retrieve(void* buf, uint64_t len) {
  assert(len <= ReadAbleSize());
  std::copy(ReadPosition(), ReadPosition() + len, static_cast<char*>(buf));
  INFO("读取数据: {}", static_cast<char*>(buf));
  MoveReadOffset(len);
}
// 查看缓冲区内的数据
const char* Buffer::Peek() { return Begin() + read_index_; }
// 清空缓冲区
void Buffer::Clear() {
  read_index_ = 0;
  write_index_ = 0;
}
}  // namespace sim_muduo
