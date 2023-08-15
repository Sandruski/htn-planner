#pragma once

#include <format>
#include <iostream>

#ifdef HTN_DEBUG
#define LOG(Text, ...) std::cout << std::format(Text, __VA_ARGS__) << std::endl;
#define LOG_ERROR(Text, ...) std::cout << std::format("{}({}): error: {}", __func__, __LINE__, std::format(Text, __VA_ARGS__)) << std::endl;
#define LOG_CERROR(Condition, Text, ...)                                                                                                             \
    if (Condition)                                                                                                                                   \
    LOG_ERROR(Text, __VA_ARGS__)
#define LOG_HTN_ERROR(Row, Column, Text, ...)                                                                                                        \
    std::cout << std::format("({},{}): error: {}", Row, Column, std::format(Text, __VA_ARGS__)) << std::endl;
#define LOG_HTN_CERROR(Row, Column, Text, ...)                                                                                                       \
    if (Condition)                                                                                                                                   \
    LOG_HTN_ERROR(Row, Column, Text, __VA_ARGS__)
#else
#define LOG(Text, ...)
#define LOG_ERROR(Text, ...)
#define LOG_CERROR(Condition, Text, ...)
#define LOG_HTN_ERROR(Row, Column, Text, ...)
#define LOG_HTN_CERROR(Row, Column, Text, ...)
#endif
