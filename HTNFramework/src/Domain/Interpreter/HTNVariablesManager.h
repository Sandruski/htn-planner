// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNAtom.h"
#include "HTNCoreMinimal.h"

#include <string>
#include <unordered_map>

// Variable path to variable value
using HTNVariables = std::unordered_map<std::string, HTNAtom>;
using HTNVariable  = std::pair<const std::string, HTNAtom>;

/**
 * Manager of the variables of a decomposition
 */
class HTNVariablesManager
{
public:
    // Sets the value of an existing variable or adds a new variable and sets its value
    void SetVariable(const std::string& inVariablePath, const HTNAtom& inVariableValue);

    // Returns an existing variable or an unbound atom if the variable does not exist
    HTN_NODISCARD HTNAtom FindVariable(const std::string& inVariablePath) const;

    // Removes an existing variable
    void RemoveVariable(const std::string& inVariablePath);

    // Removes all existing variables under the variables path
    void RemoveVariables(const std::string& inVariablesPath);

    // Returns the variables
    HTN_NODISCARD const HTNVariables& GetVariables() const;

private:
    HTNVariables mVariables;
};

inline void HTNVariablesManager::SetVariable(const std::string& inVariablePath, const HTNAtom& inVariableValue)
{
    mVariables[inVariablePath] = inVariableValue;
}

inline HTNAtom HTNVariablesManager::FindVariable(const std::string& inVariablePath) const
{
    const auto It = mVariables.find(inVariablePath);
    return (It != mVariables.end()) ? It->second : HTNAtom();
}

inline void HTNVariablesManager::RemoveVariable(const std::string& inVariablePath)
{
    mVariables.erase(inVariablePath);
}

inline const HTNVariables& HTNVariablesManager::GetVariables() const
{
    return mVariables;
}
