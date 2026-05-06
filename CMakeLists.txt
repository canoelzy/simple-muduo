cmake_minimum_required(VERSION 3.10)

project(simple-muduo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

# ==================================================================
# Libraries
# ==================================================================

add_library(simple_log STATIC
  log/log.cc
)
target_include_directories(simple_log PUBLIC ${PROJECT_SOURCE_DIR})

add_library(simple_muduo STATIC
  net/buffer.cc
  net/socket.cc
)
target_link_libraries(simple_muduo PRIVATE simple_log)

# ==================================================================
# General tests
# ==================================================================

set(GENERAL_TESTS
  any
  bind
  regex
  request
  timefd
  timewheel
)

foreach(t ${GENERAL_TESTS})
  add_executable(test_${t} test/general/${t}.cc)
endforeach()

# ==================================================================
# Module tests
# ==================================================================

set(MODULE_TESTS
  tlog
  tbuffer
  tsocket
)

foreach(t ${MODULE_TESTS})
  add_executable(test_${t} test/module/${t}.cc)
endforeach()

target_link_libraries(test_tlog PRIVATE simple_log)
target_link_libraries(test_tbuffer PRIVATE simple_muduo simple_log)
target_link_libraries(test_tsocket PRIVATE simple_muduo simple_log)
