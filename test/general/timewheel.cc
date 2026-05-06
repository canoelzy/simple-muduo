#include <unistd.h>

#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>

using TaskFunc = std::function<void()>;
using ReleaseFunc = std::function<void()>;

class TimerTask {
 public:
  TimerTask(uint64_t id, uint32_t delay, const TaskFunc& cb)
      : id_(id), timeout_(delay), task_cb_(cb), canceled_(false) {}

  ~TimerTask() {
    if (canceled_ == false) {
      task_cb_();
    }
    release_();
  }

  void SetRelease(const ReleaseFunc& cb) { release_ = cb; }
  uint32_t DelayTime() { return timeout_; }
  void Cancel() { canceled_ = true; }

 private:
  uint64_t id_;          // 定时器任务对象ID
  uint32_t timeout_;     // 定时任务的超时时间
  bool canceled_;        // 任务是否取消
  TaskFunc task_cb_;     // 定时器对象要执行的定时任务
  ReleaseFunc release_;  // 用于删除TimerWheel中保存的定时器对象信息
};

class TimerWheel {
 public:
  TimerWheel() : tick_(0), capacity_(60), wheel_(capacity_) {}

  // 添加定时任务
  void TimerAdd(uint64_t id, uint32_t delay, const TaskFunc& cb) {
    PtrTask pt(new TimerTask(id, delay, cb));
    pt->SetRelease(std::bind(&TimerWheel::RemoveTimer, this, id));
    timers_[id] = WeakTask(pt);
    int pos = (tick_ + delay) % capacity_;
    wheel_[pos].push_back(pt);
  }

  // 刷新/延迟定时任务
  void TimerRefresh(uint64_t id) {
    // 通过保存的定时器对象weak_ptr构造一个shared_ptr出来，添加到轮子中
    auto it = timers_.find(id);
    if (it == timers_.end()) {
      return;
    }
    PtrTask pt = it->second.lock();  // 获取weak_ptr管理的对象对应的shared_ptr
    int delay = pt->DelayTime();
    int pos = (tick_ + delay) % capacity_;
    wheel_[pos].push_back(pt);
  }

  // 这个函数每秒钟执行一次，相当于秒针往后走一步
  void RunTimerTask() {
    tick_ = (tick_ + 1) % capacity_;
    // 清空指定位置的数组，就会把数组中保存的所有管理定时器对象的shared_ptr释放掉
    wheel_[tick_].clear();
  }

  void TimerCancel(uint64_t id) {
    auto it = timers_.find(id);
    if (it != timers_.end()) {
      PtrTask pt = it->second.lock();
      if (pt) {
        pt->Cancel();
      }
    }
  }

 private:
  void RemoveTimer(uint64_t id) {
    auto it = timers_.find(id);
    if (it != timers_.end()) {
      timers_.erase(id);
    }
  }

 private:
  using WeakTask = std::weak_ptr<TimerTask>;
  using PtrTask = std::shared_ptr<TimerTask>;
  int tick_;      // 当前的秒针，走到哪释放到哪，释放哪里，就相当于执行哪里的任务
  int capacity_;  // 表盘最大数量--其实就是最大延迟时间
  std::vector<std::vector<PtrTask>> wheel_;
  std::unordered_map<uint64_t, WeakTask> timers_;
};

class Test {
 public:
  Test() { std::cout << "构造" << std::endl; }
  ~Test() { std::cout << "析构" << std::endl; }
};

void DelTest(Test* t) { delete t; }

int main() {
  TimerWheel tw;
  Test* t = new Test();
  tw.TimerAdd(888, 5, std::bind(DelTest, t));

  for (int i = 0; i < 5; ++i) {
    sleep(1);
    tw.TimerRefresh(888);
    tw.RunTimerTask();
    std::cout << "刷新了一下定时任务，重新需要5s中后才会销毁\n";
  }

  while (1) {
    sleep(1);
    std::cout << "-----------------\n";
    tw.RunTimerTask();
  }

  return 0;
}