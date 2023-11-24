// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <limits>
#include <string>
#include <unordered_map>

class HTNIndices
{
public:
    // Adds a new index
    size AddIndex(const std::string& inNodePath);

    // Increments an existing index or adds a new index and increments it
    size IncrementIndex(const std::string& inNodePath);

    // Adds a new index or increments an existing index
    size AddOrIncrementIndex(const std::string& inNodePath);

    // Removes an existing index
    void RemoveIndex(const std::string& inNodePath);

    // Returns an existing index or an invalid one if not existing
    size GetIndex(const std::string& inNodePath) const;

    const std::unordered_map<std::string, size>& GetIndices() const;

private:
    // Node path to index
    // - Method to branch
    // - And, or, and alt conditions to sub-condition
    // - Condition to fact entry
    std::unordered_map<std::string, size> mIndices;
};

inline size HTNIndices::AddIndex(const std::string& inNodePath)
{
    return mIndices[inNodePath];
}

inline size HTNIndices::IncrementIndex(const std::string& inNodePath)
{
    return ++mIndices[inNodePath];
}

inline size HTNIndices::AddOrIncrementIndex(const std::string& inNodePath)
{
    const auto It = mIndices.find(inNodePath);
    if (It == mIndices.end())
    {
        return AddIndex(inNodePath);
    }

    return IncrementIndex(inNodePath);
}

inline void HTNIndices::RemoveIndex(const std::string& inNodePath)
{
    mIndices.erase(inNodePath);
}

inline size HTNIndices::GetIndex(const std::string& inNodePath) const
{
    const auto It = mIndices.find(inNodePath);
    return (It != mIndices.end()) ? It->second : std::numeric_limits<size>::max();
}

inline const std::unordered_map<std::string, size>& HTNIndices::GetIndices() const
{
    return mIndices;
}
