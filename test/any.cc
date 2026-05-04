#include <any>
#include <cassert>
#include <iostream>

class Any {
 public:
  Any() : content_(nullptr) {}
  template <typename T>
  Any(const T& val) : content_(new placeholder<T>(val)) {}
  Any(const Any& other) : content_(other.content_ ? other.content_->clone() : nullptr) {}
  ~Any() { delete content_; }

  Any& swap(Any& other) {
    std::swap(content_, other.content_);
    return *this;
  }

  // 返回子类对象保存的数据的指针
  template <typename T>
  T* get() {
    assert(typeid(T) != content_->type());
    return &reinterpret_cast<placeholder<T>*>(content_)->val_;
  }

  template <typename T>
  Any& operator=(const T& val) {
    // 为val构造一个临时的通用容器，然后与当前容器自身进行指针交换，临时对象释放的时候，原先保存的数据也就被释放了
    Any(val).swap(*this);
    return *this;
  }

  Any& operator=(const Any& other) {
    Any(other).swap(*this);
    return *this;
  }

 private:
  class holder {
   public:
    virtual ~holder() {}
    virtual const std::type_info& type() = 0;
    virtual holder* clone() = 0;
  };
  template <typename T>
  class placeholder : public holder {
   public:
    placeholder(const T& val) : val_(val) {}
    // 获取子类对象保存的数据类型
    virtual const std::type_info& type() { return typeid(T); }
    // 针对当前的对象自身，克隆出一个新的子类对象
    virtual holder* clone() { return new placeholder(val_); }
    T val_;
  };
  holder* content_;
};

int main() {
  std::any a;
  a = 10;
  int* pa = std::any_cast<int>(&a);
  std::cout << *pa << std::endl;
  // Any a;
  // a = 10;
  // int* pa = a.get<int>();
  // std::cout << *pa << std::endl;
  return 0;
}