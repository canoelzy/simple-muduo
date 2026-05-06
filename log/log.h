#ifndef SIMPLE_MUDUO_LOG_H
#define SIMPLE_MUDUO_LOG_H

#include <cstdio>
#include <format>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

namespace sim_muduo {

// ============================================================================
// 日志输出策略（Strategy Pattern）
// ============================================================================

// 日志输出策略抽象接口。
class LogStrategy {
 public:
  virtual ~LogStrategy() = default;

  /**
   * @brief 同步写出一条完整的日志消息。
   * @param message 已格式化的完整日志行（含头部和正文），不含换行
   *
   * 实现类需自行保证线程安全，调用者不会额外加锁。
   */
  virtual void SyncLog(const std::string& message) = 0;
};

// ============================================================================
// 控制台策略
// ============================================================================

// 将日志输出到 stdout（printf），内部互斥保证多线程安全。
class ConsoleLogStrategy : public LogStrategy {
 public:
  void SyncLog(const std::string& message) override {
    std::lock_guard lock(mutex_);
    printf("%s\n", message.c_str());
  }

 private:
  std::mutex mutex_;
};

constexpr auto kLogPath = "./logs";
constexpr auto kLogFile = "muduo.log";

// ============================================================================
// 文件策略
// ============================================================================

// 将日志以追加模式输出到指定文件。
class FileLogStrategy : public LogStrategy {
 public:
  // 构造文件策略，必要时创建日志目录。
  // path 日志目录路径，默认 "./logs"
  // file 日志文件名，默认 "muduo.log"
  FileLogStrategy(std::string path = kLogPath, std::string file = kLogFile);

  void SyncLog(const std::string& message) override;

 private:
  std::string path_;
  std::string file_;
  /// 惰性打开的日志文件句柄，首次调用 SyncLog 时打开，对象析构时关闭
  std::ofstream file_stream_;
  std::mutex mutex_;
};

// ============================================================================
// 日志等级
// ============================================================================

/// 日志等级，按严重程度递增排列，FATAL 为最高级别
enum class LogLevel {
  LOG_DEBUG,    ///< 调试信息，仅开发阶段使用
  LOG_INFO,     ///< 一般运行时信息
  LOG_WARNING,  ///< 警告，不影响正常运行但需关注
  LOG_ERROR,    ///< 错误，功能受损但进程仍可继续
  LOG_FATAL     ///< 致命错误，通常意味着进程即将退出
};

// ============================================================================
// Logger — 日志入口
// ============================================================================

// 策略配置（可选）:
//   ENABLE_CONSOLE_LOG_STRATEGY();   // 默认已启用
//   ENABLE_FILE_LOG_STRATEGY();      // 切换为文件输出
// 写日志:
//   INFO("listening on {}:{}", host, port);
//   ERROR("failed to open {}: {}", filename, errmsg);
class Logger {
 public:
  /// Meyer's Singleton：惰性构造，首次调用时初始化，C++11 保证线程安全
  static Logger& Init() {
    static Logger instance;
    return instance;
  }

  /// 切换到文件输出策略（覆盖当前策略，不保留旧策略）
  void EnableFileLogStrategy() { flush_strategy_ = std::make_unique<FileLogStrategy>(); }

  /// 切换到控制台输出策略（覆盖当前策略，默认行为）
  void EnableConsoleLogStrategy() { flush_strategy_ = std::make_unique<ConsoleLogStrategy>(); }

  // =========================================================================
  // LogMessage — RAII 日志消息
  // =========================================================================

  // 代表一条日志消息，构造时一次性完成所有格式化，析构时自动刷出。
  class LogMessage {
   public:
    // 构造日志消息，头部与正文一次性格式化完成。
    // level       日志等级
    // src_name    源文件名（由 __FILE__ 宏提供）
    // line_number 源文件行号（由 __LINE__ 宏提供）
    // logger      关联的 Logger 实例，析构时通过它刷出
    // body        已格式化的日志正文（由宏内 std::format 生成）
    LogMessage(const LogLevel level, const std::string& src_name, const int line_number,
               Logger& logger, std::string body);

    ~LogMessage();

   private:
    std::string loginfo_;  ///< 完整日志内容（头部 + 正文）
    Logger& logger_;       ///< 关联的 Logger，析构时通过它刷出
  };

  /**
   * @brief 创建一条日志消息。
   * @param level 日志等级
   * @param name  源文件名
   * @param line  源文件行号
   * @param body  已格式化的日志正文
   * @return LogMessage 临时对象，表达式结束后自动析构并刷出
   */
  LogMessage operator()(const LogLevel level, const std::string& name, const int line,
                        std::string body) {
    return LogMessage(level, name, line, *this, std::move(body));
  }

 private:
  Logger() { EnableConsoleLogStrategy(); }

  std::unique_ptr<LogStrategy> flush_strategy_;  ///< 当前使用的输出策略
};

#define LOG_IMPL(level, ...) Init()(level, __FILE__, __LINE__, std::format(__VA_ARGS__))

#define DEBUG(...) Logger::LOG_IMPL(sim_muduo::LogLevel::LOG_DEBUG, __VA_ARGS__)
#define INFO(...) Logger::LOG_IMPL(sim_muduo::LogLevel::LOG_INFO, __VA_ARGS__)
#define WARNING(...) Logger::LOG_IMPL(sim_muduo::LogLevel::LOG_WARNING, __VA_ARGS__)
#define ERROR(...) Logger::LOG_IMPL(sim_muduo::LogLevel::LOG_ERROR, __VA_ARGS__)
#define FATAL(...) Logger::LOG_IMPL(sim_muduo::LogLevel::LOG_FATAL, __VA_ARGS__)

}  // namespace sim_muduo

#endif