#pragma once

#include "HTNAtom.h"

#include <string>
#include <unordered_map>

class HTNVariables
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

private:
    // Variable path to value
    std::unordered_map<std::string, HTNAtom> mVariables;
};

inline void HTNVariables::SetVariable(const std::string& inVariablePath, const HTNAtom& inVariableValue)
{
    mVariables[inVariablePath] = inVariableValue;
}

inline HTNAtom HTNVariables::GetVariable(const std::string& inVariablePath) const
{
    const auto It = mVariables.find(inVariablePath);
    if (It == mVariables.end())
    {
        return HTNAtom();
    }

    return It->second;
}

inline void HTNVariables::RemoveVariable(const std::string& inVariablePath)
{
    mVariables.erase(inVariablePath);
}