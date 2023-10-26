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
#include<pthread.h>
#include<fstream>
#include<string>
#include<vector>
#include<sys/types.h>
#include<stdint.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>
#include<cxxabi.h>



namespace GGo{

/**
 * @brief 返回当前线程ID
 */
pid_t GetThreadID();

/**
 * @brief 得到当前协程ID
 */
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
/// @return 
std::string backTraceToString(size_t size ,int skip = 2, const std::string& prefix = "");

/**
 * @brief 常用文件API类
 * 
 */
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

/// @brief 将类型转为字符串
/// @tparam T 类型名
/// @return 对应字符串
template<class T>
const char* typeToName(){
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(),nullptr,nullptr,nullptr);
    return s_name;
}


}