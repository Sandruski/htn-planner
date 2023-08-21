#pragma once

#include <format>
#include <iostream>

#ifdef HTN_DEBUG
#define LOG(Message, ...) std::cout << std::format("{}({}): {}", __func__, __LINE__, std::format(Message, __VA_ARGS__)) << std::endl;
#define CLOG(Condition, Message, ...)                                                                                                                \
    if (Condition)                                                                                                                                   \
    LOG(Message, __VA_ARGS__)
#define LOG_ERROR(Message, ...) std::cout << std::format("{}({}): error: {}", __func__, __LINE__, std::format(Message, __VA_ARGS__)) << std::endl;
#define CLOG_ERROR(Condition, Message, ...)                                                                                                          \
    if (Condition)                                                                                                                                   \
    LOG_ERROR(Message, __VA_ARGS__)
#define LOG_HTN(Row, Column, Message, ...) std::cout << std::format("({},{}): {}", Row, Column, std::format(Message, __VA_ARGS__)) << std::endl;
#define CLOG_HTN(Condition, Row, Column, Message, ...)                                                                                               \
    if (Condition)                                                                                                                                   \
    LOG_HTN(Row, Column, Message, __VA_ARGS__)
#define LOG_HTN_ERROR(Row, Column, Message, ...)                                                                                                     \
    std::cout << std::format("({},{}): error: {}", Row, Column, std::format(Message, __VA_ARGS__)) << std::endl;
#define CLOG_HTN_ERROR(Condition, Row, Column, Message, ...)                                                                                         \
    if (Condition)                                                                                                                                   \
    LOG_HTN_ERROR(Row, Column, Message, __VA_ARGS__)
#else
#define LOG(Message, ...)
#define CLOG(Condition, Message, ...)
#define LOG_ERROR(Message, ...)
#define CLOG_ERROR(Condition, Message, ...)
#define LOG_HTN(Row, Column, Message, ...)
#define CLOG_HTN(Condition, Row, Column, Message, ...)
#define LOG_HTN_ERROR(Row, Column, Message, ...)
#define CLOG_HTN_ERROR(Condition, Row, Column, Message, ...)
#endif
