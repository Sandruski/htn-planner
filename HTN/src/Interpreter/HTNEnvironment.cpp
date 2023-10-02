#include "Interpreter/HTNEnvironment.h"

void HTNEnvironment::RemoveVariables(const std::string& inVariableScopePath)
{
    for (auto It = mVariables.begin(); It != mVariables.end();)
    {
        const std::string& VariablePath = It->first;

        // Check if variable path is prefixed by variable scope path
        const size_t Index = VariablePath.find(inVariableScopePath);
        if (Index != 0)
        {
            ++It;
            continue;
        }

        It = mVariables.erase(It);
    }
}
