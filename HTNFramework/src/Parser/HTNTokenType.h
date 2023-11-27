// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

enum class HTNTokenType : uint8
{
    COLON,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    EXCLAMATION_MARK,
    QUESTION_MARK,
    HASH,
    AT,

    // Keywords
    HTN_DOMAIN,
    HTN_TOP_LEVEL_DOMAIN,
    HTN_METHOD,
    HTN_TOP_LEVEL_METHOD,
    HTN_AXIOM,
    HTN_CONSTANTS,
    AND,
    OR,
    ALT,
    NOT,
    TRUE,
    FALSE,

    IDENTIFIER,
    NUMBER,
    STRING,

    END_OF_FILE
};
