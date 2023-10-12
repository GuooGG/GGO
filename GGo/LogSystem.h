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

//��־����
enum class LogLevel {
	//δ֪����
	UNKNOWN = 0,
	//DEBUG����
	DEBUG = 1,
	//INFO����
	INFO = 2,
	//WARN����
	WARN = 3,
	//ERROR����
	ERROR = 4,
	//FATAL����
	FATAL = 5
};
/// <summary>
/// ����־����ת��Ϊ�ı����
/// </summary>
/// <param name="level">��־����</param>
static const char* TOString(LogLevel level);

/// <summary>
/// ���ı�ת������־����
/// </summary>
/// <param name="str">��־�����ı�</param>
static LogLevel FromString(const std::string& str);

//��־�¼�
class LogEvent {
public:
	using ptr = std::shared_ptr<LogEvent>;
	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="logger">��־��</param>
	/// <param name="level">��־����</param>
	/// <param name="file">�ļ���</param>
	/// <param name="line">�ļ��к�</param>
	/// <param name="elapse">�������������ĺ�ʱ</param>
	/// <param name="thread_id">�߳�id</param>
	/// <param name="fiber_id">Э��id</param>
	/// <param name="time">��־ʱ�䣨�룩</param>
	/// <param name="thread_name">�߳�����</param>
	LogEvent(Logger::ptr logger, LogLevel level
		, const char* file, int32_t line
		, uint32_t elapse, uint32_t thread_id
		, uint32_t fiber_id, uint64_t time
		, const std::string& thread_name);

	
	/// <summary>
	/// �����ļ���
	/// </summary>
	const char* getFile()const { return m_file; }

	/// <summary>
	/// �����к�
	/// </summary>
	int32_t getLine() const { return m_line; }

	/// <summary>
	/// ���غ�ʱ
	/// </summary>
	uint32_t getElapse() const{ return m_elapse; }

	/// <summary>
	/// �����߳�ID
	/// </summary>
	uint32_t getThreadID() const { return m_threadID; }

	/// <summary>
	/// ����Э��ID
	/// </summary>
	uint32_t getFiberID() const { return m_fiberID; }

	/// <summary>
	/// ����ʱ���
	/// </summary>
	uint64_t getTime() const { return m_time; }
	
	/// <summary>
	/// �����߳�����
	/// </summary>
	const std::string& getThreadName()const { return m_threadName; }

	/// <summary>
	/// ������־����
	/// </summary>
	std::string getContent()const { return m_ss.str(); }

	/// <summary>
	/// ������־��
	/// </summary>
	Logger::ptr getLogger() const { return m_logger; }
	
	/// <summary>
	/// ������־����
	/// </summary>
	LogLevel getLevel()const { return m_level; }
	
	/// <summary>
	/// ������־�����ַ�����
	/// </summary>
	std::stringstream& getSS() { return m_ss; }


private:
	//�ļ���
	const char* m_file = nullptr;
	//�к�
	int32_t m_line = 0;
	//�������������ĺ�����
	uint32_t m_elapse = 0;
	//�߳�ID
	uint32_t m_threadID = 0;
	//Э��ID
	uint32_t m_fiberID = 0;
	//ʱ���
	uint64_t m_time = 0;
	//�߳�����
	std::string m_threadName;
	//��־������
	std::stringstream m_ss;
	//��־��
	Logger::ptr m_logger;
	//��־�ȼ�
	LogLevel m_level;


};

//��־��ʽ��
class LogFormatter {
public:
	using ptr = std::shared_ptr<LogFormatter>;

};

//��־���Ŀ��
class LogAppender {
public:
	using ptr = std::shared_ptr<LogAppender>;


protected:
	//��־����
	LogLevel m_level = LogLevel::DEBUG;
	//�Ƿ�ӵ����־��ʽ��
	bool m_hasFormatter = false;
	//��־��ʽ��
	LogFormatter::ptr m_formatter;

};

//��־��
class Logger {
public:
	using ptr = std::shared_ptr<Logger>;

	/// <summary>
	/// ���캯��
	/// </summary>
	/// <param name="name">��־������</param>
	Logger(const std::string& name = "root");

	/// <summary>
	/// д��־
	/// </summary>
	/// <param name="level">��־�ȼ�</param>
	/// <param name="event">��־�¼�</param>
	void log(LogLevel level, LogEvent::ptr event);

	/// <summary>
	/// дdebug������־
	/// </summary>
	/// <param name="event">��־�¼�</param>
	void debug(LogEvent::ptr event);

	/// <summary>
	/// дinfo������־
	/// </summary>
	/// <param name="event">��־�¼�</param>
	void info(LogEvent::ptr event);

	/// <summary>
	/// дwarn������־
	/// </summary>
	/// <param name="event">��־�¼�</param>
	void warn(LogEvent::ptr event);

	/// <summary>
	/// дerror������־
	/// </summary>
	/// <param name="event">��־�¼�</param>
	void error(LogEvent::ptr event);
	
	/// <summary>
	/// дfatal������־
	/// </summary>
	/// <param name="event">��־�¼�</param>
	void fatal(LogEvent::ptr event);

	/// <summary>
	/// �����־Ŀ��
	/// </summary>
	/// <param name="appender">��־Ŀ��</param>
	void addAppender(LogAppender::ptr appender);

	/// <summary>
	/// ɾ����־Ŀ��
	/// </summary>
	/// <param name="appender">��־Ŀ��</param>
	void delAppender(LogAppender::ptr appender);

	/// <summary>
	/// �����־Ŀ��
	/// </summary>
	void clearAppenders();

	/// <summary>
	/// ������־����
	/// </summary>
	LogLevel getLevel()const { return m_level; }

	/// <summary>
	/// ������־����
	/// </summary>
	void setLevel(LogLevel val) { m_level = val; }

	/// <summary>
	/// ������־����
	/// </summary>
	const std::string& getNanme() const { return m_name; }

	/// <summary>
	/// ������־��ʽ��
	/// </summary>
	void setFormatter(LogFormatter::ptr val);

	/// <summary>
	/// ��ȡ��־��ʽ��
	/// </summary>
	LogFormatter::ptr getFormatter();
private:
	//��־����
	std::string m_name;
	///��־����
	LogLevel m_level;
	//��־Ŀ�꼯��
	std::list<LogAppender::ptr> m_appenders;
	//��־��ʽ��
	LogFormatter::ptr m_formatter;
	//����־��
	Logger::ptr m_root;
};

//���������̨��Appender
class StdoutLogAppender :public LogAppender {
public:
	using ptr = std::shared_ptr<StdoutLogAppender>;


};


//������ļ���Appender
class FileLogAppender :public LogAppender {
public:
	using ptr = std::shared_ptr<FileLogAppender>;


private:
	//�ļ�·��
	std::string m_filename;
	//�ļ���
	std::ofstream m_filestream;
	//�ϴδ�ʱ��
	uint64_t m_lastTime = 0;
};



}