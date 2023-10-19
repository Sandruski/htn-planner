#include "Domain/HTNDecompositionWatchWindowPrinter.h"

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

HTNDecompositionWatchWindowPrinter::HTNDecompositionWatchWindowPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                       const HTNDecompositionNode&                 inNode)
    : mDomainNode(inDomainNode), mNode(inNode)
{
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  NodeParameters = mNode.GetNodeParameters();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments  = mNode.GetNodeArguments();
    mNodeVariablePaths.reserve(std::max(NodeParameters.size(), NodeArguments.size()));
}

void HTNDecompositionWatchWindowPrinter::Print()
{
    const HTNNodeSnapshotDebug* NodeSnapshot = mNode.GetNodeSnapshot();
    if (!NodeSnapshot)
    {
        return;
    }

    if (ImGui::BeginTable("WatchWindowTable", 2, HTNImGuiHelpers::kDefaultTableFlags))
    {
        // Print node parameters
        const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>& NodeParameters = mNode.GetNodeParameters();
        for (const std::shared_ptr<const HTNVariableExpressionNode>& NodeParameter : NodeParameters)
        {
            ImGui::TableNextRow();

            const HTNAtom  Parameter               = GetNodeValue(*NodeParameter);
            constexpr bool ShouldDoubleQuoteString = false;

            // Parameter ID
            ImGui::TableNextColumn();

            const HTNAtom&    ParameterID       = *Parameter.FindListElement(0);
            const std::string ParameterIDString = ParameterID.ToString(ShouldDoubleQuoteString);
            constexpr ImVec4  ParameterIDColor  = HTNImGuiHelpers::kParameterColor;
            ImGui::TextColored(ParameterIDColor, ParameterIDString.c_str());

            // Parameter value
            ImGui::TableNextColumn();

            const HTNAtom&    ParameterValue       = *Parameter.FindListElement(1);
            const std::string ParameterValueString = ParameterValue.ToString(ShouldDoubleQuoteString);
            ImGui::Text(ParameterValueString.c_str());
        }

        // Print node arguments
        const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments = mNode.GetNodeArguments();
        for (const std::shared_ptr<const HTNValueExpressionNodeBase>& NodeArgument : NodeArguments)
        {
            ImGui::TableNextRow();

            const HTNAtom  Argument                = GetNodeValue(*NodeArgument);
            constexpr bool ShouldDoubleQuoteString = false;

            // Argument ID
            ImGui::TableNextColumn();

            const HTNAtom&    ArgumentID       = *Argument.FindListElement(0);
            const std::string ArgumentIDString = ArgumentID.ToString(ShouldDoubleQuoteString);
            const ImVec4      ArgumentIDColor =
                HTNDecompositionHelpers::IsParameter(ArgumentIDString) ? HTNImGuiHelpers::kParameterColor : HTNImGuiHelpers::kArgumentColor;
            ImGui::TextColored(ArgumentIDColor, ArgumentIDString.c_str());

            // Argument value
            ImGui::TableNextColumn();

            const HTNAtom&    ArgumentValue       = *Argument.FindListElement(1);
            const std::string ArgumentValueString = ArgumentValue.ToString(ShouldDoubleQuoteString);
            ImGui::Text(ArgumentValueString.c_str());
        }

        // Print remaining variables
        const std::unordered_map<std::string, HTNAtom>& Variables = NodeSnapshot->GetVariables().GetVariables();
        for (const std::pair<std::string, HTNAtom>& VariablePair : Variables)
        {
            const std::string& VariablePath = VariablePair.first;
            const auto         It           = std::find(mNodeVariablePaths.begin(), mNodeVariablePaths.end(), VariablePath);
            if (It != mNodeVariablePaths.end())
            {
                continue;
            }

            ImGui::TableNextRow();

            // Variable ID
            ImGui::TableNextColumn();

            std::string VariableID;
            const bool  FindVariableIDResult = HTNDecompositionHelpers::FindVariableID(VariablePath, VariableID);
            assert(FindVariableIDResult);
            const std::string VariableIDString = std::format("?{}", VariableID);
            const ImVec4      VariableIDColor =
                HTNDecompositionHelpers::IsParameter(VariableID) ? HTNImGuiHelpers::kParameterColor : HTNImGuiHelpers::kArgumentColor;
            ImGui::TextColored(VariableIDColor, VariableIDString.c_str());

            // Variable value
            ImGui::TableNextColumn();

            const HTNAtom&    Variable                = VariablePair.second;
            constexpr bool    ShouldDoubleQuoteString = true;
            const std::string VariableString          = Variable.ToString(ShouldDoubleQuoteString);
            ImGui::Text(VariableString.c_str());
        }

        ImGui::EndTable();
    }
}

HTNAtom HTNDecompositionWatchWindowPrinter::Visit(const HTNConstantNode& inConstantNode)
{
    // Constant value
    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode  = inConstantNode.GetValueNode();
    const HTNAtom                                          ValueValue = *GetNodeValue(*ValueNode).FindListElement(0);
    return ValueValue;
}

HTNAtom HTNDecompositionWatchWindowPrinter::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode)
{
    const HTNAtom& Value = inLiteralExpressionNode.GetValue();

    // Literal ID
    const bool         ShouldDoubleQuoteIDString = false;
    const std::string& ValueIDString             = Value.ToString(ShouldDoubleQuoteIDString);

    // Literal value
    const bool         ShouldDoubleQuoteValueString = true;
    const std::string& ValueValueString             = Value.ToString(ShouldDoubleQuoteValueString);

    return HTNAtomList({ValueIDString, ValueValueString});
}

HTNAtom HTNDecompositionWatchWindowPrinter::Visit(const HTNVariableExpressionNode& inVariableExpressionNode)
{
    HTNAtomList Result;

    // Variable ID
    {
        const HTNAtom&    VariableID              = inVariableExpressionNode.GetValue();
        constexpr bool    ShouldDoubleQuoteString = false;
        const std::string VariableIDString        = std::format("?{}", VariableID.ToString(ShouldDoubleQuoteString));
        Result.Add(VariableIDString);
    }

    // Variable value
    {
        const std::string& VariableID                = inVariableExpressionNode.GetValue().GetValue<std::string>();
        const std::string& NodeVariableScopeNodePath = mNode.GetNodeVariableScopeNodePath().GetNodePath();
        std::string        VariablePath;
        const bool         MakeVariablePathResult = HTNDecompositionHelpers::MakeVariablePath(VariableID, NodeVariableScopeNodePath, VariablePath);
        assert(MakeVariablePathResult);
        mNodeVariablePaths.emplace_back(VariablePath);

        const HTNVariables& Variables               = mNode.GetNodeSnapshot()->GetVariables();
        const HTNAtom       VariableValue           = Variables.FindVariable(VariablePath);
        constexpr bool      ShouldDoubleQuoteString = true;
        const std::string   VariableValueString     = VariableValue.ToString(ShouldDoubleQuoteString);
        Result.Add(VariableValueString);
    }

    return Result;
}

HTNAtom HTNDecompositionWatchWindowPrinter::Visit(const HTNConstantExpressionNode& inConstantExpressionNode)
{
    HTNAtomList Result;

    // Constant ID
    {
        const HTNAtom&    ConstantID              = inConstantExpressionNode.GetValue();
        constexpr bool    ShouldDoubleQuoteString = false;
        const std::string ConstantIDString        = std::format("@{}", ConstantID.ToString(ShouldDoubleQuoteString));
        Result.Add(ConstantIDString);
    }

    // Constant value
    {
        const std::string&                     ConstantID              = inConstantExpressionNode.GetValue().GetValue<std::string>();
        std::shared_ptr<const HTNConstantNode> ConstantNode            = mDomainNode->FindConstantNodeByID(ConstantID);
        const HTNAtom                          ConstantValue           = GetNodeValue(*ConstantNode);
        constexpr bool                         ShouldDoubleQuoteString = true;
        const std::string                      ConstantValueString     = ConstantValue.ToString(ShouldDoubleQuoteString);
        Result.Add(ConstantValueString);
    }

    return Result;
}
#endif
