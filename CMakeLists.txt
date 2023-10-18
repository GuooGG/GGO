#cmake最低版本3.14
cmake_minimum_required(VERSION 3.14)

#项目名称GGo
project(GGo)

#C++20
set(CMAKE_CXX_STANDARD 11)

# #显示详细编译信息
# set(CMAKE_VERBOSE_MAKEFILE ON)

#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#链接<pthread.h>
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
#启用全部警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#头文件包含路径
include_directories(GGo/include)

link_directories(/usr/local/lib64)

find_library(yaml-cpp /usr/local/lib64/libyaml-cpp.so)

#源码变量
aux_source_directory(./GGo/src SOURCES)


message("=============打印全部源码文件==============")
foreach(SRC ${SOURCES})
  message(STATUS "Source file: ${SRC}")
endforeach()
message("===========================================")

#测试库
add_executable(GGoTest GGo/GGo.cpp ${SOURCES} )
#生成日志测试 01
add_executable(LogTest01 Test/LogTest01.cpp ${SOURCES})
#生成日志测试 02 测试日志系统完整框架功能
add_executable(LogTest02 Test/LogTest02.cpp ${SOURCES})
#生成配置测试 01
add_executable(ConfigTest01 Test/ConfigTest01.cpp ${SOURCES})
#生成Yaml-cpp测试 01
add_executable(YmlTest01 Test/YmlTest01.cpp ${SOURCES})
target_link_libraries(YmlTest01 yaml-cpp)


