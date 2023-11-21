// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNFrameworkMinimal.h"

#include <string>

class HTNPathHandler
{
public:
    HTNPathHandler() = default;
    explicit HTNPathHandler(const std::string& inPath);

    bool TryPushSegmentToPath(const std::string& inSegment);
    bool TryPopSegmentFromPath();

    bool               FindLastPathSegment(std::string& outSegment) const;
    const std::string& GetPath() const;

private:
    std::string mPath;
};

inline const std::string& HTNPathHandler::GetPath() const
{
    return mPath;
}
