#pragma once

#include "HTNAtom.h"

#include <string>
#include <unordered_map>

class HTNEnvironment
{
public:
    HTNAtom& GetOrAddVariable(const std::string& inID); // TODO salvarez Remove this

    void           SetOrAddVariable(const std::string& inID, const HTNAtom& inValue);
    const HTNAtom* FindVariable(const std::string& inID) const;

    void SetVariables(const std::unordered_map<std::string, HTNAtom>& inVariables);
    const std::unordered_map<std::string, HTNAtom>& GetVariables() const;

    // Returns an existing index or adds a new index
    unsigned int GetOrAddIndex(const std::string& inID);

    // Increments an existing index or adds a new index and increments it
    unsigned int IncrementIndex(const std::string& inID);

    // Adds a new index or increments an existing index
    unsigned int AddOrIncrementIndex(const std::string& inID);

private:
    // Variable ID to value
    std::unordered_map<std::string, HTNAtom> mVariables;

    // Node ID to index
    // - Method scope ID to branch index
    // - HTNConditionAnd, HTNConditionOr, and HTNConditionAlt scope ID to sub-condition index
    // - HTNCondition scope ID to fact entry index
    std::unordered_map<std::string, unsigned int> mIndices;
};

inline HTNAtom& HTNEnvironment::GetOrAddVariable(const std::string& inID)
{
    return mVariables[inID];
}

inline void HTNEnvironment::SetOrAddVariable(const std::string& inID, const HTNAtom& inValue)
{
    mVariables[inID] = inValue;
}

inline const HTNAtom* HTNEnvironment::FindVariable(const std::string& inID) const
{
    const auto VariableIt = mVariables.find(inID);
    if (VariableIt == mVariables.end())
    {
        return nullptr;
    }

    return &VariableIt->second;
}

inline void HTNEnvironment::SetVariables(const std::unordered_map<std::string, HTNAtom>& inVariables)
{
    mVariables = inVariables;
}

inline const std::unordered_map<std::string, HTNAtom>& HTNEnvironment::GetVariables() const
{
    return mVariables;
}

inline unsigned int HTNEnvironment::GetOrAddIndex(const std::string& inID)
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

    return ++ScopeIt->second;
}
