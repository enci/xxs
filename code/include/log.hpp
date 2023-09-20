#pragma once
#include <string>
#include <format>
#include <iostream>
#include <string_view>

namespace xxs::log
{
	template<typename... Args>
	void info(std::string_view rt_fmt_str, Args&... args);

    template <typename... Args>
    void warn(std::string_view rt_fmt_str, Args&&... args);

    template <typename... Args>
    void error(std::string_view rt_fmt_str, Args&&... args);
}

template<typename... Args>
inline void xxs::log::info(std::string_view rt_fmt_str, Args&... args)
{
    auto fmt = std::make_format_args(args...);
    auto s = std::vformat(rt_fmt_str, std::make_format_args(args...));
    std::cout << "[info] " << s << std::endl;
}

template<typename... Args>
inline void xxs::log::warn(std::string_view rt_fmt_str, Args&&... args)
{
    auto fmt = std::make_format_args(args...);
    auto s = std::vformat(rt_fmt_str, std::make_format_args(args...));
    std::cout << "[warn] " << s << std::endl;
}

template<typename... Args>
inline void xxs::log::error(std::string_view rt_fmt_str, Args&&... args)
{
    auto fmt = std::make_format_args(args...);
    auto s = std::vformat(rt_fmt_str, std::make_format_args(args...));
    std::cout << "[error] " << s << std::endl;
}
