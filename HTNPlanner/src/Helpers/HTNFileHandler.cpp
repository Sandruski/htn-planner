#include "Helpers/HTNFileHandler.h"

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
        LOG_ERROR("File is not open");
        return false;
    }

    std::ostringstream Buffer;
    Buffer << mFile.rdbuf();
    outFileText = Buffer.str();

    return true;
}
