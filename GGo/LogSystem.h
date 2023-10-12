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

//日志级别
enum class LogLevel {
	//未知级别
	UNKNOWN = 0,
	//DEBUG级别
	DEBUG = 1,
	//INFO级别
	INFO = 2,
	//WARN级别
	WARN = 3,
	//ERROR级别
	ERROR = 4,
	//FATAL级别
	FATAL = 5
};
/// <summary>
/// 将日志级别转换为文本输出
/// </summary>
/// <param name="level">日志级别</param>
static const char* TOString(LogLevel level);

/// <summary>
/// 将文本转换成日志级别
/// </summary>
/// <param name="str">日志级别文本</param>
static LogLevel FromString(const std::string& str);

//日志事件
class LogEvent {
public:
	using ptr = std::shared_ptr<LogEvent>;
	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="logger">日志器</param>
	/// <param name="level">日志级别</param>
	/// <param name="file">文件名</param>
	/// <param name="line">文件行号</param>
	/// <param name="elapse">程序启动以来的耗时</param>
	/// <param name="thread_id">线程id</param>
	/// <param name="fiber_id">协程id</param>
	/// <param name="time">日志时间（秒）</param>
	/// <param name="thread_name">线程名称</param>
	LogEvent(Logger::ptr logger, LogLevel level
		, const char* file, int32_t line
		, uint32_t elapse, uint32_t thread_id
		, uint32_t fiber_id, uint64_t time
		, const std::string& thread_name);

	
	/// <summary>
	/// 返回文件名
	/// </summary>
	const char* getFile()const { return m_file; }

	/// <summary>
	/// 返回行号
	/// </summary>
	int32_t getLine() const { return m_line; }

	/// <summary>
	/// 返回耗时
	/// </summary>
	uint32_t getElapse() const{ return m_elapse; }

	/// <summary>
	/// 返回线程ID
	/// </summary>
	uint32_t getThreadID() const { return m_threadID; }

	/// <summary>
	/// 返回协程ID
	/// </summary>
	uint32_t getFiberID() const { return m_fiberID; }

	/// <summary>
	/// 返回时间戳
	/// </summary>
	uint64_t getTime() const { return m_time; }
	
	/// <summary>
	/// 返回线程名称
	/// </summary>
	const std::string& getThreadName()const { return m_threadName; }

	/// <summary>
	/// 返回日志内容
	/// </summary>
	std::string getContent()const { return m_ss.str(); }

	/// <summary>
	/// 返回日志器
	/// </summary>
	Logger::ptr getLogger() const { return m_logger; }
	
	/// <summary>
	/// 返回日志级别
	/// </summary>
	LogLevel getLevel()const { return m_level; }
	
	/// <summary>
	/// 返回日志内容字符串流
	/// </summary>
	std::stringstream& getSS() { return m_ss; }


private:
	//文件名
	const char* m_file = nullptr;
	//行号
	int32_t m_line = 0;
	//程序启动以来的毫秒数
	uint32_t m_elapse = 0;
	//线程ID
	uint32_t m_threadID = 0;
	//协程ID
	uint32_t m_fiberID = 0;
	//时间戳
	uint64_t m_time = 0;
	//线程名称
	std::string m_threadName;
	//日志内容流
	std::stringstream m_ss;
	//日志器
	Logger::ptr m_logger;
	//日志等级
	LogLevel m_level;


};

//日志格式器
class LogFormatter {
public:
	using ptr = std::shared_ptr<LogFormatter>;

};

//日志输出目标
class LogAppender {
public:
	using ptr = std::shared_ptr<LogAppender>;


protected:
	//日志级别
	LogLevel m_level = LogLevel::DEBUG;
	//是否拥有日志格式器
	bool m_hasFormatter = false;
	//日志格式器
	LogFormatter::ptr m_formatter;

};

//日志器
class Logger {
public:
	using ptr = std::shared_ptr<Logger>;

	/// <summary>
	/// 构造函数
	/// </summary>
	/// <param name="name">日志器名称</param>
	Logger(const std::string& name = "root");

	/// <summary>
	/// 写日志
	/// </summary>
	/// <param name="level">日志等级</param>
	/// <param name="event">日志事件</param>
	void log(LogLevel level, LogEvent::ptr event);

	/// <summary>
	/// 写debug级别日志
	/// </summary>
	/// <param name="event">日志事件</param>
	void debug(LogEvent::ptr event);

	/// <summary>
	/// 写info级别日志
	/// </summary>
	/// <param name="event">日志事件</param>
	void info(LogEvent::ptr event);

	/// <summary>
	/// 写warn级别日志
	/// </summary>
	/// <param name="event">日志事件</param>
	void warn(LogEvent::ptr event);

	/// <summary>
	/// 写error级别日志
	/// </summary>
	/// <param name="event">日志事件</param>
	void error(LogEvent::ptr event);
	
	/// <summary>
	/// 写fatal级别日志
	/// </summary>
	/// <param name="event">日志事件</param>
	void fatal(LogEvent::ptr event);

	/// <summary>
	/// 添加日志目标
	/// </summary>
	/// <param name="appender">日志目标</param>
	void addAppender(LogAppender::ptr appender);

	/// <summary>
	/// 删除日志目标
	/// </summary>
	/// <param name="appender">日志目标</param>
	void delAppender(LogAppender::ptr appender);

	/// <summary>
	/// 清空日志目标
	/// </summary>
	void clearAppenders();

	/// <summary>
	/// 返回日志级别
	/// </summary>
	LogLevel getLevel()const { return m_level; }

	/// <summary>
	/// 设置日志级别
	/// </summary>
	void setLevel(LogLevel val) { m_level = val; }

	/// <summary>
	/// 返回日志名称
	/// </summary>
	const std::string& getNanme() const { return m_name; }

	/// <summary>
	/// 设置日志格式器
	/// </summary>
	void setFormatter(LogFormatter::ptr val);

	/// <summary>
	/// 获取日志格式器
	/// </summary>
	LogFormatter::ptr getFormatter();
private:
	//日志名称
	std::string m_name;
	///日志级别
	LogLevel m_level;
	//日志目标集合
	std::list<LogAppender::ptr> m_appenders;
	//日志格式器
	LogFormatter::ptr m_formatter;
	//主日志器
	Logger::ptr m_root;
};

//输出到控制台的Appender
class StdoutLogAppender :public LogAppender {
public:
	using ptr = std::shared_ptr<StdoutLogAppender>;


};


//输出到文件的Appender
class FileLogAppender :public LogAppender {
public:
	using ptr = std::shared_ptr<FileLogAppender>;


private:
	//文件路径
	std::string m_filename;
	//文件流
	std::ofstream m_filestream;
	//上次打开时间
	uint64_t m_lastTime = 0;
};



}