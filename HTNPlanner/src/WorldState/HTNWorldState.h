#pragma once

#include "HTNAtom.h"
#include "HTNLog.h"
#include "WorldState/HTNWorldStateHelpers.h"

#include <array>
#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef HTN_DEBUG
#include <format>
#endif

class HTNFactArgumentsTable;

using HTNFactArguments       = std::array<HTNAtom, HTNWorldStateHelpers::kFactArgumentsContainerSize>;
using HTNFactArgumentsTables = std::array<HTNFactArgumentsTable, HTNWorldStateHelpers::kFactArgumentsContainerSize>;

/**
 * Table of fact arguments mapped to a fact ID in the world state
 * - The table has a fixed number of fact arguments
 * - Each row of the table represents a group of fact arguments, and each column represents a single fact argument within its group
 * - Each group of fact arguments can only be contained once in the table
 */
class HTNFactArgumentsTable
{
public:
    // Adds a unique row to the table
    template<typename T>
    bool AddUniqueFactArguments(const T& inFactArgumentsBegin, const T& inFactArgumentsEnd);

    // Removes the row at the index from the table
    void RemoveFactArguments(const std::size_t inFactArgumentsIndex);

    // Removes all rows from the table
    void RemoveAllFactArguments();

    // Checks if the passed fact arguments match the ones of the row at the index in the table
    // Binds the passed fact arguments that are unbound
    template<typename T>
    bool Check(const std::size_t inFactArgumentsIndex, const T& ioFactArgumentsBegin, const T& ioFactArgumentsEnd) const;

    // Checks if the passed arguments match the fact arguments of any row in the table
    template<typename T>
    bool ContainsFactArguments(const T& inFactArgumentsBegin, const T& inFactArgumentsEnd) const;

    const std::vector<HTNFactArguments>& GetFactArguments() const;
    std::size_t                          GetFactArgumentsSize() const;

private:
    // Adds a row to the table
    template<typename T>
    void AddFactArguments(const T& inFactArgumentsBegin, const T& inFactArgumentsEnd);

    std::vector<HTNFactArguments> mFactArguments;
};

/**
 * Database of facts used for knowledge representation
 * Map of fact IDs to fact tables
 * - Each fact ID can contain multiple fact tables, but it can only contain once a fact table of a number of fact arguments
 */
class HTNWorldState
{
public:
    // Adds a unique row to the table associated to the fact ID
    template<typename T>
    void AddFact(const std::string& inFactID, const T& inFactArgumentsBegin, const T& inFactArgumentsEnd);

    // Removes the row at the index from the table of the number of fact arguments associated to the fact ID
    void RemoveFact(const std::string& inFactID, const std::size_t inFactArgumentsSize, const std::size_t inFactArgumentsIndex);

    // Removes all facts
    void RemoveAllFacts();

    // If all fact arguments are bound, returns 1
    // Otherwise, returns the number of rows of the table associated to the fact ID
    template<typename T>
    std::size_t Query(const std::string& inFactID, const T& inFactArgumentsBegin, const T& inFactArgumentsEnd) const;

    // If all fact arguments are bound, returns true
    // Otherwise, checks the passed fact arguments with the fact arguments of the row at the index of the table associated to the fact ID
    template<typename T>
    bool QueryIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, const T& ioFactArgumentsBegin,
                    const T& ioFactArgumentsEnd) const;

    // Checks the passed fact arguments with the fact arguments of the row at the index of the table associated to the fact ID
    template<typename T>
    bool CheckIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, const T& ioFactArgumentsBegin,
                    const T& ioFactArgumentsEnd) const;

    // Returns the number of tables associated to the fact ID
    std::size_t GetFactArgumentsTablesSize(const std::string& inFactID) const;

    // Returns whether the table of the number of arguments is associated to the fact ID
    bool ContainsFactArgumentTable(const std::string& inFactID, const std::size_t inFactArgumentsSize) const;

    // Returns the number of rows of the table of the number of arguments associated to the fact ID
    std::size_t GetFactArgumentsSize(const std::string& inFactID, const std::size_t inFactArgumentsSize) const;

    // Returns whether the fact arguments are contained by the fact ID
    template<typename T>
    bool ContainsFactArguments(const std::string& inFactID, const T& inFactArgumentsBegin, const T& inFactArgumentsEnd) const;

    const std::unordered_map<std::string, HTNFactArgumentsTables>& GetFacts() const;

private:
    // Fact ID to fact argument tables
    std::unordered_map<std::string, HTNFactArgumentsTables> mFacts;
};

template<typename T>
inline bool HTNFactArgumentsTable::AddUniqueFactArguments(const T& inFactArgumentsBegin, const T& inFactArgumentsEnd)
{
    if (ContainsFactArguments(inFactArgumentsBegin, inFactArgumentsEnd))
    {
#ifdef HTN_DEBUG
        std::string FactArgumentsDescription;

        for (auto It = inFactArgumentsBegin; It != inFactArgumentsEnd; ++It)
        {
            static constexpr bool ShouldDoubleQuoteString = true;
            FactArgumentsDescription.append(std::format("{} ", (*It).ToString(ShouldDoubleQuoteString)));
        }

        // Remove last whitespace
        const std::size_t Index = FactArgumentsDescription.find_last_of(" ");
        if (Index != std::string::npos)
        {
            FactArgumentsDescription.erase(Index);
        }
#endif
        LOG_ERROR("Fact arguments [{}] already contained", FactArgumentsDescription);
        return false;
    }

    AddFactArguments(inFactArgumentsBegin, inFactArgumentsEnd);

    return true;
}

inline void HTNFactArgumentsTable::RemoveAllFactArguments()
{
    mFactArguments.clear();
}

template<typename T>
inline bool HTNFactArgumentsTable::Check(const std::size_t inFactArgumentsIndex, const T& ioFactArgumentsBegin, const T& ioFactArgumentsEnd) const
{
    if (inFactArgumentsIndex >= mFactArguments.size())
    {
        LOG_ERROR("Fact arguments index [{}] outside of bounds [{}]", inFactArgumentsIndex, mFactArguments.size());
        return false;
    }

    const HTNFactArguments& FactArguments     = mFactArguments[inFactArgumentsIndex];
    const std::size_t       FactArgumentsSize = std::distance(ioFactArgumentsBegin, ioFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);

    // Check arguments bound
    for (std::size_t i = 0; i < FactArgumentsSize; ++i)
    {
        const HTNAtom& ioFactArgument = *(ioFactArgumentsBegin + i);
        if (!ioFactArgument.IsSet())
        {
            continue;
        }

        const HTNAtom& FactArgument = FactArguments[i];
        if (ioFactArgument != FactArgument)
        {
            return false;
        }
    }

    // Bind arguments unbound
    for (std::size_t i = 0; i < FactArgumentsSize; ++i)
    {
        HTNAtom& ioFactArgument = *(ioFactArgumentsBegin + i);
        if (ioFactArgument.IsSet())
        {
            continue;
        }

        const HTNAtom& FactArgument = FactArguments[i];
        ioFactArgument              = FactArgument;
    }

    return true;
}

template<typename T>
inline bool HTNFactArgumentsTable::ContainsFactArguments(const T& inFactArgumentsBegin, const T& inFactArgumentsEnd) const
{
    const std::size_t FactArgumentsSize = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    for (const HTNFactArguments& FactArguments : mFactArguments)
    {
        bool Result = true;
        for (std::size_t i = 0; i < FactArgumentsSize; ++i)
        {
            const HTNAtom& FactArgument   = FactArguments[i];
            const HTNAtom& inFactArgument = *(inFactArgumentsBegin + i);
            if (FactArgument != inFactArgument)
            {
                Result = false;
                break;
            }
        }

        if (Result)
        {
            return true;
        }
    }

    return false;
}

inline const std::vector<HTNFactArguments>& HTNFactArgumentsTable::GetFactArguments() const
{
    return mFactArguments;
}

inline std::size_t HTNFactArgumentsTable::GetFactArgumentsSize() const
{
    return mFactArguments.size();
}

template<typename T>
inline void HTNFactArgumentsTable::AddFactArguments(const T& inFactArgumentsBegin, const T& inFactArgumentsEnd)
{
    HTNFactArguments  FactArguments;
    const std::size_t FactArgumentsSize = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    for (std::size_t i = 0; i < FactArgumentsSize; ++i)
    {
        FactArguments[i] = *(inFactArgumentsBegin + i);
    }

    mFactArguments.emplace_back(FactArguments);
}

template<typename T>
inline void HTNWorldState::AddFact(const std::string& inFactID, const T& inFactArgumentsBegin, const T& inFactArgumentsEnd)
{
    HTNFactArgumentsTables& FactArgumentsTables = mFacts[inFactID];
    const std::size_t       FactArgumentsSize   = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[FactArgumentsSize];
    FactArgumentsTable.AddUniqueFactArguments(inFactArgumentsBegin, inFactArgumentsEnd);
}

inline void HTNWorldState::RemoveAllFacts()
{
    mFacts.clear();
}

template<typename T>
inline std::size_t HTNWorldState::Query(const std::string& inFactID, const T& inFactArgumentsBegin, const T& inFactArgumentsEnd) const
{
    const unsigned int FactArgumentsBoundNum = HTNWorldStateHelpers::CountFactArgumentsBound(inFactArgumentsBegin, inFactArgumentsEnd);
    const std::size_t  FactArgumentsSize     = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    if (FactArgumentsBoundNum == FactArgumentsSize)
    {
        // All arguments are bound
        return 1;
    }

    return GetFactArgumentsSize(inFactID, FactArgumentsSize);
}

template<typename T>
inline bool HTNWorldState::QueryIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, const T& ioFactArgumentsBegin,
                                      const T& ioFactArgumentsEnd) const
{
    const unsigned int FactArgumentsBoundNum = CountFactArgumentsBound(ioFactArgumentsBegin, ioFactArgumentsEnd);
    const std::size_t  FactArgumentsSize     = std::distance(ioFactArgumentsBegin, ioFactArgumentsEnd);
    if (FactArgumentsBoundNum == FactArgumentsSize)
    {
        // All arguments are bound
        return true;
    }

    return CheckIndex(inFactID, inFactArgumentsIndex, ioFactArgumentsBegin, ioFactArgumentsEnd);
}

template<typename T>
inline bool HTNWorldState::CheckIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, const T& ioFactArgumentsBegin,
                                      const T& ioFactArgumentsEnd) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        LOG_ERROR("Fact [{}] not found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    const std::size_t             FactArgumentsSize   = std::distance(ioFactArgumentsBegin, ioFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[FactArgumentsSize];
    return FactArgumentsTable.Check(inFactArgumentsIndex, ioFactArgumentsBegin, ioFactArgumentsEnd);
}

template<typename T>
inline bool HTNWorldState::ContainsFactArguments(const std::string& inFactID, const T& inFactArgumentsBegin, const T& inFactArgumentsEnd) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        LOG_ERROR("Fact [{}] not found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    const std::size_t             FactArgumentsSize   = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[FactArgumentsSize];
    return FactArgumentsTable.ContainsFactArguments(inFactArgumentsBegin, inFactArgumentsEnd);
}

inline const std::unordered_map<std::string, HTNFactArgumentsTables>& HTNWorldState::GetFacts() const
{
    return mFacts;
}
