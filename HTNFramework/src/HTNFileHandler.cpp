// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "HTNFileHandler.h"

#include "HTNLog.h"

#include <sstream>

HTNFileHandler::HTNFileHandler(const std::string& inFilePath)
{
    mFile.open(inFilePath);
}

HTNFileHandler::~HTNFileHandler()
{
    if (!mFile.is_open())
    {
        return;
    }

    mFile.close();
}

bool HTNFileHandler::ReadFile(std::string& outFileText) const
{
    if (!mFile.is_open())
    {
        HTN_LOG_ERROR("File is not open");
        return false;
    }

    std::ostringstream Buffer;
    Buffer << mFile.rdbuf();
    outFileText = Buffer.str();

    return true;
}
