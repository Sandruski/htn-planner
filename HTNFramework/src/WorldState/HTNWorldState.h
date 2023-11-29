// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "Core/HTNAtom.h"
#include "HTNCoreMinimal.h"
#include "WorldState/HTNWorldStateFwd.h"
#include "WorldState/HTNWorldStateHelpers.h"

#include <cassert>
#include <iterator>
#include <string>

#ifdef HTN_ENABLE_LOGGING
#include <format>
#endif

/**
 * Collection of fact arguments
 * - The table has a fixed number of fact arguments
 * - Each row represents a group of fact arguments, and each column represents a single fact argument within its group
 * - Each group of fact arguments can only appear once in the table
 */
class HTNFactArgumentsTable
{
public:
    // Adds a unique row with the given fact arguments to the table
    template<typename T>
    bool AddUniqueFactArguments(const T& inFactArguments);

    // Removes the row at the given index from the table
    void RemoveFactArguments(const size inFactArgumentsIndex);

    // Removes all rows from the table
    void RemoveAllFactArguments();

    // Checks if the given fact arguments match the fact arguments of the row at the given index
    // Binds the fact arguments that are unbound
    template<typename T>
    HTN_NODISCARD bool Check(const size inFactArgumentsIndex, T& ioFactArguments) const;

    // Checks if the given fact arguments match the fact arguments of any row
    template<typename T>
    HTN_NODISCARD bool ContainsFactArguments(const T& inFactArguments) const;

    // Returns the fact arguments collection
    HTN_NODISCARD const HTNFactArgumentsCollection& GetFactArgumentsCollection() const;

    // Returns the number of entries in the table
    HTN_NODISCARD size GetFactArgumentsCollectionSize() const;

private:
    // Adds a new row with the given fact arguments to the table
    template<typename T>
    void AddFactArguments(const T& inFactArguments);

    HTNFactArgumentsCollection mFactArgumentsCollection;
};

/**
 * Database of facts that represent the knowledge about the world
 * - Each fact ID can be mapped to multiple fact arguments tables, but it can only contain one fact argument table for each number of fact arguments
 */
class HTNWorldState
{
public:
    // Adds a unique row with the given fact arguments to the corresponding table
    template<typename T>
    void AddFact(const std::string& inFactID, const T& inFactArguments);

    // Removes the row at the given index from the corresponding table
    void RemoveFact(const std::string& inFactID, const size inFactArgumentsSize, const size inFactArgumentsIndex);

    // Removes all facts
    void RemoveAllFacts();

    // If all fact arguments are bound, returns 1
    // Otherwise, returns the number of rows of the corresponding table
    template<typename T>
    HTN_NODISCARD size Query(const std::string& inFactID, const T& inFactArguments) const;

    // If all fact arguments are bound, returns true
    // Otherwise, checks the given fact arguments with the fact arguments of the row at the given index of the corresponding table
    template<typename T>
    HTN_NODISCARD bool QueryIndex(const std::string& inFactID, const size inFactArgumentsIndex, T& ioFactArguments) const;

    // Checks the given fact arguments with the fact arguments of the row at the given index of the corresponding table
    template<typename T>
    HTN_NODISCARD bool CheckIndex(const std::string& inFactID, const size inFactArgumentsIndex, T& ioFactArguments) const;

    // Returns the number of tables associated to the given fact ID
    HTN_NODISCARD size GetFactArgumentsTablesSize(const std::string& inFactID) const;

    // Returns whether the table of the given number of fact arguments is associated to the given fact ID
    HTN_NODISCARD bool ContainsFactArgumentsTable(const std::string& inFactID, const size inFactArgumentsSize) const;

    // Returns the number of rows of the given table
    HTN_NODISCARD size GetFactArgumentsCollectionSize(const std::string& inFactID, const size inFactArgumentsSize) const;

    // Returns whether the given fact arguments are contained by the table associated to the given fact ID
    template<typename T>
    HTN_NODISCARD bool ContainsFactArguments(const std::string& inFactID, const T& inFactArguments) const;

    // Returns the facts
    HTN_NODISCARD const HTNFacts& GetFacts() const;

private:
    HTNFacts mFacts;
};

template<typename T>
bool HTNFactArgumentsTable::AddUniqueFactArguments(const T& inFactArguments)
{
    if (ContainsFactArguments(inFactArguments))
    {
#ifdef HTN_ENABLE_LOGGING
        std::string FactArgumentsDescription;

        const T::const_iterator ItEnd = std::cend(inFactArguments);
        for (auto It = std::cbegin(inFactArguments); It != ItEnd; ++It)
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

    AddFactArguments(inFactArguments);

    return true;
}

inline void HTNFactArgumentsTable::RemoveAllFactArguments()
{
    mFactArgumentsCollection.clear();
}

template<typename T>
bool HTNFactArgumentsTable::Check(const size inFactArgumentsIndex, T& ioFactArguments) const
{
    if (inFactArgumentsIndex >= mFactArgumentsCollection.size())
    {
        HTN_LOG_ERROR("Fact arguments index [{}] outside of bounds [{}]", inFactArgumentsIndex, mFactArgumentsCollection.size());
        return false;
    }

    const HTNFactArguments& FactArguments     = mFactArgumentsCollection[inFactArgumentsIndex];
    const T::iterator       ItBegin           = std::begin(ioFactArguments);
    const size              FactArgumentsSize = std::distance(ItBegin, std::end(ioFactArguments));
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsSize);

    // Check bound arguments
    for (size i = 0; i < FactArgumentsSize; ++i)
    {
        auto It = ItBegin;
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

    // Bind unbound arguments
    for (size i = 0; i < FactArgumentsSize; ++i)
    {
        auto It = ItBegin;
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
bool HTNFactArgumentsTable::ContainsFactArguments(const T& inFactArguments) const
{
    const T::const_iterator ItBegin           = std::cbegin(inFactArguments);
    const size              FactArgumentsSize = std::distance(ItBegin, std::cend(inFactArguments));
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsSize);

    for (const HTNFactArguments& FactArguments : mFactArgumentsCollection)
    {
        bool Result = true;
        for (size i = 0; i < FactArgumentsSize; ++i)
        {
            const HTNAtom& FactArgument = FactArguments[i];

            auto It = ItBegin;
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

inline const HTNFactArgumentsCollection& HTNFactArgumentsTable::GetFactArgumentsCollection() const
{
    return mFactArgumentsCollection;
}

inline size HTNFactArgumentsTable::GetFactArgumentsCollectionSize() const
{
    return mFactArgumentsCollection.size();
}

template<typename T>
void HTNFactArgumentsTable::AddFactArguments(const T& inFactArguments)
{
    const T::const_iterator ItBegin           = std::cbegin(inFactArguments);
    const size              FactArgumentsSize = std::distance(ItBegin, std::cend(inFactArguments));
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsSize);

    HTNFactArguments FactArguments;
    for (size i = 0; i < FactArgumentsSize; ++i)
    {
        auto It = ItBegin;
        std::advance(It, i);

        FactArguments[i] = *It;
    }

    mFactArgumentsCollection.emplace_back(FactArguments);
}

template<typename T>
void HTNWorldState::AddFact(const std::string& inFactID, const T& inFactArguments)
{
    HTNFactArgumentsTables& FactArgumentsTables = mFacts[inFactID];
    const size              FactArgumentsSize   = std::distance(std::cbegin(inFactArguments), std::cend(inFactArguments));
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsSize);

    HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[FactArgumentsSize];
    FactArgumentsTable.AddUniqueFactArguments(inFactArguments);
}

inline void HTNWorldState::RemoveAllFacts()
{
    mFacts.clear();
}

template<typename T>
size HTNWorldState::Query(const std::string& inFactID, const T& inFactArguments) const
{
    const uint32 FactArgumentsBoundNum = HTNWorldStateHelpers::CountFactArgumentsBound(inFactArguments);
    const size   FactArgumentsSize     = std::distance(std::cbegin(inFactArguments), std::cend(inFactArguments));
    if (FactArgumentsBoundNum == FactArgumentsSize)
    {
        // All arguments are bound
        return 1;
    }

    return GetFactArgumentsCollectionSize(inFactID, FactArgumentsSize);
}

template<typename T>
bool HTNWorldState::QueryIndex(const std::string& inFactID, const size inFactArgumentsIndex, T& ioFactArguments) const
{
    const uint32 FactArgumentsBoundNum = HTNWorldStateHelpers::CountFactArgumentsBound(ioFactArguments);
    const size   FactArgumentsSize     = std::distance(std::cbegin(ioFactArguments), std::cend(ioFactArguments));
    if (FactArgumentsBoundNum == FactArgumentsSize)
    {
        // All arguments are bound
        return true;
    }

    return CheckIndex(inFactID, inFactArgumentsIndex, ioFactArguments);
}

template<typename T>
bool HTNWorldState::CheckIndex(const std::string& inFactID, const size inFactArgumentsIndex, T& ioFactArguments) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.cend())
    {
        HTN_LOG_ERROR("Fact [{}] could not be found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    const size                    FactArgumentsSize   = std::distance(std::cbegin(ioFactArguments), std::cend(ioFactArguments));
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsSize);

    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[FactArgumentsSize];
    return FactArgumentsTable.Check(inFactArgumentsIndex, ioFactArguments);
}

template<typename T>
bool HTNWorldState::ContainsFactArguments(const std::string& inFactID, const T& inFactArguments) const
{
    const auto It = mFacts.find(inFactID);
    if (It == mFacts.cend())
    {
        HTN_LOG_ERROR("Fact [{}] could not be found", inFactID);
        return false;
    }

    const HTNFactArgumentsTables& FactArgumentsTables = It->second;
    const size                    FactArgumentsSize   = std::distance(std::cbegin(inFactArguments), std::cend(inFactArguments));
    assert(FactArgumentsSize < HTNWorldStateHelpers::kFactArgumentsSize);

    const HTNFactArgumentsTable& FactArgumentsTable = FactArgumentsTables[FactArgumentsSize];
    return FactArgumentsTable.ContainsFactArguments(inFactArguments);
}

inline const HTNFacts& HTNWorldState::GetFacts() const
{
    return mFacts;
}
