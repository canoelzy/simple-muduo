/**
 * @file    log.cc
 * @brief   日志模块实现：文件策略、消息格式化。
 */

#include "log/log.h"

#include <unistd.h>

#include <ctime>
#include <filesystem>
#include <utility>
#include <vector>

namespace sim_muduo {

// ============================================================================
// 内部辅助函数（匿名命名空间，仅本翻译单元可见）
// ============================================================================

namespace {

// 将 LogLevel 枚举值转为可读字符串。
std::string LevelStr(LogLevel level) {
  static std::vector<std::string> levelnames = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
  const int idx = static_cast<int>(level);
  if (idx < 0 || idx >= static_cast<int>(levelnames.size())) return "UNKNOWN";
  return levelnames[idx];
}

// 获取当前本地时间戳，格式为 "YYYY-MM-DD HH:MM:SS"。
std::string GetTimeStamp() {
  const time_t now = time(nullptr);
  struct tm tm_now;
  localtime_r(&now, &tm_now);
  char time_buffer[64];
  strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &tm_now);
  return std::string{time_buffer};
}

}  // namespace

// ============================================================================
// FileLogStrategy 实现
// ============================================================================

FileLogStrategy::FileLogStrategy(std::string path, std::string file)
    : path_(std::move(path)), file_(std::move(file)) {
  std::lock_guard lock(mutex_);
  if (std::filesystem::exists(path_)) return;
  try {
    std::filesystem::create_directories(path_);
  } catch (const std::filesystem::filesystem_error& e) {
    fprintf(stderr, "%s\n", e.what());
  }
}

// 同步写入一条日志到文件。
void FileLogStrategy::SyncLog(const std::string& message) {
  std::lock_guard lock(mutex_);
  if (!file_stream_.is_open()) {
    const std::string filename = std::format("{}/{}", path_, file_);
    file_stream_.open(filename, std::ios::app);
    if (!file_stream_.is_open()) {
      fprintf(stderr, "File open failed: %s (%s:%d)", filename.c_str(), __FILE__, __LINE__);
      return;
    }
  }
  file_stream_ << message << std::endl;
}

// ============================================================================
// Logger::LogMessage 实现
// ============================================================================

// 构造日志消息，头部与正文一次性格式化完成。
Logger::LogMessage::LogMessage(const LogLevel level, const std::string& src_name,
                               const int line_number, Logger& logger, std::string body)
    : logger_(logger) {
  loginfo_ = std::format("[{}] [{}] [{}] [{}] [{}] - {}", GetTimeStamp(), LevelStr(level), getpid(),
                         src_name, line_number, std::move(body));
}

// 将完整日志消息交由 Logger 当前的 Strategy 输出。
// 如果 Logger 未设置任何策略（flush_strategy_ 为空），日志将静默丢弃。
Logger::LogMessage::~LogMessage() {
  if (logger_.flush_strategy_) logger_.flush_strategy_->SyncLog(loginfo_);
}

}  // namespace sim_muduo
