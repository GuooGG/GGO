#include"LogSystem.h"

namespace GGo {

const char* LogLevelTOString(LogLevel level)
{
	switch (level) {
	case LogLevel::DEBUG:
		return "DEBUG";
		break;
	case LogLevel::INFO:
		return "INFO";
		break;
	case LogLevel::WARN:
		return"WARN";
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



LogLevel FromStringToLogLevel(const std::string& str)
{
	if (str == "DEBUG" || str == "debug") {
		return LogLevel::DEBUG;
	}
	if (str == "INFO" || str == "info") {
		return LogLevel::INFO;
	}
	if (str == "WARN" || str == "warn") {
		return LogLevel::WARN;
	}
	if (str == "ERROR" || str == "error") {
		return LogLevel::ERROR;
	}
	if (str == "FATAL" || str == "fatal") {
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



LogEventWarp::LogEventWarp(LogEvent::ptr event)
	:m_event(event){
}

LogEventWarp::~LogEventWarp()
{
	m_event->getLogger()->log(m_event->getLevel(),m_event);
}

std::stringstream &LogEventWarp::getSS()
{
    return m_event->getSS();
}

//TODO::可变参数列表函数的处理方法
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

std::string LogFormatter::format(Logger::ptr logger, LogLevel level, LogEvent::ptr event)
{
    std::stringstream ss;
	for(auto& i : m_items){
		i->format(ss,logger,level,event);
	}
	return ss.str();
}

std::ostream &LogFormatter::format(std::ostream &ofs, Logger::ptr logger, LogLevel level, LogEvent::ptr event)
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
		//TODO::得到时间戳转换成本地时间并格式化输出
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
	m_formatter = val;
	if(m_formatter){
		m_hasFormatter = true;
	}else{
		m_hasFormatter = false;
	}

}

LogFormatter::ptr LogAppender::getFormatter()
{
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
		if(!m_appenders.empty()){
			for(auto& appender:m_appenders){
				appender->log(self,level,event);
			}
		}else if(m_root){
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
	if(!appender->m_hasFormatter){
		appender->m_formatter = m_formatter;
	}
	m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender)
{
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
	m_appenders.clear();
}
void Logger::setFormatter(LogFormatter::ptr val)
{
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
    return m_formatter;
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

FileLogAppender::FileLogAppender(const std::string &filename)
	:m_filename(filename)
{
	reopen();
}

void FileLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event)
{
	if(level >= m_level){
		uint64_t now = event->getTime();
		//TODO::这里为什么要判断时间
		if(now >= (m_lastTime + 3)){
			reopen();
			m_lastTime = now;
		}
	}
	if(!m_formatter->format(m_filestream,logger,level,event)){
		std::cout<<"error"<<std::endl;
	}
}

bool FileLogAppender::reopen()
{
    if(m_filestream){
		m_filestream.close();
	}
	//app模式打开，所有操作发生在文件末尾，追加新的日志内容
	return FSUtil::openForWrite(m_filestream, m_filename, std::ios::app);
}

void StdoutLogAppender::log(Logger::ptr logger, LogLevel level, LogEvent::ptr event)
{
	if(level >= m_level){
		m_formatter->format(std::cout,logger,level,event);
	}
}


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
	bool operator<(const LogDefine& other) const{
		return name < other.name;
	}
	bool isValid() const {
		return !name.empty();
	}
};


}