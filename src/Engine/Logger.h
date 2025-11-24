#pragma once

#include "Log/Log.h"

#include <mutex>
#include <fstream>

enum class LogLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
};

class Logger
{
public:
    
    void SetLogLevel(const LogLevel level)
    {
    	std::lock_guard<std::mutex> lock(_mutex);

    	_level = level;
    }

    void SetLogFile(const char* path)
    {
    	std::lock_guard<std::mutex> lock(_mutex);

    	if(_file.is_open())
    	{
    		_file.close();
    	}

    	_file.open(path, std::ios::app);
    }

    template<typename... Args>
    void Write(const LogLevel level, Args&&... args)
    {
    	if (level < _level)
    	{
    		return;
    	}

    	std::lock_guard<std::mutex> lock(_mutex);

    	if (_file.is_open())
    	{
	        _file << "[" << GetCurrentTimeString() << "] " << "[" << LevelName(level) << "] ";
	        (_file << ... << std::forward<Args>(args)) << '\n';
	    }

        std::cerr << LevelColor(level) << "[" << GetCurrentTimeString() << "] " << "[" << LevelName(level) << "] ";
        (std::cerr << ... << std::forward<Args>(args)) << ANSI_RESET << '\n';
    }

private:

	const char* LevelName(const LogLevel level)
	{
		switch (level)
		{
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
        }

        return "UNKNOWN";
    }

    const char* LevelColor(const LogLevel level)
	{
		switch (level)
		{
            case LogLevel::DEBUG: return LOG_BLUE;
            case LogLevel::INFO: return LOG_WHITE;
            case LogLevel::WARN: return LOG_YELLOW;
            case LogLevel::ERROR: return LOG_RED;
        }

        return LOG_WHITE;
    }

private:

    std::ofstream _file;
    LogLevel _level = LogLevel::DEBUG;
    std::mutex _mutex;	
};