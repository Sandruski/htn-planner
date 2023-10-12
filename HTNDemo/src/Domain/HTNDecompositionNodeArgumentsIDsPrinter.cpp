#include "Domain/HTNDecompositionNodeArgumentsIDsPrinter.h"

#include "Domain/HTNDecompositionNode.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNValueNode.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

HTNDecompositionNodeArgumentsIDsPrinter::HTNDecompositionNodeArgumentsIDsPrinter(const HTNDecompositionNode& inNode) : mNode(inNode)
{
    const std::vector<std::shared_ptr<const HTNValueNodeBase>>& NodeArguments = mNode.GetNodeArguments();
    mNodeVariablesPaths.reserve(NodeArguments.size());
}

void HTNDecompositionNodeArgumentsIDsPrinter::Print()
{
    const HTNNodeSnapshotDebug* NodeSnapshot = mNode.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    if (ImGui::BeginTable("NodeArgumentsIDs", 1, HTNImGuiHelpers::kDefaultTableFlags))
    {
        // Print node arguments
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
            std::string VariableID;
            const bool  Result = HTNDecompositionHelpers::FindVariableID(VariablePath, VariableID);
            assert(Result);
            ImGui::TextDisabled(VariableID.c_str());
        }

        ImGui::EndTable();
    }
}

HTNAtom HTNDecompositionNodeArgumentsIDsPrinter::Visit(const HTNValueNode& inValueNode)
{
    assert(!inValueNode.IsIdentifier());

    const HTNAtom&     Value                   = inValueNode.GetValue();
    const bool         ShouldDoubleQuoteString = false;
    const std::string& ValueString             = Value.ToString(ShouldDoubleQuoteString);
    return ValueString;
}

HTNAtom HTNDecompositionNodeArgumentsIDsPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    const std::string& VariableID                = inVariableValueNode.GetValue().GetValue<std::string>();
    const std::string& NodeVariableScopeNodePath = mNode.GetNodeVariableScopeNodePath().GetNodePath();
    std::string        VariablePath;
    const bool         Result = HTNDecompositionHelpers::MakeVariablePath(VariableID, NodeVariableScopeNodePath, VariablePath);
    assert(Result);
    mNodeVariablesPaths.emplace_back(VariablePath);

    const std::string VariableValueString = inVariableValueNode.ToString();
    return std::format("?{}", VariableValueString);
}

HTNAtom HTNDecompositionNodeArgumentsIDsPrinter::Visit(const HTNConstantValueNode& inConstantValueNode)
{
    const std::string ConstantValueString = inConstantValueNode.ToString();
    return std::format("@{}", ConstantValueString);
}
