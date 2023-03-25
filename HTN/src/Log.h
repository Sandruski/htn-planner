#pragma once

#include <format>
#include <iostream>

#define LOG(Text, ...) std::cout << std::format(Text, __VA_ARGS__) << std::endl;
