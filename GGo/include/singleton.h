/**
 * @file singleton.h
 * @author GGo
 * @brief 单例模式封装
 * @date 2023-10-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include<memory>
namespace GGo{

namespace{

template<class T,class X,int N>
T& GetInstance(){
    static T v;
    return v;
}

template<class T,class X,int N>
std::shared_ptr<T> GetInstancePtr(){
    static std::shared_ptr<T> v(new T);
    return v;
}

}

/// @brief 单例模式封装类
/// @tparam T 类型
/// @tparam X 为了创造多个实例对应的Tag
/// @tparam N 同一个Tag创造多个实例的索引
template<class T,class X = void,int N = 0>
class Singleton{
public:

    /// @brief 返回单例裸指针 
    static T* GetInstance(){
        static T v;
        return &v;
    }

};

/// @brief 单例模式智能指针封装类
/// @tparam T 类型
/// @tparam X 为了创造多个实例对应的Tag
/// @tparam N 同一个Tag创造多个实例的索引
template<class T,class X = void,int N = 0>
class SingletonPtr{

public:
    static std::shared_ptr<T> GetInstance(){
        static std::shared_ptr<T> v(new T);
        return v;
    }
};





}