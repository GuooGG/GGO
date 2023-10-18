#cmake最低版本3.14
cmake_minimum_required(VERSION 3.14)

#项目名称GGo
project(GGo)

#C++20
set(CMAKE_CXX_STANDARD 11)

#显示详细编译信息
set(CMAKE_VERBOSE_MAKEFILE ON)

#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#链接<pthread.h>
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
#启用全部警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#头文件包含路径
include_directories(GGo/include)

#源码变量
aux_source_directory(./GGo/src SOURCES)

# #选中服务器框架库源码文件
# file(GGO_LIB_SRC "${PROJECT_SOURCE_DIR}/GGO/src/*.cpp")

# #创建新动态库库
# add_library(GGo SHARED ${GGO_LIB_SRC})
# #为下文所有项目链接库
# link_libraries(GGo)

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


