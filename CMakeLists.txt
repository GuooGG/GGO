#cmake最低版本3.14
cmake_minimum_required(VERSION 3.14)

#项目名称GGo
project(GGo)

#C++20
set(CMAKE_CXX_STANDARD 20)

#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#链接<pthread.h>
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")



#源码变量
aux_source_directory(./GGo/src SOURCES)

#头文件路径
include_directories(GGo/include)

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


