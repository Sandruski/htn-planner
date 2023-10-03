#pragma once

#include "HTNAtom.h"
#include "WorldState/HTNWorldStateHelpers.h"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

// Contains the array with a list of arguments that corresponds to certain HtnWorldState key.
class HTNFactArgumentsTable
{
public:
    bool AddUniqueFactArguments(const std::vector<HTNAtom>& inFactArguments);
    void RemoveFactArguments(const std::size_t inFactArgumentsIndex);
    void RemoveAllFactArguments();

    bool Check(const std::size_t inFactArgumentsIndex, std::vector<HTNAtom>& ioFactArguments) const;

    bool ContainsFactArguments(const std::vector<HTNAtom>& inFactArguments) const;

    const std::vector<HTNFactArguments>& GetFactArguments() const;
    std::size_t                          GetFactArgumentsSize() const;

private:
    void AddFactArguments(const std::vector<HTNAtom>& inFactArguments);

    // Fact arguments
    std::vector<HTNFactArguments> mFactArguments;
};

// World state for the HTN planner.
// Knowledge
// Data structure includes a map of <sKey, Array<HTNTable>>. Each key can contain multiple tables, each table has a specific number of arguments.
// There can't be two tables with the same number of arguments for the same key.
class HTNWorldState
{
public:
    void AddFact(const std::string& inFactID, const std::vector<HTNAtom>& inFactArguments);
    void RemoveFact(const std::string& inFactID, const std::size_t inFactArgumentsSize, const std::size_t inFactArgumentsIndex);
    void RemoveAllFacts();

    // It checks if there is an existing entry for the inKey + arguments.
    // If all the arguments are binded then the result is like a binary operation, that query results in true or false.
    // If not all the arguments are binded then the result might return more than 1.
    // Return the number of possible results in the database.
    std::size_t Query(const std::string& inFactID, const std::vector<HTNAtom>& inFactArguments) const;

    // It checks if there is an existing entry for the inKey + arguments.
    // If all the arguments are binded then the result is like a binary operation, that query results in true or false.
    // If not all the arguments are binded then we need to bind it according to the row that correspond to inIndex, the result will always be 0 or 1.
    // Return either 0 or 1 because we are pointing to a specific row within a table.
    bool QueryIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, std::vector<HTNAtom>& ioFactArguments) const;

    bool CheckIndex(const std::string& inFactID, const std::size_t inFactArgumentsIndex, std::vector<HTNAtom>& ioFactArguments) const;

    // Returns the number of HTNTables registered for the fact inKey.
    std::size_t GetFactArgumentsTablesSize(const std::string& inFactID) const;

    // Returns the number of HTNTables registered for the fact inKey and the inNumArgs. This function should only return 0 or 1.
    // If it does returns something different then this will cause problems later on during decomposition.
    bool ContainsFactArgumentTable(const std::string& inFactID, const std::size_t inFactArgumentsSize) const;

    std::size_t GetFactArgumentsSize(const std::string& inFactID, const std::size_t inFactArgumentsSize) const;

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
