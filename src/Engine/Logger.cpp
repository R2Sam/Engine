#include "Logger.h"
#include "Log/Log.h"

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

void Logger::SetLogLevelColor(const LogLevel level, const Color color)
{
	u8 index = static_cast<u8>(level);
	if (index > s_levelColors.size())
	{
		return;
	}

	s_levelColors[index] = color;
}

const char* Logger::LevelName(const LogLevel level)
{
	switch (level)
	{
	case LogLevel::TRACE:
		return "TRACE";
	case LogLevel::DEBUG:
		return "DEBUG";
	case LogLevel::INFO:
		return "INFO";
	case LogLevel::WARN:
		return "WARN";
	case LogLevel::ERROR:
		return "ERROR";
	case LogLevel::FATAL:
		return "FATAL";
	}

	return "UNKNOWN";
}

std::string Logger::LevelColor(const LogLevel level)
{
	u8 index = static_cast<u8>(level);
	if (index > s_levelColors.size())
	{
		return LOG_WHITE;
	}

	return RgbToAnsi(s_levelColors[index]);
}

std::string Logger::RgbToAnsi(const Color color)
{
	return std::format("\033[38;2;{};{};{}m", color.r, color.g, color.b);
}

Trace::Trace(const char* func) :
m_func(func)
{
	Logger::Write<LogLevel::TRACE>("Entered: ", m_func);
}

Trace::~Trace()
{
	Logger::Write<LogLevel::TRACE>("Exited: ", m_func);
}