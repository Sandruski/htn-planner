// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Core/HTNAtom.h"

#include <string>
#include <unordered_map>

class HTNVariables
{
public:
    // Sets the value of an existing variable or adds a new variable and sets its value
    void SetVariable(const std::string& inVariablePath, const HTNAtom& inVariableValue);

    // Returns an existing variable or an unset HTNAtom if not existing
    HTNAtom FindVariable(const std::string& inVariablePath) const;

    // Removes an existing variable
    void RemoveVariable(const std::string& inVariablePath);

    // Removes all existing variables under the variables path
    void RemoveVariables(const std::string& inVariablesPath);

    const std::unordered_map<std::string, HTNAtom>& GetVariables() const;

private:
    // Variable path to value
    std::unordered_map<std::string, HTNAtom> mVariables;
};

inline void HTNVariables::SetVariable(const std::string& inVariablePath, const HTNAtom& inVariableValue)
{
    mVariables[inVariablePath] = inVariableValue;
}

inline HTNAtom HTNVariables::FindVariable(const std::string& inVariablePath) const
{
    const auto It = mVariables.find(inVariablePath);
    return ((It != mVariables.end()) ? It->second : HTNAtom());
}

inline void HTNVariables::RemoveVariable(const std::string& inVariablePath)
{
    mVariables.erase(inVariablePath);
}

inline const std::unordered_map<std::string, HTNAtom>& HTNVariables::GetVariables() const
{
    return mVariables;
}
