#pragma once

#ifdef HTN_DEBUG
#include <format>
#include <iostream>
#endif

#ifdef HTN_DEBUG
#define HTN_LOG(Message, ...) std::cout << std::format("{}({}): {}", __func__, __LINE__, std::format(Message, __VA_ARGS__)) << std::endl;
#define HTN_CLOG(Condition, Message, ...)                                                                                                            \
    if (Condition)                                                                                                                                   \
    HTN_LOG(Message, __VA_ARGS__)
#define HTN_LOG_ERROR(Message, ...) std::cout << std::format("{}({}): error: {}", __func__, __LINE__, std::format(Message, __VA_ARGS__)) << std::endl;
#define HTN_CLOG_ERROR(Condition, Message, ...)                                                                                                      \
    if (Condition)                                                                                                                                   \
    HTN_LOG_ERROR(Message, __VA_ARGS__)
#else
#define HTN_LOG(Message, ...)
#define HTN_CLOG(Condition, Message, ...)
#define HTN_LOG_ERROR(Message, ...)
#define HTN_CLOG_ERROR(Condition, Message, ...)
#endif
