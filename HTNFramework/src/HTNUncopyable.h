// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNFrameworkMinimal.h"

// Base class to prevent copying
class HTNUncopyable
{
protected:
    // Allow construction and destruction
    HTNUncopyable()  = default;
    ~HTNUncopyable() = default;

    // Prevent copying
    HTNUncopyable(const HTNUncopyable& inOther)            = delete;
    HTNUncopyable& operator=(const HTNUncopyable& inOther) = delete;
};
