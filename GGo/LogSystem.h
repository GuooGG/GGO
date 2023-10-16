/**
 * @file LogSystem.h
 * @author GGo
 * @brief 日志模块封装
 * @date 2023-10-15
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include<string>
#include<iostream>
#include<memory>
#include<list>
#include<vector>
#include<sstream>
#include<map>
#include<fstream>
#include<cstdarg>
#include"singleton.h"

/**
 * @brief 获取主日志器
 * 
 */
#define GGO_LOG_ROOT() GGo::LoggerMgr::GetInstance()->getRoot()

/**
 * @brief 获取指定名称的日志器
 * 
 */
#define GGO_LOG_NAME(name) GGo::LoggerMgr::GetInstance()->getLogger(name)

namespace GGo {


class Logger;
class LoggerManager;
//日志级别
enum class LogLevel {
	//UNKNOWN等级
	UNKNOWN = 0,
	//DEBUG等级
	DEBUG = 1,
	//INFO等级
	INFO = 2,
	//WARN等级
	WARN = 3,
	//ERROR等级
	ERROR = 4,
	//FATAL等级
	FATAL = 5
};

/// @brief 将日志级别转成文本输出
/// @param level 日志级别
static const char* LogLevelTOString(LogLevel level);

/// @brief 将文本转成日志级别
/// @param str 日志级别文本
static LogLevel FromStringToLogLevel(const std::string& str);


/**
 * @brief 日志事件
 * 
 */
class LogEvent {
public:
	using ptr = std::shared_ptr<LogEvent>;

	/// @brief 构造函数
	/// @param logger 日志器
	/// @param level 日志等级
	/// @param file 文件名
	/// @param line 文件行号
	/// @param elapse 程序运行开始的毫秒数
	/// @param thread_id 线程ID
	/// @param fiber_id 协程ID
	/// @param time 日志时间（秒）
	/// @param thread_name 线程名 
	LogEvent(std::shared_ptr<Logger> logger, LogLevel level
		, const char* file, int32_t line
		, uint32_t elapse, uint32_t thread_id
		, uint32_t fiber_id, uint64_t time
		, const std::string& thread_name);

	
	/// @brief 得到文件名
	const char* getFile()const { return m_file; }

	/// @brief 得到行号 
	int32_t getLine() const { return m_line; }

	/// @brief 得到程序运行开始的毫秒数 
	uint32_t getElapse() const{ return m_elapse; }

	/// @brief 得到线程ID 
	uint32_t getThreadID() const { return m_threadID; }

	/// @brief 得到携程ID 
	uint32_t getFiberID() const { return m_fiberID; }

	/// @brief 得到时间戳 
	uint64_t getTime() const { return m_time; }
	
	/// @brief 得到线程名 
	const std::string& getThreadName()const { return m_threadName; }

	/// @brief 得到日志内容 
	std::string getContent()const { return m_ss.str(); }

	/// @brief 得到主日志器 
	std::shared_ptr<Logger> getLogger() const { return m_logger; }
	
	/// @brief 得到事件等级 
	LogLevel getLevel()const { return m_level; }
	
	//得到日志内容字符串流
	std::stringstream& getSS() { return m_ss; }

	//格式化写入日志内容
	void format(const char* fmt, ...);
	void format(const char* fmt, va_list al);

private:
	//文件名
	const char* m_file = nullptr;
	//行号
	int32_t m_line = 0;
	//程序运行以来的毫秒数
	uint32_t m_elapse = 0;
	//线程ID
	uint32_t m_threadID = 0;
	//协程ID
	uint32_t m_fiberID = 0;
	//时间戳
	uint64_t m_time = 0;
	//线程名
	std::string m_threadName;
	//日志内容流
	std::stringstream m_ss;
	//主日志器
	std::shared_ptr<Logger> m_logger;
	//日志等级
	LogLevel m_level;


};

/**
 * @brief 日志事件包装器
 * 
 */
class LogEventWarp{
public:

	/// @brief 构造函数
	/// @param event 日志事件
	LogEventWarp(LogEvent::ptr event);

	/// @brief 析构函数
	~LogEventWarp();

	/// @brief 获取日志事件
	LogEvent::ptr getEvent() const {return m_event;}

	/// @brief 获取日志内容字符串流
	std::stringstream& getSS();

private:
	LogEvent::ptr m_event;
};


//日志格式器
class LogFormatter {
public:
	using ptr = std::shared_ptr<LogFormatter>;
	
	/**
	 * @brief 构造函数
	 * 
	 * @param pattern 格式模板
	 * @details
	 * 	%m 消息
	 * 	%p 日志级别
	 * 	%r 累计毫秒数
	 * 	%c 日志名称
	 * 	%t 线程ID
	 * 	%n 换行
	 * 	%d 时间
	 *  %f 文件名
	 *  %L 行号
	 *  %T 制表符
	 *  %F 协程ID
	 *  %N 线程名称
	 * 
	 * 默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%L%T%m%n"
	 */
	LogFormatter(const std::string& pattern);

	/// @brief 返回格式化的日志文本和流
	/// @param logger 日志器
	/// @param level 日志级别
	/// @param event 日志事件
	std::string format(std::shared_ptr<Logger> logger,LogLevel level,LogEvent::ptr event);
	std::ostream& format(std::ostream& ofs,std::shared_ptr<Logger> logger,LogLevel level,LogEvent::ptr event);

public:
	/// @brief 日志内容格式化单位
	class FormatItem {
	public:
		using ptr = std::shared_ptr<FormatItem>;

		/// @brief 析构函数
		virtual ~FormatItem() {}
		
		/// @brief 格式化日志到流
		/// @param os 日志输出流
		/// @param logger 日志器
		/// @param level 日志等级
		/// @param event 日志事件
		virtual void format(std::ostream& os,std::shared_ptr<Logger> logger,LogLevel level,LogEvent::ptr event) = 0;

	};

	/// @brief 解析日志模板
	void init();

	/// @brief 是否有错误
	bool isError() const {return m_error;}

	/// @brief 返回日志格式模板
	const std::string getPattern() const {return m_pattern;}



private:

	//日志格式模板
	std::string m_pattern;
	//日志解析后格式
	std::vector<FormatItem::ptr> m_items;
	//是否有错误
	bool m_error = false;
	
};

//日志输出目标
class LogAppender {
//TODO::friend关键字效果
friend class Logger;
public:
	using ptr = std::shared_ptr<LogAppender>;

	/// @brief 析构函数
	virtual ~LogAppender() = default;

	/// @brief 写日志
	/// @param logger 日志器 
	/// @param level 日志等级
	/// @param event 日志事件
	virtual void log(std::shared_ptr<Logger> logger,LogLevel level,LogEvent::ptr event) = 0;

	/// @brief 更改日志格式器
	/// @param val 目标格式器
	void setFormatter(LogFormatter::ptr val);

	/// @brief 获取日志格式器
	LogFormatter::ptr getFormatter();

	/// @brief 获取日志级别
	LogLevel getLevel() const {return m_level;}

	/// @brief 设置日志级别
	void setLevel(LogLevel val){ m_level = val;}
protected:

	// 日志级别
	LogLevel m_level = LogLevel::DEBUG;
	// 是否有格式器
	bool m_hasFormatter = false;
	//日志格式器
	LogFormatter::ptr m_formatter;

};

//TODO::这个继承有什么作用
//日志器
class Logger :public std::enable_shared_from_this<Logger>{
public:
	using ptr = std::shared_ptr<Logger>;

	/// @brief 构造函数
	/// @param name 日志器名称，默认为root
	Logger(const std::string& name = "root");

	/// @brief 写日志
	/// @param level 日志级别
	/// @param event 日志事件
	void log(LogLevel level, LogEvent::ptr event);

	/// @brief 写DEBUG级别的日志
	/// @param event 日志事件
	void debug(LogEvent::ptr event);

	/// @brief 写INFO级别的日志
	/// @param event 日志事件
	void info(LogEvent::ptr event);

	/// @brief 写WARN级别的日志
	/// @param event 日志事件
	void warn(LogEvent::ptr event);

	/// @brief 写ERROR级别的日志
	/// @param event 日志事件
	void error(LogEvent::ptr event);
	
	/// @brief 写FATAL级别的日志
	/// @param event 日志事件
	void fatal(LogEvent::ptr event);

	/// @brief 添加日志目标
	/// @param appender 日志目标
	void addAppender(LogAppender::ptr appender);

	/// @brief 删除日志目标
	/// @param appender 日志目标
	void delAppender(LogAppender::ptr appender);

	/// @brief 情空日志目标
	void clearAppenders();

	/// @brief 返回日志级别
	LogLevel getLevel()const { return m_level; }

	/// @brief 设置日志级别
	void setLevel(LogLevel val) { m_level = val; }

	/// @brief 返回日志器名称
	const std::string& getNanme() const { return m_name; }

	/// @brief 设置日志格式器
	void setFormatter(LogFormatter::ptr val);

	/// @brief 设置日志格式模板 
	void setFormatter(const std::string& val);

	/// @brief 获取日志格式器
	LogFormatter::ptr getFormatter();

	void setRootLogger(std::shared_ptr<Logger> logger);
private:
	//日志名称
	std::string m_name;
	//日志级别
	LogLevel m_level;
	//日志目标集合
	std::list<LogAppender::ptr> m_appenders;
	//日志格式器
	LogFormatter::ptr m_formatter;
	//主日志器
	Logger::ptr m_root;
};

/// @brief 输出到控制台的Appender
class StdoutLogAppender :public LogAppender {
public:
	using ptr = std::shared_ptr<StdoutLogAppender>;

	void log(Logger::ptr logger,LogLevel level,LogEvent::ptr event) override;
};


/// @brief 输出到文件的Appender
class FileLogAppender :public LogAppender {
public:
	using ptr = std::shared_ptr<FileLogAppender>;

	FileLogAppender(const std::string& filename);
	void log(Logger::ptr logger,LogLevel level,LogEvent::ptr event) override;
	
	/// @brief 重新打开日志文件
	/// @return 成功返回true
	bool reopen();
private:
	//文件路径
	std::string m_filename;
	//文件流
	std::ofstream m_filestream;
	//上次打开时间
	uint64_t m_lastTime = 0;
};



/// @brief 日志管理器类
class LoggerManager{

public:
	/// @brief 构造函数
	LoggerManager();

	/// @brief 获取日志器
	/// @param name 日志器名称
	Logger::ptr getLogger(const std::string& name);

	/// @brief 初始化
	void init();

	/// @brief 返回主日志器
	Logger::ptr getRoot() const {return m_root;}


private:
	//日志器容器
	std::map<std::string, Logger::ptr> m_loggers;
	//主日志器
	Logger::ptr m_root;
};

using LoggerMgr = GGo::Singleton<LoggerManager>;

}

