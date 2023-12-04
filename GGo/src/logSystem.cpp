#include"logSystem.h"
#include"yaml-cpp/yaml.h"
#include"config.h"
namespace GGo {

/// @brief 将日志级别转成文本输出
/// @param level 日志级别
static const char *LogLevelTOString(LogLevel level)
{
	switch (level)
	{
	case LogLevel::DEBUG:
		return "DEBUG";
		break;
	case LogLevel::INFO:
		return "INFO";
		break;
	case LogLevel::WARN:
		return "WARN";
		break;
	case LogLevel::ERROR:
		return "ERROR";
		break;
	case LogLevel::FATAL:
		return "FATAL";
	default:
		return "UNKNOWN";
	}
}

/// @brief 将文本转成日志级别
/// @param str 日志级别文本
static LogLevel FromStringToLogLevel(const std::string &str){
	if (str == "DEBUG" || str == "debug")
	{
		return LogLevel::DEBUG;
	}
	if (str == "INFO" || str == "info")
	{
		return LogLevel::INFO;
	}
	if (str == "WARN" || str == "warn")
	{
		return LogLevel::WARN;
	}
	if (str == "ERROR" || str == "error")
	{
		return LogLevel::ERROR;
	}
	if (str == "FATAL" || str == "fatal")
	{
		return LogLevel::FATAL;
	}
	return LogLevel::UNKNOWN;
}




LogEvent::LogEvent(Logger::ptr logger, LogLevel level, const char *file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time, const std::string &thread_name)
	:m_file(file)
	,m_line(line)
	,m_elapse(elapse)
	,m_threadID(thread_id)
	,m_fiberID(fiber_id)
	,m_time(time)
	,m_threadName(thread_name)
	,m_logger(logger)
	,m_level(level){
}



LogEventWrap::LogEventWrap(LogEvent::ptr event)
	:m_event(event){
}

LogEventWrap::~LogEventWrap()
{
	m_event->getLogger()->log(m_event->getLevel(),m_event);
}

std::stringstream &LogEventWrap::getSS()
{
    return m_event->getSS();
}

void LogEvent::format(const char *fmt, ...)
{
	va_list al;
	va_start(al,fmt);
	format(fmt,al);
	va_end(al);
}
void LogEvent::format(const char *fmt, va_list al)
{
	char* buf = nullptr;
	int len = vasprintf(&buf,fmt,al);
	if(len != -1){
		m_ss << std::string(buf,len);
		free(buf);
	}
}

LogFormatter::LogFormatter(const std::string &pattern)
	:m_pattern(pattern)
{
	init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event)
{
    std::stringstream ss;
	for(auto& i : m_items){
		i->format(ss,logger,level,event);
	}
	return ss.str();
}

std::ostream &LogFormatter::format(std::ostream &ofs, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event)
{
    for(auto& i : m_items){
		i->format(ofs,logger,level,event);
	}
	return ofs;
}

class MessageFormatItem : public LogFormatter::FormatItem{
public:
	MessageFormatItem(const std::string& str =""){}
	void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getContent();
    }	
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
	LevelFormatItem(const std::string& str = ""){}
	void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override{
		os << LogLevelTOString(level);
	}
};

class ElapseFormatItem : public LogFormatter::FormatItem{
public:
	ElapseFormatItem(const std::string& str=""){}
	void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override{
		os << event->getElapse();
	}
};

class LoggerNameFormatItem : public LogFormatter::FormatItem{
public:
	LoggerNameFormatItem(const std::string& str = ""){}
	void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override{
		os << event->getLogger()->getNanme();
	}
};

class ThreadIDFormatItem : public LogFormatter::FormatItem{
public:
    ThreadIDFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getThreadID();
    }
};

class FiberIDFormatItem : public LogFormatter::FormatItem{
public:
    FiberIDFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getFiberID();
    }
};

class ThreadNameFormatItem : public LogFormatter::FormatItem{
public:
	ThreadNameFormatItem(const std::string& str = ""){}
	void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override{
		os << event->getThreadName();	
	}
};

class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
	DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
		:m_format(format)	
	{
		if(m_format.empty()){
			m_format = "%Y-%m-%d %H:%M:%S";
		}
	}
	void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override{
		struct tm tm;
		time_t time = event->getTime();
		localtime_r(&time,&tm);
		char buf[64];
		strftime(buf,sizeof(buf),m_format.c_str(),&tm);
		os << buf;
	}
private:
	std::string m_format;
};

class FileNameFormatItem : public LogFormatter::FormatItem{
public:
	FileNameFormatItem(const std::string& str =""){}
	void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getFile();
    }

};

class LineFormatItem : public LogFormatter::FormatItem{
public:
	LineFormatItem(const std::string& str =""){}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getLine();
    }	
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem{
public:
    StringFormatItem(const std::string& str)
        :m_string(str) {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem{
public:
    TabFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, Logger::ptr logger, LogLevel level, LogEvent::ptr event) override {
        os << "\t";
    }
};

void LogFormatter::init()
{
 //str, format, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
	static std::map<std::string,std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
		{"m",[](const std::string& fmt){return FormatItem::ptr(new MessageFormatItem(fmt));}},
		{"p",[](const std::string& fmt){return FormatItem::ptr(new LevelFormatItem(fmt));}},
		{"r",[](const std::string& fmt){return FormatItem::ptr(new ElapseFormatItem(fmt));}},
		{"c",[](const std::string& fmt){return FormatItem::ptr(new LoggerNameFormatItem(fmt));}},
		{"t",[](const std::string& fmt){return FormatItem::ptr(new ThreadIDFormatItem(fmt));}},
		{"n",[](const std::string& fmt){return FormatItem::ptr(new NewLineFormatItem(fmt));}},
		{"d",[](const std::string& fmt){return FormatItem::ptr(new DateTimeFormatItem(fmt));}},
		{"f",[](const std::string& fmt){return FormatItem::ptr(new FileNameFormatItem(fmt));}},
		{"L",[](const std::string& fmt){return FormatItem::ptr(new LineFormatItem(fmt));}},
		{"T",[](const std::string& fmt){return FormatItem::ptr(new TabFormatItem(fmt));}},
		{"F",[](const std::string& fmt){return FormatItem::ptr(new FiberIDFormatItem(fmt));}},
		{"N",[](const std::string& fmt){return FormatItem::ptr(new ThreadNameFormatItem(fmt));}}
	};


    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }

        //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    //std::cout << m_items.size() << std::en
}

void LogAppender::setFormatter(LogFormatter::ptr val)
{
	mutexType::Lock lock(m_mutex);
	m_formatter = val;
	if(m_formatter){
		m_hasFormatter = true;
	}else{
		m_hasFormatter = false;
	}

}

LogFormatter::ptr LogAppender::getFormatter()
{
	mutexType::Lock lock(m_mutex);
	return m_formatter;
}
Logger::Logger(const std::string &name)
	:m_name(name)
	,m_level(LogLevel::DEBUG)
{
	m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%L%T%m%n"));
}

void Logger::log(LogLevel level, LogEvent::ptr event)
{
	if(level >= m_level){
		auto self = shared_from_this();
		mutexType::Lock lock(m_mutex);
		if(!m_appenders.empty()){
			for(auto& appender:m_appenders){
				appender->log(self, level, event);
			}
		}else if(m_root){
			//如果该日志器没有appender，则使用root日志器输出
			m_root->log(level,event);
		}
	}
	
}
void Logger::debug(LogEvent::ptr event)
{
	log(LogLevel::DEBUG,event);
}
void Logger::info(LogEvent::ptr event)
{
	log(LogLevel::INFO,event);
}
void Logger::warn(LogEvent::ptr event)
{
	log(LogLevel::WARN,event);
}
void Logger::error(LogEvent::ptr event)
{
	log(LogLevel::ERROR,event);
}
void Logger::fatal(LogEvent::ptr event)
{
	log(LogLevel::FATAL,event);
}

void Logger::addAppender(LogAppender::ptr appender)
{
	mutexType::Lock lock(m_mutex);
	if(!appender->m_hasFormatter){
		appender->m_formatter = m_formatter;
	}
	m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender)
{
	mutexType::Lock lock(m_mutex);
	for(auto it = m_appenders.begin()
		;it != m_appenders.end();it++){
			if(*it == appender){
				m_appenders.erase(it);
				break;
			}
		}
}
void Logger::clearAppenders()
{
	mutexType::Lock lock(m_mutex);
	m_appenders.clear();
}
void Logger::setFormatter(LogFormatter::ptr val)
{
	mutexType::Lock lock(m_mutex);
	m_formatter = val;
	for(auto& appender:m_appenders){
		if(!appender->m_hasFormatter){
			appender->setFormatter(val);
		}
	}
}
void Logger::setFormatter(const std::string &val)
{
	GGo::LogFormatter::ptr new_format(new GGo::LogFormatter(val));
	if(new_format->isError()){
		std::cout<<"Logger setFormatter name="<<m_name
		<<"value="<<val<<" invalid formatter"<<std::endl;
	}
	setFormatter(new_format);
}

LogFormatter::ptr Logger::getFormatter()
{
	mutexType::Lock lock(m_mutex);
	return m_formatter;
}

std::string Logger::toYamlString()
{
	mutexType::Lock lock(m_mutex);
	YAML::Node node;
	node["name"] = m_name;
	if(m_level != LogLevel::UNKNOWN){
		node["level"] = LogLevelTOString(m_level);
	}
	if (m_formatter)
	{
		node["formatter"] = m_formatter->getPattern();
	}
	for(auto& i :m_appenders){
		node["appenders"].push_back(YAML::Load(i->toYamlString()));
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
}

void Logger::setRootLogger(Logger::ptr logger)
{
	m_root = logger;
}

LoggerManager::LoggerManager()
{
	m_root.reset(new Logger);
	m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

	m_loggers[m_root->getNanme()] = m_root;
	init();

}

Logger::ptr LoggerManager::getLogger(const std::string &name)
{
	mutexType::Lock lock(m_mutex);
	auto it = m_loggers.find(name);
	if(it != m_loggers.end()){
		return it->second;
	}
	Logger::ptr logger(new Logger(name));
	logger->setRootLogger(m_root);
	m_loggers[name] = logger;
	return logger;
}

void LoggerManager::init(){}

std::string LoggerManager::toYamlString()
{
	mutexType::Lock lock(m_mutex);
	YAML::Node node;
	for(auto& i :m_loggers){
		node.push_back(YAML::Load(i.second->toYamlString()));
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
}

FileLogAppender::FileLogAppender(const std::string &filename)
	:m_filename(filename)
{
	reopen();
}

void FileLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event)
{
	if(level >= m_level){
		uint64_t now = event->getTime();
		if(now >= (m_lastTime + 3)){
			reopen();
			m_lastTime = now;
		}
		mutexType::Lock lock(m_mutex);
		if(!m_formatter->format(m_filestream,logger,level,event)){
			std::cout<<"error"<<std::endl;
		}
	}
}

std::string FileLogAppender::toYamlString()
{
	mutexType::Lock lock(m_mutex);
	YAML::Node node;
	node["type"] = "FileLogAppender";
	node["file"] = m_filename;
	if(m_level != LogLevel::UNKNOWN){
		node["level"] = LogLevelTOString(m_level);
	}
	if(m_hasFormatter && m_formatter){
		node["formatter"] = m_formatter->getPattern();
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
}

bool FileLogAppender::reopen()
{
	mutexType::Lock lock(m_mutex);
	if(m_filestream){
		m_filestream.close();
	}
	//app模式打开，所有操作发生在文件末尾，追加新的日志内容
	return FSUtil::openForWrite(m_filestream, m_filename, std::ios::app);
}

void StdoutLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event)
{
	if(level >= m_level){
		mutexType::Lock lock(m_mutex);
		m_formatter->format(std::cout,logger,level,event);
	}
}

std::string StdoutLogAppender::toYamlString()
{
	mutexType::Lock lock(m_mutex);
	YAML::Node node;
	node["type"] = "StdoutLogAppender";
	if(m_level != LogLevel::UNKNOWN){
		node["level"] = LogLevelTOString(m_level);
	}
	if(m_hasFormatter && m_formatter){
		node["formatter"] = m_formatter->getPattern();
	}
	std::stringstream ss;
	ss << node;
	return ss.str();
}

//配置系统整合日志系统 ，定义 LogAppender 和 logger 的定义结构体，并作模板偏特化

struct LogAppenderDefine{
	/// @brief 1 file / 2 stdout
	int type = 0;
	LogLevel level = LogLevel::UNKNOWN;
	std::string formatter;
	std::string file;

	bool operator==(const LogAppenderDefine& other) const{
		return 	type == other.type &&
				level == other.level &&
				formatter == other.formatter &&
				file == other.file;
				
	}

};

struct LogDefine{
	std::string name;
	LogLevel level = LogLevel::UNKNOWN;
	std::string formatter;
	std::vector<LogAppenderDefine> appenders;

	bool operator==(const LogDefine& other) const{
		return 	name == other.name &&
				level == other.level &&
				formatter == other.formatter &&
				appenders == other.appenders;
	}
	//为了能使用std::map，需要提供大小比较的方法
	bool operator<(const LogDefine& other) const{
		return name < other.name;
	}
	bool isValid() const {
		return !name.empty();
	}
};


/// @brief 模板类型偏特化 实现 YAML 字符串和LogDefine 自定义类的相互转换
template<>
class LexicalCast<std::string, LogDefine>{
public:
	LogDefine operator()(const std::string& ymlstr){
		YAML::Node node = YAML::Load(ymlstr);
		LogDefine ret;
		if(!node["name"].IsDefined()){
			std::cout << "log config error: name is a null, " << node << std::endl;
			throw std::logic_error("log config is null");
		}
		if(!node["level"].IsDefined()){
			std::cout << "log config error: level is a null, " << node << std::endl;
			throw std::logic_error("log config is null");
		}
		ret.name = node["name"].as<std::string>();
		ret.level = FromStringToLogLevel(node["level"].as<std::string>());
		if(node["formatter"].IsDefined()){
			ret.formatter = node["formatter"].as<std::string>();
		}
		if(node["appenders"].IsDefined()){
			for(size_t i = 0; i < node["appenders"].size(); i++){
				YAML::Node appender_node = node["appenders"][i];
				if(!appender_node["type"].IsDefined()){
					std::cout << "log config error: appender type is null" << std::endl;
				}
				std::string type = appender_node["type"].as<std::string>();
				LogAppenderDefine appender_define;
				if(type ==  "FileLogAppender"){
					appender_define.type = 1;
					if(appender_node["level"].IsDefined()){
						appender_define.level = FromStringToLogLevel(appender_node["level"].as<std::string>());
					}
					if(!appender_node["file"].IsDefined()){
						std::cout << "log config error: fileappender file is null"
								<< appender_node << std::endl;
						continue;
					}
					appender_define.file = appender_node["file"].as<std::string>();
					if(appender_node["formatter"].IsDefined()){
						appender_define.formatter = appender_node["formatter"].as<std::string>();
					}
				}else if(type == "StdoutLogAppender"){
					appender_define.type = 2;
					if (appender_node["level"].IsDefined())
					{
						appender_define.level = FromStringToLogLevel(appender_node["level"].as<std::string>());
					}
					if (appender_node["formatter"].IsDefined())
					{
						appender_define.formatter = appender_node["formatter"].as<std::string>();
					}
				}else{
					std::cout << "log config error: appender type is valid" << std::endl;
				}	
				ret.appenders.push_back(appender_define);
			}
		}
		return ret;
	}
};

template<>
class LexicalCast<LogDefine,std::string>{
public:
	std::string operator()(const LogDefine& input){
		YAML::Node node;
		std::stringstream ss;
		node["name"] = input.name;
		if(input.level != LogLevel::UNKNOWN){
			node["level"] = LogLevelTOString(input.level);
		}
		if(!input.formatter.empty()){
			node["formatter"] = input.formatter;
		}

		for(auto& appender: input.appenders){
			YAML::Node appender_node;
			if(appender.type == 1){
				appender_node["type"] = "FileLogAppender";
				appender_node["file"] = appender.file;
			}else if(appender.type == 2){
				appender_node["type"] = "StdoutLogAppender";
			}
			if(appender.level != LogLevel::UNKNOWN){
				appender_node["level"] = LogLevelTOString(appender.level);
			}
			if(!appender.formatter.empty()){
				appender_node["formatter"] = appender.formatter;
			}

			node["appenders"].push_back(appender_node);
		}
		ss << node;
		return ss.str();
	}
};


GGo::ConfigVar<std::set<LogDefine> >::ptr g_log_defines = 
	GGo::Config::Lookup("logs", std::set<LogDefine>(), "logs define");

//静态成员的实例化先于main函数之前 可以确保程序执行前日志配置加载完毕
struct LogIniter
{
	LogIniter(){
		g_log_defines->addListener([](const std::set<LogDefine>& oldv,
									const std::set<LogDefine>& newv){
			GGO_LOG_INFO(GGO_LOG_ROOT()) << "on log config changed";
			for(auto& logdefine : newv){
				auto it = oldv.find(logdefine);
				GGo::Logger::ptr logger;
				if(it == oldv.end()){
					// 添加新的logger并添加到logger容器
					logger = GGO_LOG_NAME(logdefine.name);
				}else{
					//按名字找到了logger
					if(!(*it == logdefine)){
						//日志被修改过
						logger = GGO_LOG_NAME(logdefine.name);
					}else{
						//日志没有被修改，直接跳过后面的流程
						continue;
					}
				}
				//按logdefine编辑得到的日志器
				logger->setLevel(logdefine.level);
				if(!logdefine.formatter.empty()){
					logger->setFormatter(logdefine.formatter);
				}
				//修改appenders,事先清空重添加，不再一一比较
				logger->clearAppenders();
				for(auto& appender: logdefine.appenders){
					GGo::LogAppender::ptr ap;
					if(appender.type == 1){
						ap.reset(new FileLogAppender(appender.file));
					}else if(appender.type == 2){
						ap.reset(new StdoutLogAppender());
					}else{
						//类型错误，添加该appender
						continue;
					}
					ap->setLevel(appender.level);
					if(!appender.formatter.empty()){
						LogFormatter::ptr fmt(new LogFormatter(appender.formatter));
						if(!fmt->isError()){
							ap->setFormatter(fmt);
						}
						else{
							std::cout << "log.name= " << logdefine.name << " appender type= "
								<< appender.type << " formatter= " << appender.formatter
								<< "is valid" << std::endl;
						}
					}
					logger->addAppender(ap);

				}



			}
			
			for(auto& logdefine : oldv){
				auto it = newv.find(logdefine);
				if(it == newv.end()){
					//删除logger
					auto logger = GGO_LOG_NAME(logdefine.name);
					logger->setLevel(LogLevel::UNKNOWN);
					logger->clearAppenders();
				}
			}
		});
	}
};

static LogIniter __log_init;




}