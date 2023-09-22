#include "HTNHelpers.h"

// TODO salvarez Split helpers in two files (parser and interpreter)
#include "HTNAtom.h"
#include "HTNLog.h"
#include "Interpreter/HTNDecompositionContext.h"
#include "Interpreter/HTNEnvironment.h"
#include "Parser/AST/HTNConstantNode.h"
#include "Parser/AST/HTNDomainNode.h"
#include "Parser/AST/HTNValueNode.h"

#include <cassert>

namespace HTN::Helpers
{
bool CopyArgumentsNoConst(const std::shared_ptr<const HTNDomainNode>&             inDomainNode,
                          const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inSourceArgumentNodes,
                          const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inDestinationArgumentNodes, HTNEnvironment& inSourceEnvironment,
                          HTNEnvironment& inDestinationEnvironment, const std::vector<std::string>& inSourcePrefixes,
                          const std::vector<std::string>& inDestinationPrefixes)
{
    /*
    const size_t SourceArgumentNodesSize      = inSourceArgumentNodes.size();
    const size_t DestinationArgumentNodesSize = inDestinationArgumentNodes.size();
    if (SourceArgumentNodesSize != DestinationArgumentNodesSize)
    {
        LOG_ERROR("Trying to copy [{}] arguments to [{}] arguments", SourceArgumentNodesSize, DestinationArgumentNodesSize);
        return false;
    }

    for (size_t i = 0; i < SourceArgumentNodesSize; ++i)
    {
        // TODO salvarez Merge with HTNCondition somehow
        // Source argument can be a variable, a constant, or a literal
        const std::shared_ptr<const HTNValueNode>& SourceArgumentNode  = inSourceArgumentNodes[i];
        const HTNAtom*                             SourceArgumentValue = &SourceArgumentNode->GetValue();
        switch (SourceArgumentNode->GetType())
        {
        case HTNValueType::VARIABLE: {
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
            SourceArgumentValue = &inSourceEnvironment.GetOrAddVariable(SourceArgumentValueString);
            break;
        }
        case HTNValueType::CONSTANT: {
            std::string                            SourceArgumentValueString = SourceArgumentValue->GetValue<std::string>();
            std::shared_ptr<const HTNConstantNode> ConstantNode              = inDomainNode->FindConstantNodeByID(SourceArgumentValueString);
            if (!ConstantNode)
            {
                LOG_ERROR("Constant node [{}] not found", SourceArgumentValueString);
                return false;
            }

            SourceArgumentValue = &ConstantNode->GetValueNode()->GetValue();
        }
        case HTNValueType::LITERAL: {
            break;
        }
        case HTNValueType::SYMBOL:
        case HTNValueType::AXIOM:
        default:
            assert(false);
        }

        // Destination argument must be a variable
        const std::shared_ptr<const HTNValueNode>& DestinationArgumentNode  = inDestinationArgumentNodes[i];
        const HTNAtom&                             DestinationArgumentValue = DestinationArgumentNode->GetValue();
        if (DestinationArgumentNode->GetType() != HTNValueType::VARIABLE)
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
        HTNAtom& DestinationVariable = inDestinationEnvironment.GetOrAddVariable(DestinationArgumentValueString);

        // Copy the source argument to the destination variable
        DestinationVariable = *SourceArgumentValue;
    }
    */

    return true;
}

bool CopyArgumentsConst(const std::shared_ptr<const HTNDomainNode>&             inDomainNode,
                        const std::vector<std::shared_ptr<const HTNValueNode>>& inSourceArgumentNodes,
                        const std::vector<std::shared_ptr<const HTNValueNode>>& inDestinationArgumentNodes, const HTNEnvironment& inSourceEnvironment,
                        HTNEnvironment& inDestinationEnvironment, const std::vector<std::string>& inSourcePrefixes,
                        const std::vector<std::string>& inDestinationPrefixes)
{
    /*
    const size_t SourceArgumentNodesSize      = inSourceArgumentNodes.size();
    const size_t DestinationArgumentNodesSize = inDestinationArgumentNodes.size();
    if (SourceArgumentNodesSize != DestinationArgumentNodesSize)
    {
        LOG_ERROR("Trying to copy [{}] arguments to [{}] arguments", SourceArgumentNodesSize, DestinationArgumentNodesSize);
        return false;
    }

    for (size_t i = 0; i < SourceArgumentNodesSize; ++i)
    {
        // TODO salvarez Merge with HTNCondition somehow
        // Source argument can be a variable, a constant, or a literal
        const std::shared_ptr<const HTNValueNode>& SourceArgumentNode  = inSourceArgumentNodes[i];
        const HTNAtom*                             SourceArgumentValue = &SourceArgumentNode->GetValue();
        switch (SourceArgumentNode->GetType())
        {
        case HTNValueType::VARIABLE: {
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

            // Find variable for source argument in source scope
            SourceArgumentValue = inSourceEnvironment.FindVariable(SourceArgumentValueString);
            break;
        }
        case HTNValueType::CONSTANT: {
            std::string                            SourceArgumentValueString = SourceArgumentValue->GetValue<std::string>();
            std::shared_ptr<const HTNConstantNode> ConstantNode              = inDomainNode->FindConstantNodeByID(SourceArgumentValueString);
            if (!ConstantNode)
            {
                LOG_ERROR("Constant node [{}] not found", SourceArgumentValueString);
                return false;
            }

            SourceArgumentValue = &ConstantNode->GetValueNode()->GetValue();
        }
        case HTNValueType::LITERAL: {
            break;
        }
        case HTNValueType::SYMBOL:
        case HTNValueType::AXIOM:
        default:
            assert(false);
        }

        // Destination argument must be a variable
        const std::shared_ptr<const HTNValueNode>& DestinationArgumentNode  = inDestinationArgumentNodes[i];
        const HTNAtom&                             DestinationArgumentValue = DestinationArgumentNode->GetValue();
        if (DestinationArgumentNode->GetType() != HTNValueType::VARIABLE)
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
        HTNAtom& DestinationVariable = inDestinationEnvironment.GetOrAddVariable(DestinationArgumentValueString);

        // Copy the source argument to the destination variable
        DestinationVariable = *SourceArgumentValue;
    }
    */

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
