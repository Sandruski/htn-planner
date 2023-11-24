// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>

class HTNPathHandler
{
public:
    HTNPathHandler() = default;
    explicit HTNPathHandler(const std::string& inPath);

    HTN_NODISCARD bool TryPushSegmentToPath(const std::string& inSegment);
    HTN_NODISCARD bool TryPopSegmentFromPath();

    HTN_NODISCARD bool  FindLastPathSegment(std::string& outSegment) const;
    HTN_NODISCARD const std::string& GetPath() const;

private:
    std::string mPath;
};

inline const std::string& HTNPathHandler::GetPath() const
{
    return mPath;
}
