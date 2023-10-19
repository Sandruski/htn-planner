#include "Domain/HTNDecompositionWatchPrinterBase.h"

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

HTNDecompositionWatchPrinterBase::HTNDecompositionWatchPrinterBase(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                                   const HTNDecompositionNode&                 inNode)
    : mNode(inNode), mDomainNode(inDomainNode)
{
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  NodeParameters = mNode.GetNodeParameters();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments  = mNode.GetNodeArguments();
    mNodeVariablePaths.reserve(std::max(NodeParameters.size(), NodeArguments.size()));
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantNode& inConstantNode)
{
    // Constant value
    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode  = inConstantNode.GetValueNode();
    const HTNAtom                                          ValueValue = *GetNodeValue(*ValueNode).FindListElement(0);
    return ValueValue;
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode)
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

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNVariableExpressionNode& inVariableExpressionNode)
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

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantExpressionNode& inConstantExpressionNode)
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
