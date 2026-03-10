#include "Logger.h"

void Logger::SetLogLevel(const LogLevel levelIn)
{
	std::lock_guard<std::mutex> lock(s_mutex);

	s_level = levelIn;
}

void Logger::SetLogFile(const char* path)
{
	std::lock_guard<std::mutex> lock(s_mutex);

	if (s_file.is_open())
	{
		s_file.close();
	}

	if (std::string(path).empty())
	{
		return;
	}

	s_file.open(path, std::ios::app);
}

const char* Logger::LevelName(const LogLevel level)
{
	switch (level)
	{
	case LogLevel::DEBUG:
		return "DEBUG";
	case LogLevel::INFO:
		return "INFO";
	case LogLevel::WARN:
		return "WARN";
	case LogLevel::ERROR:
		return "ERROR";
	}

	return "UNKNOWN";
}

const char* Logger::LevelColor(const LogLevel level)
{
	switch (level)
	{
	case LogLevel::DEBUG:
		return LOG_BLUE;
	case LogLevel::INFO:
		return LOG_WHITE;
	case LogLevel::WARN:
		return LOG_YELLOW;
	case LogLevel::ERROR:
		return LOG_RED;
	}

	return LOG_WHITE;
}