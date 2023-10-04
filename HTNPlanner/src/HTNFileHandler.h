#pragma once

#include <string>
#include <fstream>

/**
 * Helper that uses RAII to automatically open/close a file to perform operations on it such as reading it
 */
class HTNFileHandler
{
public:
    HTNFileHandler(const std::string& inFilePath);
    ~HTNFileHandler();

    bool ReadFile(std::string& outFileText) const;

private:
    std::ifstream mFile;
};