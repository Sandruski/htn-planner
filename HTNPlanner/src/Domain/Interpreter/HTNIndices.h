#pragma once

#include <limits>
#include <string>
#include <unordered_map>

class HTNIndices
{
public:
    // Adds a new index
    std::size_t AddIndex(const std::string& inNodePath);

    // Increments an existing index or adds a new index and increments it
    std::size_t IncrementIndex(const std::string& inNodePath);

    // Adds a new index or increments an existing index
    std::size_t AddOrIncrementIndex(const std::string& inNodePath);

    // Removes an existing index
    void RemoveIndex(const std::string& inNodePath);

    // Returns an existing index or an invalid one if not existing
    std::size_t GetIndex(const std::string& inNodePath) const;

    const std::unordered_map<std::string, std::size_t>& GetIndices() const;

private:
    // Node path to index
    // - Method to branch
    // - And, or, and alt conditions to sub-condition
    // - Condition to fact entry
    std::unordered_map<std::string, std::size_t> mIndices;
};

inline std::size_t HTNIndices::AddIndex(const std::string& inNodePath)
{
    return mIndices[inNodePath];
}

inline std::size_t HTNIndices::IncrementIndex(const std::string& inNodePath)
{
    return ++mIndices[inNodePath];
}

inline std::size_t HTNIndices::AddOrIncrementIndex(const std::string& inNodePath)
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

inline std::size_t HTNIndices::GetIndex(const std::string& inNodePath) const
{
    const auto It = mIndices.find(inNodePath);
    if (It == mIndices.end())
    {
        return std::numeric_limits<std::size_t>::max();
    }

    return It->second;
}

inline const std::unordered_map<std::string, std::size_t>& HTNIndices::GetIndices() const
{
    return mIndices;
}
