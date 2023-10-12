#include "Domain/HTNDecompositionVariableConstantIDsPrinter.h"

#include "Domain/HTNDomainHelpers.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNValueNode.h"
#include "HTNImGuiHelpers.h"
#include "HTNLog.h"

#include "imgui.h"

HTNDecompositionVariableConstantIDsPrinter::HTNDecompositionVariableConstantIDsPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                   const HTNDecompositionNode&                 inNode)
    : mDomainNode(inDomainNode), mNode(inNode)
{
}

void HTNDecompositionVariableConstantIDsPrinter::Print()
{
        /*
        for (const std::pair<std::string, HTNAtom>& VariablePair : inVariables)
        {
            const std::string& VariablePath = VariablePair.first;
            std::string        VariableID;
            const bool         FindVariableIDResult = HTNDecompositionContextHelpers::FindVariableID(VariablePath, VariableID);
            assert(FindVariableIDResult);
            const bool IsArgumentResult = IsArgument(VariableID, inArguments);
            if (IsArgumentResult)
            {
                ImGui::Text(VariableID.c_str());
            }
            else
            {
                ImGui::TextDisabled(VariableID.c_str());
            }

            const HTNAtom&    VariableValue           = VariablePair.second;
            constexpr bool    ShouldDoubleQuoteString = true;
            const std::string VariableValueString     = VariableValue.ToString(ShouldDoubleQuoteString);
            if (IsArgumentResult)
            {
                ImGui::Text(VariableValueString.c_str());
            }
            else
            {
                ImGui::TextDisabled(VariableValueString.c_str());
            }
        }
        */

        // TODO salvarez Print ID (with @ or ?, but no literals) + value
        // TODO salvarez Rename this class to ArgumentsPrinter or something like that
        /*
        for (const std::shared_ptr<const HTNValueNodeBase>& Argument : inArguments)
        {
            GetNodeValue(*Argument);
        }*/
}

HTNAtom HTNDecompositionVariableConstantIDsPrinter::Visit(const HTNConstantNode& inConstantNode)
{
    const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = inConstantNode.GetArgumentNode();
    return GetNodeValue(*ArgumentNode);
}

HTNAtom HTNDecompositionVariableConstantIDsPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    const std::string  VariableID                = inVariableValueNode.GetValue().GetValue<std::string>();
    const std::string& NodeVariableScopeNodePath = mNode.GetNodeVariableScopeNodePath().GetNodePath();
    std::string        VariablePath;
    if (!HTNDomainHelpers::MakeVariablePath(VariableID, NodeVariableScopeNodePath, VariablePath))
    {
        LOG_ERROR("Path for variable [{}] could not be made", VariableID);
        return HTNAtom();
    }

    // TODO salvarez Find variable in Variables (could be its own class with its own methods instead of environment)
    return HTNAtom();
}

HTNAtom HTNDecompositionVariableConstantIDsPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const std::string&                     ConstantNodeID = inConstantValueNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode> ConstantNode   = mDomainNode->FindConstantNodeByID(ConstantNodeID);
    if (!ConstantNode)
    {
        LOG_ERROR("Constant node [{}] not found", ConstantNodeID);
        return HTNAtom();
    }

    return GetNodeValue(*ConstantNode);
}

bool HTNDecompositionVariableConstantIDsPrinter::IsArgument(const std::string&                                          inVariableID,
                                                  const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArguments)
{
    for (const std::shared_ptr<const HTNValueNodeBase>& Argument : inArguments)
    {
        const HTNAtom ArgumentNodeValue = GetNodeValue(*Argument);
        if (!ArgumentNodeValue.IsSet())
        {
            continue;
        }

        const std::string VariableID = ArgumentNodeValue.GetValue<std::string>();
        if (inVariableID == VariableID)
        {
            return true;
        }
    }

    return false;
}
