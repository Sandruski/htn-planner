// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "HTNUncopyable.h"

#include <fstream>
#include <string>

/**
 * Helper that uses RAII to automatically open/close a file to perform operations on it such as reading it
 */
class HTNFileReader : private HTNUncopyable
{
public:
    explicit HTNFileReader(const std::string& inFilePath);
    ~HTNFileReader();

    HTN_NODISCARD bool ReadFile(std::string& outFileText) const;

private:
    std::ifstream mFile;
};
