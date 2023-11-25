// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Core/HTNAtom.h"
#include "WorldState/HTNWorldStateHelpers.h"

#include <array>
#include <cassert>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef HTN_DEBUG
#include <format>
#endif

class HTNFactArgumentsTable;

using HTNFactArguments       = std::array<HTNAtom, HTNWorldStateHelpers::kFactArgumentsContainerSize>;
using HTNFactArgumentsTables = std::array<HTNFactArgumentsTable, HTNWorldStateHelpers::kFactArgumentsContainerSize>;
using HTNFacts = std::unordered_map<std::string, HTNFactArgumentsTables>;
using HTNFact = std::pair<const std::string, HTNFactArgumentsTables>;

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
    bool AddUniqueFactArguments(const T inFactArgumentsBegin, const T inFactArgumentsEnd);

    // Removes the row at the index from the table
    void RemoveFactArguments(const size inFactArgumentsIndex);

    // Removes all rows from the table
    void RemoveAllFactArguments();

    // Checks if the passed fact arguments match the ones of the row at the index in the table
    // Binds the passed fact arguments that are unbound
    template<typename T>
    HTN_NODISCARD bool Check(const size inFactArgumentsIndex, const T ioFactArgumentsBegin, const T ioFactArgumentsEnd) const;

    // Checks if the passed arguments match the fact arguments of any row in the table
    template<typename T>
    HTN_NODISCARD bool ContainsFactArguments(const T inFactArgumentsBegin, const T inFactArgumentsEnd) const;

    HTN_NODISCARD const std::vector<HTNFactArguments>& GetFactArguments() const;
    HTN_NODISCARD size                                 GetFactArgumentsSize() const;

private:
    // Adds a row to the table
    template<typename T>
    void AddFactArguments(const T inFactArgumentsBegin, const T inFactArgumentsEnd);

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
    void AddFact(const std::string& inFactID, const T inFactArgumentsBegin, const T inFactArgumentsEnd);

    // Removes the row at the index from the table of the number of fact arguments associated to the fact ID
    void RemoveFact(const std::string& inFactID, const size inFactArgumentsSize, const size inFactArgumentsIndex);

    // Removes all facts
    void RemoveAllFacts();

    // If all fact arguments are bound, returns 1
    // Otherwise, returns the number of rows of the table associated to the fact ID
    template<typename T>
    HTN_NODISCARD size Query(const std::string& inFactID, const T inFactArgumentsBegin, const T inFactArgumentsEnd) const;

    // If all fact arguments are bound, returns true
    // Otherwise, checks the passed fact arguments with the fact arguments of the row at the index of the table associated to the fact ID
    template<typename T>
    HTN_NODISCARD bool QueryIndex(const std::string& inFactID, const size inFactArgumentsIndex, const T ioFactArgumentsBegin,
                                 const T ioFactArgumentsEnd) const;

    // Checks the passed fact arguments with the fact arguments of the row at the index of the table associated to the fact ID
    template<typename T>
    HTN_NODISCARD bool CheckIndex(const std::string& inFactID, const size inFactArgumentsIndex, const T ioFactArgumentsBegin,
                                 const T ioFactArgumentsEnd) const;

    // Returns the number of tables associated to the fact ID
    HTN_NODISCARD size GetFactArgumentsTablesSize(const std::string& inFactID) const;

    // Returns whether the table of the number of arguments is associated to the fact ID
    HTN_NODISCARD bool ContainsFactArgumentTable(const std::string& inFactID, const size inFactArgumentsSize) const;

    // Returns the number of rows of the table of the number of arguments associated to the fact ID
    HTN_NODISCARD size GetFactArgumentsSize(const std::string& inFactID, const size inFactArgumentsSize) const;

    // Returns whether the fact arguments are contained by the fact ID
    template<typename T>
    HTN_NODISCARD bool ContainsFactArguments(const std::string& inFactID, const T inFactArgumentsBegin, const T inFactArgumentsEnd) const;

    HTN_NODISCARD const HTNFacts& GetFacts() const;

private:
    // Fact ID to fact argument tables
    HTNFacts mFacts;
};

template<typename T>
bool HTNFactArgumentsTable::AddUniqueFactArguments(const T inFactArgumentsBegin, const T inFactArgumentsEnd)
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
        const size Index = FactArgumentsDescription.find_last_of(" ");
        if (Index != std::string::npos)
        {
            FactArgumentsDescription.erase(Index);
        }
#endif
        HTN_LOG_ERROR("Fact arguments [{}] already contained", FactArgumentsDescription);
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
bool HTNFactArgumentsTable::Check(const size inFactArgumentsIndex, const T ioFactArgumentsBegin, const T ioFactArgumentsEnd) const
{
    if (inFactArgumentsIndex >= mFactArguments.size())
    {
        HTN_LOG_ERROR("Fact arguments index [{}] outside of bounds [{}]", inFactArgumentsIndex, mFactArguments.size());
        return false;
    }

    const HTNFactArguments& FactArguments     = mFactArguments[inFactArgumentsIndex];
    const size              FactArgumentsSize = std::distance(ioFactArgumentsBegin, ioFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);

    // Check arguments bound
    for (size i = 0; i < FactArgumentsSize; ++i)
    {
        auto It = ioFactArgumentsBegin;
        std::advance(It, i);

        const HTNAtom& ioFactArgument = *It;
        if (!ioFactArgument.IsBound())
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
    for (size i = 0; i < FactArgumentsSize; ++i)
    {
        auto It = ioFactArgumentsBegin;
        std::advance(It, i);

        HTNAtom& ioFactArgument = *It;
        if (ioFactArgument.IsBound())
        {
            continue;
        }

        const HTNAtom& FactArgument = FactArguments[i];
        ioFactArgument              = FactArgument;
    }

    return true;
}

template<typename T>
bool HTNFactArgumentsTable::ContainsFactArguments(const T inFactArgumentsBegin, const T inFactArgumentsEnd) const
{
    const size FactArgumentsSize = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    for (const HTNFactArguments& FactArguments : mFactArguments)
    {
        bool Result = true;
        for (size i = 0; i < FactArgumentsSize; ++i)
        {
            const HTNAtom& FactArgument = FactArguments[i];

            auto It = inFactArgumentsBegin;
            std::advance(It, i);

            const HTNAtom& inFactArgument = *It;
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

inline size HTNFactArgumentsTable::GetFactArgumentsSize() const
{
    return mFactArguments.size();
}

template<typename T>
void HTNFactArgumentsTable::AddFactArguments(const T inFactArgumentsBegin, const T inFactArgumentsEnd)
{
    HTNFactArguments FactArguments;
    const size       FactArgumentsSize = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    for (size i = 0; i < FactArgumentsSize; ++i)
    {
        auto It = inFactArgumentsBegin;
        std::advance(It, i);

        FactArguments[i] = *It;
    }

    mFactArguments.emplace_back(FactArguments);
}

template<typename T>
void HTNWorldState::AddFact(const std::string& inFactID, const T inFactArgumentsBegin, const T inFactArgumentsEnd)
{
    HTNFactArgumentsTables& FactArgumentsTables = mFacts[inFactID];
    const size              FactArgumentsSize   = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[FactArgumentsSize];
    FactArgumentsTable.AddUniqueFactArguments(inFactArgumentsBegin, inFactArgumentsEnd);
}

inline void HTNWorldState::RemoveAllFacts()
{
    mFacts.clear();
}

template<typename T>
size HTNWorldState::Query(const std::string& inFactID, const T inFactArgumentsBegin, const T inFactArgumentsEnd) const
{
    const uint32 FactArgumentsBoundNum = HTNWorldStateHelpers::CountFactArgumentsBound(inFactArgumentsBegin, inFactArgumentsEnd);
    const size   FactArgumentsSize     = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    if (FactArgumentsBoundNum == FactArgumentsSize)
    {
        // All arguments are bound
        return 1;
    }

    return GetFactArgumentsSize(inFactID, FactArgumentsSize);
}

template<typename T>
bool HTNWorldState::QueryIndex(const std::string& inFactID, const size inFactArgumentsIndex, const T ioFactArgumentsBegin,
                               const T ioFactArgumentsEnd) const
{
    const uint32 FactArgumentsBoundNum = CountFactArgumentsBound(ioFactArgumentsBegin, ioFactArgumentsEnd);
    const size   FactArgumentsSize     = std::distance(ioFactArgumentsBegin, ioFactArgumentsEnd);
    if (FactArgumentsBoundNum == FactArgumentsSize)
    {
        // All arguments are bound
        return true;
    }

    return CheckIndex(inFactID, inFactArgumentsIndex, ioFactArgumentsBegin, ioFactArgumentsEnd);
}

template<typename T>
bool HTNWorldState::CheckIndex(const std::string& inFactID, const size inFactArgumentsIndex, const T ioFactArgumentsBegin,
                               const T ioFactArgumentsEnd) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        HTN_LOG_ERROR("Fact [{}] not found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    const size                    FactArgumentsSize   = std::distance(ioFactArgumentsBegin, ioFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[FactArgumentsSize];
    return FactArgumentsTable.Check(inFactArgumentsIndex, ioFactArgumentsBegin, ioFactArgumentsEnd);
}

template<typename T>
bool HTNWorldState::ContainsFactArguments(const std::string& inFactID, const T inFactArgumentsBegin, const T inFactArgumentsEnd) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.end())
    {
        HTN_LOG_ERROR("Fact [{}] not found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    const size                    FactArgumentsSize   = std::distance(inFactArgumentsBegin, inFactArgumentsEnd);
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsContainerSize);
    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[FactArgumentsSize];
    return FactArgumentsTable.ContainsFactArguments(inFactArgumentsBegin, inFactArgumentsEnd);
}

inline const HTNFacts& HTNWorldState::GetFacts() const
{
    return mFacts;
}
