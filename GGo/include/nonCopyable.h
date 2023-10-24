/**
 * @file nonCopyable.h
 * @author GGo
 * @brief 不可拷贝对象基类
 * @date 2023-10-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
class nonCopyable{
public:

    /// @brief 默认构造函数
    nonCopyable() = default;

    /// @brief 默认析构函数
    ~nonCopyable() = default;

    /// @brief 禁用拷贝构造函数
    nonCopyable(const nonCopyable& other) = delete;

    /// @brief 禁用赋值函数
    nonCopyable& operator=(const nonCopyable& other) = delete;
};