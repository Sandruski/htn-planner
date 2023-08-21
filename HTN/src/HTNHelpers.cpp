#include "HTNHelpers.h"

#include "HTNLog.h"
#include "Interpreter/AST/HTNValue.h"
#include "Runtime/HTNAtom.h"
#include "Runtime/HTNDecompositionContext.h"

namespace HTN::Helpers
{
bool CopyArguments(HTNDecompositionRecord& inDecomposition, const std::vector<std::shared_ptr<const HTNValue>>& inSourceArguments,
                   const std::vector<std::shared_ptr<const HTNValue>>& inDestinationArguments,
                   const std::shared_ptr<const HTNNodeBase>& inSourceScope, const std::shared_ptr<const HTNNodeBase>& inDestinationScope,
                   const std::vector<std::string>& inSourcePrefixes, const std::vector<std::string>& inDestinationPrefixes)
{
    const size_t SourceArgumentsSize      = inSourceArguments.size();
    const size_t DestinationArgumentsSize = inDestinationArguments.size();
    if (SourceArgumentsSize != DestinationArgumentsSize)
    {
        LOG_ERROR("Trying to copy [{}] arguments to [{}] arguments", SourceArgumentsSize, DestinationArgumentsSize);
        return false;
    }

    for (size_t i = 0; i < SourceArgumentsSize; ++i)
    {
        // TODO salvarez
        // Source argument can be a variable
        const std::shared_ptr<const HTNValue>& SourceArgument      = inSourceArguments[i];
        const HTNAtom*                         SourceArgumentValue = &SourceArgument->GetValue();
        if (SourceArgument->GetType() == HTNValueType::VARIABLE)
        {
            // Source argument can be prefixed
            std::string SourceArgumentValueString = SourceArgumentValue->GetValue<std::string>();
            bool        IsSourcePrefixRemoved     = inSourcePrefixes.empty();
            for (const std::string& SourcePrefix : inSourcePrefixes)
            {
                IsSourcePrefixRemoved = IsSourcePrefixRemoved || RemovePrefix(SourcePrefix, SourceArgumentValueString);
            }

            if (!IsSourcePrefixRemoved)
            {
                continue;
            }

            // Get or add variable for source argument in source scope
            SourceArgumentValue = &inDecomposition.GetOrAddVariable(inSourceScope, SourceArgumentValueString);
        }

        // Destination argument must be a variable
        const std::shared_ptr<const HTNValue>& DestinationArgument      = inDestinationArguments[i];
        const HTNAtom&                         DestinationArgumentValue = DestinationArgument->GetValue();
        if (DestinationArgument->GetType() != HTNValueType::VARIABLE)
        {
            continue;
        }

        // Destination argument can be prefixed
        std::string DestinationArgumentValueString = DestinationArgumentValue.GetValue<std::string>();
        bool        IsDestinationPrefixRemoved     = inDestinationPrefixes.empty();
        for (const std::string& DestinationPrefix : inDestinationPrefixes)
        {
            IsDestinationPrefixRemoved = IsDestinationPrefixRemoved || RemovePrefix(DestinationPrefix, DestinationArgumentValueString);
        }

        if (!IsDestinationPrefixRemoved)
        {
            continue;
        }

        // Get or add variable for destination argument in destination scope
        HTNAtom& DestinationVariable = inDecomposition.GetOrAddVariable(inDestinationScope, DestinationArgumentValueString);

        // Copy the source argument to the destination variable
        DestinationVariable = *SourceArgumentValue;
    }

    return true;
}

bool RemovePrefix(const std::string& inPrefix, std::string& inString)
{
    const size_t Position = inString.find(inPrefix);
    if (Position == std::string::npos)
    {
        return false;
    }

    inString = inString.erase(Position, inPrefix.size());

    return true;
}
} // namespace HTN::Helpers
