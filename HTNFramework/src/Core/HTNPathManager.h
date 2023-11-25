// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>

class HTNPathManager
{
public:
    HTNPathManager() = default;
    explicit HTNPathManager(const std::string& inPath);

    HTN_NODISCARD bool TryPushSegmentToPath(const std::string& inSegment);
    HTN_NODISCARD bool TryPopSegmentFromPath();

    HTN_NODISCARD bool  FindLastPathSegment(std::string& outSegment) const;
    HTN_NODISCARD const std::string& GetPath() const;

private:
    std::string mPath;
};

inline const std::string& HTNPathManager::GetPath() const
{
    return mPath;
}
