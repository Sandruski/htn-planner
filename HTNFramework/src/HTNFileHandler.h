// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNFrameworkMinimal.h"
#include "HTNUncopyable.h"

#include <string>
#include <fstream>

/**
 * Helper that uses RAII to automatically open/close a file to perform operations on it such as reading it
 */
class HTNFileHandler : private HTNUncopyable
{
public:
    explicit HTNFileHandler(const std::string& inFilePath);
    ~HTNFileHandler();

    bool ReadFile(std::string& outFileText) const;

private:
    std::ifstream mFile;
};
