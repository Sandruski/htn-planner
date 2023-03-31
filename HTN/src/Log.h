#pragma once

#include <format>
#include <iostream>

#ifdef HTN_DEBUG
#define LOG(Text, ...) std::cout << std::format(Text, __VA_ARGS__) << std::endl;
#define LOG_HTN_ERROR(Line, Column, Text, ...) std::cout << std::format("({},{}): error: {}", Line, Column, std::format(Text, __VA_ARGS__)) << std::endl;
#else
#define LOG(Text, ...)
#define LOG_HTN_ERROR(Line, Column, Text, ...)
#endif
