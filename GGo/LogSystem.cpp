#include"LogSystem.h"
namespace GGo {

const char* TOString(LogLevel level)
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

LogLevel FromString(const std::string& str)
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



}