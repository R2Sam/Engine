#pragma once

#include "Log/Log.h"

#include <fstream>
#include <mutex>

/**
 * @brief Console and file logging levels
 */

enum class LogLevel
{
	DEBUG,
	INFO,
	WARN,
	ERROR,
};

/**
 * @brief Thread safe logging utility with differing levels for both console and file
 */

class Logger
{
public:

	/**
	 * @brief Sets minimum log level
	 *
	 * @param levelIn Minimum log level to output
	 */

	static void SetLogLevel(const LogLevel levelIn)
	{
		std::lock_guard<std::mutex> lock(mutex);

		level = levelIn;
	}

	/**
	 * @brief Sets the output file
	 *
	 * If a file is already opened it is closed.
	 *
	 * @param path Path to log file, empty string disables file output
	 */

	static void SetLogFile(const char* path)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if (file.is_open())
		{
			file.close();
		}

		if (std::string(path).empty())
		{
			return;
		}

		file.open(path, std::ios::app);
	}

	/**
	 * @brief Writes a log message
	 *
	 * Outputs a message to stderr and a file if set,
	 * depending if the log level is higher than the set minimum.
	 *
	 * @tparam Args Strings and other printable
	 * @param levelIn Message log level
	 * @param args Values to be printed separated by commas
	 *
	 * Usage:
	 * @code
	 * logger.Write(LogLevel::INFO, "Value is: ", number);
	 * @endcode
	 */

	template <typename... Args>
	static void Write(const LogLevel levelIn, Args&&... args)
	{
		if (levelIn < level)
		{
			return;
		}

		std::lock_guard<std::mutex> lock(mutex);

		if (file.is_open())
		{
			file << "[" << GetCurrentTimeString() << "] " << "[" << LevelName(level) << "] ";
			(file << ... << std::forward<Args>(args)) << '\n';
		}

		std::cerr << LevelColor(level) << "[" << GetCurrentTimeString() << "] " << "[" << LevelName(level) << "] ";
		(std::cerr << ... << std::forward<Args>(args)) << ANSI_RESET << '\n';
	}

private:

	static const char* LevelName(const LogLevel level)
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

	static const char* LevelColor(const LogLevel level)
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

	static std::ofstream file;
	static LogLevel level;
	static std::mutex mutex;
};