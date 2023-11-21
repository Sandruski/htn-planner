#pragma once

#ifdef HTN_DEBUG
#include <format>
#include <iostream>
#endif

#ifdef HTN_DEBUG
#define HTN_DOMAIN_LOG(Row, Column, Message, ...)                                                                                                    \
    std::cout << std::format("({},{}): {}", Row, Column, std::format(Message, __VA_ARGS__)) << std::endl;
#define HTN_DOMAIN_CLOG(Condition, Row, Column, Message, ...)                                                                                        \
    if (Condition)                                                                                                                                   \
    HTN_DOMAIN_LOG(Row, Column, Message, __VA_ARGS__)
#define HTN_DOMAIN_LOG_ERROR(Row, Column, Message, ...)                                                                                              \
    std::cout << std::format("({},{}): error: {}", Row, Column, std::format(Message, __VA_ARGS__)) << std::endl;
#define HTN_DOMAIN_CLOG_ERROR(Condition, Row, Column, Message, ...)                                                                                  \
    if (Condition)                                                                                                                                   \
    HTN_DOMAIN_LOG_ERROR(Row, Column, Message, __VA_ARGS__)
#else
#define HTN_DOMAIN_LOG(Row, Column, Message, ...)
#define HTN_DOMAIN_CLOG(Condition, Row, Column, Message, ...)
#define HTN_DOMAIN_LOG_ERROR(Row, Column, Message, ...)
#define HTN_DOMAIN_CLOG_ERROR(Condition, Row, Column, Message, ...)
#endif
