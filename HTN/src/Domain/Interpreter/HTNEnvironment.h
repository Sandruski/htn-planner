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
    const std::unordered_map<std::string, HTNAtom>& GetVariables() const; // TODO salvarez Using

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

private:
    // Variable path to value
    std::unordered_map<std::string, HTNAtom> mVariables;

    // Node path to index
    // - Method to branch
    // - And, or, and alt conditions to sub-condition
    // - Condition to fact entry
    std::unordered_map<std::string, std::size_t> mIndices;
};

inline void HTNEnvironment::SetVariable(const std::string& inVariablePath, const HTNAtom& inVariableValue)
{
    mVariables[inVariablePath] = inVariableValue;
}

inline HTNAtom HTNEnvironment::GetVariable(const std::string& inVariablePath) const
{
    const auto It = mVariables.find(inVariablePath);
    if (It == mVariables.end())
    {
        return HTNAtom();
    }

    return It->second;
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

inline std::size_t HTNEnvironment::AddIndex(const std::string& inNodePath)
{
    return mIndices[inNodePath];
}

inline std::size_t HTNEnvironment::IncrementIndex(const std::string& inNodePath)
{
    return ++mIndices[inNodePath];
}

inline std::size_t HTNEnvironment::AddOrIncrementIndex(const std::string& inNodePath)
{
    const auto It = mIndices.find(inNodePath);
    if (It == mIndices.end())
    {
        return AddIndex(inNodePath);
    }

    return IncrementIndex(inNodePath);
}

inline void HTNEnvironment::RemoveIndex(const std::string& inNodePath)
{
    mIndices.erase(inNodePath);
}

inline std::size_t HTNEnvironment::GetIndex(const std::string& inNodePath) const
{
    const auto It = mIndices.find(inNodePath);
    if (It == mIndices.end())
    {
        return std::numeric_limits<std::size_t>::max();
    }

    return It->second;
}