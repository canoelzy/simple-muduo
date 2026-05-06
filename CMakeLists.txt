cmake_minimum_required(VERSION 3.10)

# 项目名称（可以改成你的仓库名）
project(simple-muduo)

# C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
# if(MSVC)
#   add_compile_options(/W4 /WX)
# else()
#   add_compile_options(-Wall -Wextra -Werror -pedantic)
# endif()

# 头文件目录
include_directories(${PROJECT_SOURCE_DIR})

set(SRCS
  net/buffer.cc
)

# 源文件
set(LOG_SRCS
  log/log.cc
)

# 生成静态库
add_library(simple_muduo ${SRCS})
add_library(simple_log ${LOG_SRCS})

# 测试可执行文件
set(TEST_LIST
  bind
  timefd
  timewheel
  regex
  request
  any
  tlog
  tbuffer
)

# 循环自动生成可执行文件
foreach(test_name ${TEST_LIST})
  add_executable(test_${test_name} test/${test_name}.cc)
endforeach()

target_link_libraries(test_tlog simple_log)
target_link_libraries(test_tbuffer simple_muduo simple_log )