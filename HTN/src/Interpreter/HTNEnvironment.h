#pragma once

#include "HTNAtom.h"
#include "HTNMacros.h"

#include <string>
#include <unordered_map>

class HTNEnvironment
{
public:
    void           SetVariable(const std::string& inVariableID, const HTNAtom& inVariableValue);
    const HTNAtom& GetOrAddVariable(const std::string& inVariableID);

    void                                            SetVariables(const std::unordered_map<std::string, HTNAtom>& inVariables);
    const std::unordered_map<std::string, HTNAtom>& GetVariables() const;

    // Adds a new index if not existing
    unsigned int AddIndex(const std::string& inID);

    // Increments an existing index or adds a new index and increments it
    unsigned int IncrementIndex(const std::string& inID);

    // Adds a new index or increments an existing index
    unsigned int AddOrIncrementIndex(const std::string& inID);

    // Returns an existing index or INVALID_INDEX if not existing
    int GetIndex(const std::string& inID) const;

private:
    // Variable ID to value
    std::unordered_map<std::string, HTNAtom> mVariables;

    // Node ID to index
    // - Method scope ID to branch index
    // - HTNConditionAnd, HTNConditionOr, and HTNConditionAlt scope ID to sub-condition index
    // - HTNCondition scope ID to fact entry index
    std::unordered_map<std::string, unsigned int> mIndices;
};

inline void HTNEnvironment::SetVariable(const std::string& inVariableID, const HTNAtom& inVariableValue)
{
    mVariables[inVariableID] = inVariableValue;
}

inline const HTNAtom& HTNEnvironment::GetOrAddVariable(const std::string& inVariableID)
{
    return mVariables[inVariableID];
}

inline void HTNEnvironment::SetVariables(const std::unordered_map<std::string, HTNAtom>& inVariables)
{
    mVariables = inVariables;
}

inline const std::unordered_map<std::string, HTNAtom>& HTNEnvironment::GetVariables() const
{
    return mVariables;
}

inline unsigned int HTNEnvironment::AddIndex(const std::string& inID)
{
    return mIndices[inID];
}

inline unsigned int HTNEnvironment::IncrementIndex(const std::string& inID)
{
    return ++mIndices[inID];
}

inline unsigned int HTNEnvironment::AddOrIncrementIndex(const std::string& inID)
{
    auto ScopeIt = mIndices.find(inID);
    if (ScopeIt == mIndices.end())
    {
        return mIndices[inID];
    }

    return ++(ScopeIt->second);
}

inline int HTNEnvironment::GetIndex(const std::string& inID) const
{
    auto ScopeIt = mIndices.find(inID);
    if (ScopeIt == mIndices.end())
    {
        return INVALID_INDEX;
    }

    return ScopeIt->second;
}
