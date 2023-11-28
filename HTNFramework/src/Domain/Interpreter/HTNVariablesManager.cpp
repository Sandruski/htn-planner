// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Interpreter/HTNVariablesManager.h"

void HTNVariablesManager::RemoveVariables(const std::string& inVariablesPath)
{
    for (auto It = mVariables.cbegin(); It != mVariables.cend();)
    {
        const std::string& VariablePath = It->first;

        // Check if variable path is prefixed by variables path
        const size Index = VariablePath.find(inVariablesPath);
        if (Index != 0)
        {
            ++It;
            continue;
        }

        It = mVariables.erase(It);
    }
}
