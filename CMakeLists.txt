#cmake最低版本2.8
cmake_minimum_required(VERSION 3.16)

#项目名称GGo
project(GGo)

#C++20
set(CMAKE_CXX_STANDARD 20)

# #显示详细编译信息
# set(CMAKE_VERBOSE_MAKEFILE ON)

#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#链接<pthread.h>
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#启用全部警告
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#头文件包含路径
include_directories(GGo/src)
include_directories(GGo/include)

#链接库路径
link_directories(/usr/local/lib64)
link_directories(${PROJECT_SOURCE_DIR}/lib)
#yaml-cpp动态库
find_library(yaml-cpp /usr/local/lib64/libyaml-cpp.so)

#源码文件
aux_source_directory(./GGo/src GGO_LIB_SRC)
#制作GGo动态库
add_library(GGO_LIB SHARED ${GGO_LIB_SRC})

set(LIBS 
        GGO_LIB
        yaml-cpp
)

message(STATUS ${PROJECT_BINARY_DIR})

#生成日志测试 01
add_executable(LogTest01 Test/LogTest01.cpp)
target_link_libraries(LogTest01 ${LIBS})
#生成日志测试 02 测试日志系统完整框架功能
add_executable(LogTest02 Test/LogTest02.cpp)
target_link_libraries(LogTest02 ${LIBS})
#生成配置测试 01
add_executable(ConfigTest01 Test/ConfigTest01.cpp)
target_link_libraries(ConfigTest01 ${LIBS})
#生成Yaml-cpp测试 01
add_executable(YmlTest01 Test/YmlTest01.cpp)
target_link_libraries(YmlTest01 ${LIBS})
#生成YML文件转配置信息测试 01
add_executable(YmlToConfigTest01 Test/YmlToConfigTest01.cpp)
target_link_libraries(YmlToConfigTest01 ${LIBS})
# 线程模块测试
add_executable(ThreadTest01 Test/ThreadTest01.cpp)
target_link_libraries(ThreadTest01 ${LIBS})
