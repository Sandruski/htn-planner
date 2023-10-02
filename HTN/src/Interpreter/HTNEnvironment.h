#pragma once

#include "HTNAtom.h"
#include "HTNMacros.h"

#include <string>
#include <unordered_map>

class HTNEnvironment
{
public:
    // Sets the value of an existing variable or adds a new variable and sets its value
    void SetVariable(const std::string& inVariablePath, const HTNAtom& inVariableValue);

    // Returns an existing variable or an unset HTNAtom if not existing
    HTNAtom GetVariable(const std::string& inVariablePath) const;

    // Removes an existing variable
    void RemoveVariable(const std::string& inVariablePath);

    // Removes all existing variables under the scope path
    void RemoveVariables(const std::string& inVariableScopePath);

    void                                            SetVariables(const std::unordered_map<std::string, HTNAtom>& inVariables);
    const std::unordered_map<std::string, HTNAtom>& GetVariables() const;

    // Adds a new index
    unsigned int AddIndex(const std::string& inNodePath);

    // Increments an existing index or adds a new index and increments it
    unsigned int IncrementIndex(const std::string& inNodePath);

    // Adds a new index or increments an existing index
    unsigned int AddOrIncrementIndex(const std::string& inNodePath);

    // Removes an existing index
    void RemoveIndex(const std::string& inNodePath);

    // Returns an existing index or INVALID_INDEX if not existing
    int GetIndex(const std::string& inNodePath) const;

private:
    // Variable path to value
    std::unordered_map<std::string, HTNAtom> mVariables;

    // Node path to index
    // - Method to branch
    // - And, or, and alt conditions to sub-condition
    // - Condition to fact entry
    std::unordered_map<std::string, unsigned int> mIndices;
};

inline void HTNEnvironment::SetVariable(const std::string& inVariablePath, const HTNAtom& inVariableValue)
{
    mVariables[inVariablePath] = inVariableValue;
}

inline HTNAtom HTNEnvironment::GetVariable(const std::string& inVariablePath) const
{
    auto ScopeIt = mVariables.find(inVariablePath);
    if (ScopeIt == mVariables.end())
    {
        return HTNAtom();
    }

    return ScopeIt->second;
}

inline void HTNEnvironment::RemoveVariable(const std::string& inVariablePath)
{
    mVariables.erase(inVariablePath);
}

inline void HTNEnvironment::SetVariables(const std::unordered_map<std::string, HTNAtom>& inVariables)
{
    mVariables = inVariables;
}

inline const std::unordered_map<std::string, HTNAtom>& HTNEnvironment::GetVariables() const
{
    return mVariables;
}

inline unsigned int HTNEnvironment::AddIndex(const std::string& inNodePath)
{
    return mIndices[inNodePath];
}

inline unsigned int HTNEnvironment::IncrementIndex(const std::string& inNodePath)
{
    return ++mIndices[inNodePath];
}

inline unsigned int HTNEnvironment::AddOrIncrementIndex(const std::string& inNodePath)
{
    auto ScopeIt = mIndices.find(inNodePath);
    if (ScopeIt == mIndices.end())
    {
        return AddIndex(inNodePath);
    }

    return IncrementIndex(inNodePath);
}

inline void HTNEnvironment::RemoveIndex(const std::string& inNodePath)
{
    mIndices.erase(inNodePath);
}

inline int HTNEnvironment::GetIndex(const std::string& inNodePath) const
{
    auto ScopeIt = mIndices.find(inNodePath);
    if (ScopeIt == mIndices.end())
    {
        return INVALID_INDEX;
    }

    return ScopeIt->second;
}
