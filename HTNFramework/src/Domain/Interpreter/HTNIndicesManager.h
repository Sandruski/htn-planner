// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>
#include <unordered_map>

// Node path to element index
using HTNIndices = std::unordered_map<std::string, size>;
using HTNIndex   = std::pair<const std::string, size>;

class HTNIndicesManager
{
public:
    // Adds a new index
    HTN_NODISCARD size AddIndex(const std::string& inNodePath);

    // Increments an existing index or adds a new index and increments it
    size IncrementIndex(const std::string& inNodePath);

    // Adds a new index or increments an existing index
    HTN_NODISCARD size AddOrIncrementIndex(const std::string& inNodePath);

    // Removes an existing index
    void RemoveIndex(const std::string& inNodePath);

    // Returns an existing index or an invalid one if not existing
    HTN_NODISCARD size GetIndex(const std::string& inNodePath) const;

private:
    // - Method to branch
    // - And, or, and alt conditions to sub-condition
    // - Condition to fact entry
    HTNIndices mIndices;
};

inline size HTNIndicesManager::AddIndex(const std::string& inNodePath)
{
    return mIndices[inNodePath];
}

inline size HTNIndicesManager::IncrementIndex(const std::string& inNodePath)
{
    return ++mIndices[inNodePath];
}

inline size HTNIndicesManager::AddOrIncrementIndex(const std::string& inNodePath)
{
    const auto It = mIndices.find(inNodePath);
    if (It == mIndices.end())
    {
        return AddIndex(inNodePath);
    }

    return IncrementIndex(inNodePath);
}

inline void HTNIndicesManager::RemoveIndex(const std::string& inNodePath)
{
    mIndices.erase(inNodePath);
}

inline size HTNIndicesManager::GetIndex(const std::string& inNodePath) const
{
    const auto It = mIndices.find(inNodePath);
    return (It != mIndices.end()) ? It->second : std::numeric_limits<size>::max();
}
