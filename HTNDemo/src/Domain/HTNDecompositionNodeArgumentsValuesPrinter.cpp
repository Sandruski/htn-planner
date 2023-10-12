#include "Domain/HTNDecompositionNodeArgumentsValuesPrinter.h"

#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDomainHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNValueNode.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

HTNDecompositionNodeArgumentsValuesPrinter::HTNDecompositionNodeArgumentsValuesPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                                       const HTNDecompositionNode&                 inNode)
    : mDomainNode(inDomainNode), mNode(inNode)
{
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& NodeArguments = mNode.GetNodeArguments();
    mNodeVariablesPaths.reserve(NodeArguments.size());
}

void HTNDecompositionNodeArgumentsValuesPrinter::Print()
{
    const HTNNodeSnapshotDebug* NodeSnapshot = mNode.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    if (ImGui::BeginTable("NodeArgumentsValues", 1, HTNImGuiHelpers::kDefaultTableFlags))
    {
        const std::vector<std::shared_ptr<const HTNValueNodeBase>>& NodeArguments = mNode.GetNodeArguments();
        for (const std::shared_ptr<const HTNValueNodeBase>& NodeArgument : NodeArguments)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const HTNAtom     Argument                = GetNodeValue(*NodeArgument);
            constexpr bool    ShouldDoubleQuoteString = false;
            const std::string ArgumentString          = Argument.ToString(ShouldDoubleQuoteString);
            ImGui::Text(ArgumentString.c_str());
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

HTNAtom HTNDecompositionNodeArgumentsValuesPrinter::Visit(const HTNConstantNode& inConstantNode)
{
    const std::shared_ptr<const HTNValueNodeBase>& ArgumentNode = inConstantNode.GetArgumentNode();
    return GetNodeValue(*ArgumentNode);
}

HTNAtom HTNDecompositionNodeArgumentsValuesPrinter::Visit(const HTNValueNode& inValueNode)
{
    assert(!inValueNode.IsIdentifier());

    const HTNAtom&     Value                   = inValueNode.GetValue();
    const bool         ShouldDoubleQuoteString = true;
    const std::string& ValueString             = Value.ToString(ShouldDoubleQuoteString);
    return ValueString;
}

HTNAtom HTNDecompositionNodeArgumentsValuesPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    const std::string& VariableID                = inVariableValueNode.GetValue().GetValue<std::string>();
    const std::string& NodeVariableScopeNodePath = mNode.GetNodeVariableScopeNodePath().GetNodePath();
    std::string        VariablePath;
    const bool         Result = HTNDomainHelpers::MakeVariablePath(VariableID, NodeVariableScopeNodePath, VariablePath);
    assert(Result);
    mNodeVariablesPaths.emplace_back(VariablePath);

    const HTNVariables& Variables = mNode.GetNodeSnapshot()->GetVariables();
    return Variables.GetVariable(VariablePath);
}

HTNAtom HTNDecompositionNodeArgumentsValuesPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const std::string&                     ConstantNodeID = inConstantValueNode.GetValue().GetValue<std::string>();
    std::shared_ptr<const HTNConstantNode> ConstantNode   = mDomainNode->FindConstantNodeByID(ConstantNodeID);
    return GetNodeValue(*ConstantNode);
}
