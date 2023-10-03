#pragma once

#include "HTNAtom.h"
#include "WorldState/HTNWorldStateHelpers.h"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

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
    bool AddUniqueFactArguments(const std::vector<HTNAtom>& inFactArguments);

    // Removes the row at the index from the table
    void RemoveFactArguments(const std::size_t inFactArgumentsIndex);

    // Removes all rows from the table
    void RemoveAllFactArguments();

    // Checks if the passed fact arguments match the ones of the row at the index in the table
    // Binds the passed fact arguments that are unbound
    bool Check(const std::size_t inFactArgumentsIndex, std::vector<HTNAtom>& ioFactArguments) const;

    // Checks if the passed arguments match the fact arguments of any row in the table
    bool ContainsFactArguments(const std::vector<HTNAtom>& inFactArguments) const;

    const std::vector<HTNFactArguments>& GetFactArguments() const;
    std::size_t                          GetFactArgumentsSize() const;

private:
    // Adds a row to the table
    void AddFactArguments(const std::vector<HTNAtom>& inFactArguments);

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
    void AddFact(const std::string& inFactID, const std::vector<HTNAtom>& inFactArguments);

    // Removes the row at the index from the table of the number of fact arguments associated to the fact ID
    void RemoveFact(const std::string& inFactID, const std::size_t inFactArgumentsSize, const std::size_t inFactArgumentsIndex);

    // Removes all facts
    void RemoveAllFacts();

    // If all fact arguments are bound, returns 1
    // Otherwise, returns the number of rows of the table associated to the fact ID
    std::size_t Query(const std::string& inFactID, const std::vector<HTNAtom>& inFactArguments) const;

    // If all fact arguments are bound, returns true
    // Otherwise, checks the passed fact arguments with the fact arguments of the row at the index of the table associated to the fact ID
    bool QueryIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, std::vector<HTNAtom>& ioFactArguments) const;

    // Checks the passed fact arguments with the fact arguments of the row at the index of the table associated to the fact ID
    bool CheckIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, std::vector<HTNAtom>& ioFactArguments) const;

    // Returns the number of tables associated to the fact ID
    std::size_t GetFactArgumentsTablesSize(const std::string& inFactID) const;

    // Returns whether the table of the number of arguments is associated to the fact ID
    bool ContainsFactArgumentTable(const std::string& inFactID, const std::size_t inFactArgumentsSize) const;

    // Returns the number of rows of the table of the number of arguments associated to the fact ID
    std::size_t GetFactArgumentsSize(const std::string& inFactID, const std::size_t inFactArgumentsSize) const;

    // Returns whether the fact arguments are contained by the fact ID
    bool ContainsFactArguments(const std::string& inFactID, const std::vector<HTNAtom>& inFactArguments) const;

    const std::unordered_map<std::string, HTNFactArgumentsTables>& GetFacts() const;

private:
    // Fact ID to fact argument tables
    std::unordered_map<std::string, HTNFactArgumentsTables> mFacts;
};

inline void HTNFactArgumentsTable::RemoveAllFactArguments()
{
    mFactArguments.clear();
}

inline const std::vector<HTNFactArguments>& HTNFactArgumentsTable::GetFactArguments() const
{
    return mFactArguments;
}

inline std::size_t HTNFactArgumentsTable::GetFactArgumentsSize() const
{
    return mFactArguments.size();
}

inline void HTNWorldState::RemoveAllFacts()
{
    mFacts.clear();
}

inline const std::unordered_map<std::string, HTNFactArgumentsTables>& HTNWorldState::GetFacts() const
{
    return mFacts;
}
