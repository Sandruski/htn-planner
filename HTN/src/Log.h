#pragma once

#include <format>
#include <iostream>

#ifdef HTN_DEBUG
#define LOG(Text, ...) std::cout << std::format(Text, __VA_ARGS__) << std::endl;
#define LOG_ERROR(Text, ...) std::cout << std::format("error: {}", std::format(Text, __VA_ARGS__)) << std::endl;
#define LOG_CERROR(Condition, Text, ...) \
	if (Condition) \
		std::cout << std::format("{}({}): error: {}", __FUNCTION__, __LINE__, std::format(Text, __VA_ARGS__)) << std::endl;
#define LOG_HTN_ERROR(Row, Column, Text, ...) std::cout << std::format("({},{}): error: {}", Row, Column, std::format(Text, __VA_ARGS__)) << std::endl;
#else
#define LOG(Text, ...)
#define LOG_HTN_ERROR(Row, Column, Text, ...)
#define LOG_CERROR(Condition, Text, ...)
#endif
