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
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  NodeParameters = mNode.GetNodeParameters();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments  = mNode.GetNodeArguments();
    mNodeVariablesPaths.reserve(std::max(NodeParameters.size(), NodeArguments.size()));
}

void HTNDecompositionNodeParameterArgumentValuesPrinter::Print()
{
    const HTNNodeSnapshotDebug* NodeSnapshot = mNode.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    if (ImGui::BeginTable("NodeParameterArgumentValues", 1, HTNImGuiHelpers::kDefaultTableFlags))
    {
        // Print node parameters
        const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters = mNode.GetNodeParameters();
        for (const std::shared_ptr<const HTNVariableExpressionNode>& NodeParameter : NodeParameters)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const HTNAtom     Parameter               = GetNodeValue(*NodeParameter);
            constexpr bool    ShouldDoubleQuoteString = true;
            const std::string ParameterString         = Parameter.ToString(ShouldDoubleQuoteString);
            constexpr ImVec4  ParameterColor          = ImVec4(1.f, 1.f, 1.f, 1.f);
            ImGui::TextColored(ParameterColor, ParameterString.c_str());
        }

        // Print node arguments
        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments = mNode.GetNodeArguments();
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& NodeArgument : NodeArguments)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const HTNAtom     Argument                = GetNodeValue(*NodeArgument);
            constexpr bool    ShouldDoubleQuoteString = true;
            const std::string ArgumentString          = Argument.ToString(ShouldDoubleQuoteString);
            constexpr ImVec4  ArgumentColor           = ImVec4(1.f, 1.f, 1.f, 1.f);
            ImGui::TextColored(ArgumentColor, ArgumentString.c_str());
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
            constexpr bool    ShouldDoubleQuoteString = true;
            const std::string VariableString          = Variable.ToString(ShouldDoubleQuoteString);
            const ImVec4      VariableColor           = ImVec4(1.f, 1.f, 1.f, 1.f);
            ImGui::TextColored(VariableColor, VariableString.c_str());
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
    const bool         ShouldDoubleQuoteString = false;
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
