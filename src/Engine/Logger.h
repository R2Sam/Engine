#pragma once

#include "Log/Log.h"
#include "Types.h"

#include <fstream>
#include <mutex>

/**
 * @brief Console and file logging levels
 */

enum class LogLevel : u8
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

	static void SetLogLevel(const LogLevel levelIn);

	/**
	 * @brief Sets the output file
	 *
	 * If a file is already opened it is closed.
	 *
	 * @param path Path to log file, empty string disables file output
	 */

	static void SetLogFile(const char* path);

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

	template <LogLevel levelIn = LogLevel::DEBUG, typename... Args>
	static void Write(Args&&... args)
	{
		if (levelIn < s_level)
		{
			return;
		}

		std::lock_guard<std::mutex> lock(s_mutex);

		if (s_file.is_open())
		{
			s_file << "[" << GetCurrentTimeString() << "] " << "[" << LevelName(s_level) << "] ";
			(s_file << ... << std::forward<Args>(args)) << '\n';
		}

		std::cerr << LevelColor(s_level) << "[" << GetCurrentTimeString() << "] " << "[" << LevelName(s_level) << "] ";
		(std::cerr << ... << std::forward<Args>(args)) << ANSI_RESET << '\n';
	}

private:

	static const char* LevelName(const LogLevel level);

	static const char* LevelColor(const LogLevel level);

	inline static std::ofstream s_file;
	inline static LogLevel s_level = LogLevel::DEBUG;
	inline static std::mutex s_mutex;
};