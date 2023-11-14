#include "Domain/HTNDecompositionWatchPrinterBase.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Helpers/HTNImGuiHelpers.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"

#include "imgui.h"

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioContext)
{
    // Constant value
    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode  = inConstantNode.GetValueNode();
    const HTNAtom                                          ValueValue = *GetNodeValue(*ValueNode, ioContext).FindListElement(0);
    return ValueValue;
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioContext)
{
    const HTNAtom& Literal = inLiteralExpressionNode.GetValue();

    // Literal ID
    const bool         ShouldDoubleQuoteIDString = false;
    const std::string& LiteralIDString           = Literal.ToString(ShouldDoubleQuoteIDString);
    constexpr ImVec4   LiteralIDColor            = HTNImGuiHelpers::kArgumentColor;

    // Literal value
    const bool         ShouldDoubleQuoteValueString = true;
    const std::string& LiteralValueString           = Literal.ToString(ShouldDoubleQuoteValueString);

    return HTNAtomList({LiteralIDString, HTNAtomList({LiteralIDColor.x, LiteralIDColor.y, LiteralIDColor.z, LiteralIDColor.w}), LiteralValueString});
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioContext)
{
    HTNAtomList Result;

    // Variable ID
    {
        const HTNAtom&    VariableID              = inVariableExpressionNode.GetValue();
        constexpr bool    ShouldDoubleQuoteString = false;
        const std::string VariableIDString        = std::format("?{}", VariableID.ToString(ShouldDoubleQuoteString));
        Result.Add(VariableIDString);
        const ImVec4 VariableIDColor = HTNImGuiHelpers::GetVariableColor(VariableIDString);
        Result.Add(HTNAtomList({VariableIDColor.x, VariableIDColor.y, VariableIDColor.z, VariableIDColor.w}));
    }

    // Variable value
    {
        const std::string& VariableID                = inVariableExpressionNode.GetValue().GetValue<std::string>();
        const std::string& NodeVariableScopeNodePath = mNode->GetNodeVariableScopeNodePath().GetNodePath();
        std::string        VariablePath;
        const bool         MakeVariablePathResult = HTNDecompositionHelpers::MakeVariablePath(VariableID, NodeVariableScopeNodePath, VariablePath);
        assert(MakeVariablePathResult);
        mNodeVariablePaths.emplace_back(VariablePath);

        const HTNVariables& Variables               = mNode->GetNodeSnapshot()->GetVariables();
        const HTNAtom       VariableValue           = Variables.FindVariable(VariablePath);
        constexpr bool      ShouldDoubleQuoteString = true;
        const std::string   VariableValueString     = VariableValue.ToString(ShouldDoubleQuoteString);
        Result.Add(VariableValueString);
    }

    return Result;
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioContext)
{
    HTNAtomList Result;

    // Constant ID
    {
        const HTNAtom&    ConstantID              = inConstantExpressionNode.GetValue();
        constexpr bool    ShouldDoubleQuoteString = false;
        const std::string ConstantIDString        = std::format("@{}", ConstantID.ToString(ShouldDoubleQuoteString));
        Result.Add(ConstantIDString);
        constexpr ImVec4 ConstantIDColor = HTNImGuiHelpers::kArgumentColor;
        Result.Add(HTNAtomList({ConstantIDColor.x, ConstantIDColor.y, ConstantIDColor.z, ConstantIDColor.w}));
    }

    // Constant value
    {
        const std::string&                     ConstantID              = inConstantExpressionNode.GetValue().GetValue<std::string>();
        std::shared_ptr<const HTNConstantNode> ConstantNode            = mDomainNode->FindConstantNodeByID(ConstantID);
        const HTNAtom                          ConstantValue           = GetNodeValue(*ConstantNode, ioContext);
        constexpr bool                         ShouldDoubleQuoteString = true;
        const std::string                      ConstantValueString     = ConstantValue.ToString(ShouldDoubleQuoteString);
        Result.Add(ConstantValueString);
    }

    return Result;
}

void HTNDecompositionWatchPrinterBase::Reset(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNDecompositionNode& inNode)
{
    mNode                                                                                = &inNode;
    const std::vector<std::shared_ptr<const HTNVariableExpressionNode>>&  NodeParameters = mNode->GetNodeParameters();
    const std::vector<std::shared_ptr<const HTNValueExpressionNodeBase>>& NodeArguments  = mNode->GetNodeArguments();
    mNodeVariablePaths.clear();
    mNodeVariablePaths.reserve(std::max(NodeParameters.size(), NodeArguments.size()));

    mDomainNode = inDomainNode;
}
#endif
