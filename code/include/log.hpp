#pragma once
#include <string>
#include <iostream>
#include <spdlog/spdlog.h>

namespace xxs::log
{
    static void initialize();
	
	template<typename FormatString, typename... Args>
	static void info(const FormatString& fmt, const Args&... args);

	template<typename FormatString, typename... Args>
	static void warn(const FormatString& fmt, const Args&... args);

	template<typename FormatString, typename... Args>
	static void error(const FormatString& fmt, const Args&... args);

    template<typename FormatString, typename... Args>
    static void critical(const FormatString& fmt, const Args&... args);
}


template<typename FormatString, typename ...Args>
inline void xxs::log::info(const FormatString& fmt, const Args & ...args)
{
	spdlog::info(fmt, args...);
}

template<typename FormatString, typename ...Args>
inline void xxs::log::warn(const FormatString& fmt, const Args & ...args)
{
	spdlog::warn(fmt, args...);
}

template<typename FormatString, typename ...Args>
inline void xxs::log::error(const FormatString& fmt, const Args & ...args)
{
	spdlog::error(fmt, args...);
}

template<typename FormatString, typename ...Args>
inline void xxs::log::critical(const FormatString& fmt, const Args & ...args)
{
    spdlog::critical(fmt, args...);
}
