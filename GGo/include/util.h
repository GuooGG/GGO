/**
 * @file util.h
 * @author GGo
 * @brief 常用的工具函数和类
 * @date 2023-10-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <cxxabi.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <boost/lexical_cast.hpp>



namespace GGo{


/// @brief 返回当前线程ID
pid_t GetThreadID();

/// @brief 得到当前协程ID
uint32_t GetFiberID();

/// @brief 获取当前调用栈
/// @param bt 调用栈信息容器
/// @param size 最大返回层数
/// @param skip 跳过栈顶层数
void backTrace(std::vector<std::string>& bt, int size, int skip = 1);

/// @brief 获取当前调用栈
/// @param size 最大返回层数
/// @param skip 跳过栈顶层数
/// @param prefix 调用栈信息前缀
std::string backTraceToString(size_t size = 100 ,int skip = 2, const std::string& prefix = "");

/// @brief 将时间结构体转换为指定格式字符串
/// @param ts 时间结构体
/// @param format 格式
std::string TimeToStr(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");

/// @brief 将指定格式的时间字符串转换为结构体
/// @param str 时间字符串
/// @param format 格式
time_t StrToTime(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");

/// @brief 返回当前时间的毫秒数
uint64_t getCurrentMS();

/// @brief 获取当前时间的微秒数
uint64_t getCurrentUS();

///@brief 常用文件操作辅助工具
class FSUtil{
public:


    /**
     * @brief 得到目标目录下所有subfix格式的文件并加入到files容器中
     * 
     * @param files 文件容器
     * @param path 目标路径
     * @param subfix 文件格式,默认列出所有文件
     */
    static void listAllFile(std::vector<std::string>& files,
                            const std::string& path,
                            const std::string& subfix = "");

    /**
     * @brief 获取目标文件绝对路径
     * 
     * @param path 目标文件路径
     * @param rpath 存放结果
     */
    static bool realPath(const std::string &path, std::string &rpath);

    /**
     * @brief 返回文件的父目录路径
     *
     * @param filename 文件名
     * @return std::string 父目录路径
     */
    static std::string dirName(const std::string &filename);

    /**
     * @brief 创建文件夹
     * 
     * @param dirname 文件夹名称
     */
    static bool mkDir(const std::string& dirname);

    /**
     * @brief 移除目标目录
     * 
     * @param path 路径名
     */
    static bool Rm(const std::string& path);

    /**
     * @brief 移动文件
     * 
     * @param from 源路径
     * @param to 目标路径
     */
    static bool Mv(const std::string& from, const std::string& to);

    /**
    * @brief 删除一个文件
    * @param filename 文件名
    * @param force 是否强制执行删除/预期文件是否存在
    * @return 是否删除成功
    */
    static bool unLink(const std::string& filename,bool force = false);

    /**
     * @brief 得到文件名
     * 
     * @param filename 文件路径
     */
    static std::string baseName(const std::string &filename);

    /**
     * @brief 新建符号链接
     * 
     * @param from 源文件
     * @param to 目标链接位置
     */
    static bool symLink(const std::string& from, const std::string& to);

    /**
     * @brief 打开文件并读取
     * 
     * @param ifs 文件流
     * @param filename 文件名
     * @param mode 打开模式
     * @return true 打开成功
     * @return false 打开失败
     */
    static bool openForRead(std::ifstream& ifs, const std::string& filename,
                            std::ios_base::openmode mode);

    /**
     * @brief 打开文件并写入
     * 
     * @param ofs 文件流
     * @param filename 文件名
     * @param mode 打开模式
     * @return true 写入成功
     * @return false 写入失败
     */
    static bool openForWrite(std::ofstream& ofs, const std::string& filename,
                            std::ios_base::openmode mode);


};

// TODO:: 完善接口注释
/// @brief 常用字符串操作辅助工具
class StringUtil{
public:
    static std::string format(const char* fmt, ...);
    static std::string formatVa(const char* fmt, va_list op);

    static std::string urlEncode(const std::string& str, bool space_as_plus = true);
    static std::string urlDecode(const std::string& str, bool sapce_as_plus = true);

    static std::string trim(const std::string& str, const std::string& delimit = " \t\r\n");
    static std::string trimLeft(const std::string& str, const std::string& delimit = " \t\r\n");
    static std::string trimRight(const std::string& str, const std::string& delimit = " \t\r\n");

    static std::string wstringToString(const std::wstring& wstring);
    static std::wstring stringToWstring(const std::string& string);
};


/// @brief 将类型转为字符串
/// @tparam T 类型名
/// @return 对应字符串
template<class T>
const char* typeToName(){
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(),nullptr,nullptr,nullptr);
    return s_name;
}


}