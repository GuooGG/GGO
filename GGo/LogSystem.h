#pragma once
#include<string>
#include<memory>
#include<list>
#include<vector>
#include<sstream>
#include<fstream>
namespace GGo {
class Logger;
class LoggerManager;

//日志等级
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


static const char* TOString(LogLevel level);


static LogLevel FromString(const std::string& str);


class LogEvent {
public:
	using ptr = std::shared_ptr<LogEvent>;

	/// @brief 构造函数
	/// @param logger 日志器
	/// @param level 日志等级
	/// @param file 文件名
	/// @param line 行号
	/// @param elapse 程序运行开始的毫秒数
	/// @param thread_id 线程ID
	/// @param fiber_id 协程ID
	/// @param time 时间戳
	/// @param thread_name 线程名 
	LogEvent(Logger::ptr logger, LogLevel level
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
	Logger::ptr getLogger() const { return m_logger; }
	
	/// @brief 得到事件等级 
	LogLevel getLevel()const { return m_level; }
	
	//得到日志流
	std::stringstream& getSS() { return m_ss; }


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
	Logger::ptr m_logger;
	//日志等级
	LogLevel m_level;


};

//日志格式器
class LogFormatter {
public:
	using ptr = std::shared_ptr<LogFormatter>;
	LogFormatter(const std::string& pattern);


	std::string format(LogEvent::ptr event);
public:
	class FormatItem {
	public:
		using ptr = std::shared_ptr<FormatItem>;
		virtual ~FormatItem() {}
		virtual std::string format(LogEvent::ptr event) = 0;
	};

private:

	std::string m_pattern;

	std::vector<FormatItem::ptr> m_items;

	bool m_error = false;
	


};

//日志输出目标
class LogAppender {
public:
	using ptr = std::shared_ptr<LogAppender>;


protected:

	LogLevel m_level = LogLevel::DEBUG;

	bool m_hasFormatter = false;

	LogFormatter::ptr m_formatter;

};


class Logger {
public:
	using ptr = std::shared_ptr<Logger>;


	Logger(const std::string& name = "root");


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


	void addAppender(LogAppender::ptr appender);


	void delAppender(LogAppender::ptr appender);


	void clearAppenders();


	LogLevel getLevel()const { return m_level; }


	void setLevel(LogLevel val) { m_level = val; }


	const std::string& getNanme() const { return m_name; }


	void setFormatter(LogFormatter::ptr val);


	LogFormatter::ptr getFormatter();
private:

	std::string m_name;

	LogLevel m_level;

	std::list<LogAppender::ptr> m_appenders;

	LogFormatter::ptr m_formatter;

	Logger::ptr m_root;
};


class StdoutLogAppender :public LogAppender {
public:
	using ptr = std::shared_ptr<StdoutLogAppender>;


};



class FileLogAppender :public LogAppender {
public:
	using ptr = std::shared_ptr<FileLogAppender>;


private:

	std::string m_filename;

	std::ofstream m_filestream;

	uint64_t m_lastTime = 0;
};



}

