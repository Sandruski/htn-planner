#include "Domain/HTNDecompositionNodeParameterArgumentValuesPrinter.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

HTNDecompositionNodeParameterArgumentValuesPrinter::HTNDecompositionNodeParameterArgumentValuesPrinter(
    const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode)
    : mDomainNode(inDomainNode), mNode(inNode)
{
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeParametersArguments = mNode.GetNodeParametersArguments();
    mNodeVariablesPaths.reserve(NodeParametersArguments.size());
}

void HTNDecompositionNodeParameterArgumentValuesPrinter::Print()
{
    const HTNNodeSnapshotDebug* NodeSnapshot = mNode.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    if (ImGui::BeginTable("NodeArgumentsValues", 1, HTNImGuiHelpers::kDefaultTableFlags))
    {
        // Print node parameters or arguments
        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeParametersArguments = mNode.GetNodeParametersArguments();
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& NodeParametersArgument : NodeParametersArguments)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const HTNAtom     Value                   = GetNodeValue(*NodeParametersArgument);
            constexpr bool    ShouldDoubleQuoteString = false;
            const std::string ValueString             = Value.ToString(ShouldDoubleQuoteString);
            ImGui::Text(ValueString.c_str());
        }

        // Print remaining variables
        const std::unordered_map<std::string, HTNAtom>& Variables = NodeSnapshot->GetVariables().GetVariables();
        for (const std::pair<std::string, HTNAtom>& VariablePair : Variables)
        {
            const std::string& VariablePath = VariablePair.first;
            const auto         It           = std::find(mNodeVariablesPaths.begin(), mNodeVariablesPaths.end(), VariablePath);
            if (It != mNodeVariablesPaths.end())
            {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const HTNAtom&    Variable                = VariablePair.second;
            constexpr bool    ShouldDoubleQuoteString = false;
            const std::string VariableString          = Variable.ToString(ShouldDoubleQuoteString);
            ImGui::TextDisabled(VariableString.c_str());
        }

        ImGui::EndTable();
    }
}

HTNAtom HTNDecompositionNodeParameterArgumentValuesPrinter::Visit(const HTNConstantNode& inConstantNode)
{
    const std::shared_ptr<const HTNValueExpressionNodeBase>& ValueNode = inConstantNode.GetValueNode();
    return GetNodeValue(*ValueNode);
}

HTNAtom HTNDecompositionNodeParameterArgumentValuesPrinter::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode)
{
    const HTNAtom&     Value                   = inLiteralExpressionNode.GetValue();
    const bool         ShouldDoubleQuoteString = true;
    const std::string& ValueString             = Value.ToString(ShouldDoubleQuoteString);
    return ValueString;
}

HTNAtom HTNDecompositionNodeParameterArgumentValuesPrinter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode)
{
    const std::string& VariableID                = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const std::string& NodeVariableScopeNodePath = mNode.GetNodeVariableScopeNodePath().GetNodePath();
    std::string        VariablePath;
    const bool         Result = HTNDecompositionHelpers::MakeVariablePath(VariableID, NodeVariableScopeNodePath, VariablePath);
    assert(Result);
    mNodeVariablesPaths.emplace_back(VariablePath);

    const HTNVariables& Variables = mNode.GetNodeSnapshot()->GetVariables();
    return Variables.GetVariable(VariablePath);
}

HTNAtom HTNDecompositionNodeParameterArgumentValuesPrinter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode)
{
    const std::string&                     ConstantNodeID = inConstantExpressionNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode> ConstantNode   = mDomainNode->FindConstantNodeByID(ConstantNodeID);
    return GetNodeValue(*ConstantNode);
}
#endif
