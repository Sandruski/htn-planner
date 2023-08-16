#pragma once

#include <format>
#include <iostream>

#ifdef HTN_DEBUG
#define LOG(Text, ...) std::cout << std::format("{}({}): {}", __func__, __LINE__, std::format(Text, __VA_ARGS__)) << std::endl;
#define CLOG(Condition, Text, ...)                                                                                                                   \
    if (Condition)                                                                                                                                   \
    LOG(Text, __VA_ARGS__)
#define LOG_ERROR(Text, ...) std::cout << std::format("{}({}): error: {}", __func__, __LINE__, std::format(Text, __VA_ARGS__)) << std::endl;
#define CLOG_ERROR(Condition, Text, ...)                                                                                                             \
    if (Condition)                                                                                                                                   \
    LOG_ERROR(Text, __VA_ARGS__)
#define LOG_HTN(Row, Column, Text, ...) std::cout << std::format("({},{}): {}", Row, Column, std::format(Text, __VA_ARGS__)) << std::endl;
#define CLOG_HTN(Condition, Row, Column, Text, ...)                                                                                                  \
    if (Condition)                                                                                                                                   \
    LOG_HTN(Row, Column, Text, __VA_ARGS__)
#define LOG_HTN_ERROR(Row, Column, Text, ...)                                                                                                        \
    std::cout << std::format("({},{}): error: {}", Row, Column, std::format(Text, __VA_ARGS__)) << std::endl;
#define CLOG_HTN_ERROR(Row, Column, Text, ...)                                                                                                       \
    if (Condition)                                                                                                                                   \
    LOG_HTN_ERROR(Row, Column, Text, __VA_ARGS__)
#else
#define LOG(Text, ...)
#define CLOG(Condition, Text, ...)
#define LOG_ERROR(Text, ...)
#define CLOG_ERROR(Condition, Text, ...)
#define LOG_HTN(Row, Column, Text, ...)
#define CLOG_HTN(Condition, Row, Column, Text, ...)
#define LOG_HTN_ERROR(Row, Column, Text, ...)
#define CLOG_HTN_ERROR(Condition, Row, Column, Text, ...)
#endif
