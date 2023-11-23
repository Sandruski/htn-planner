// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/HTNDecompositionWatchPrinterBase.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionWatchPrinterContextBase.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Core/HTNScope.h"
#include "Helpers/HTNImGuiHelpers.h"

#include "imgui.h"

namespace
{
enum HTNVariableExpressionNodeResult : uint8
{
    STRING,
    COLOR
};

HTNDecompositionWatchPrinterContextBase& GetDecompositionWatchPrinterContext(HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext)
{
    return static_cast<HTNDecompositionWatchPrinterContextBase&>(ioDecompositionWatchPrinterContext);
}
} // namespace

HTNDecompositionWatchPrinterBase::~HTNDecompositionWatchPrinterBase() = default;

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext) const
{
    // Constant value
    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode = inConstantNode.GetValueNode();
    return GetNodeValue(*ValueNode, ioDecompositionWatchPrinterContext).GetListElement(HTNVariableExpressionNodeResult::STRING);
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNLiteralExpressionNode&         inLiteralExpressionNode,
                                                HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext) const
{
    const HTNAtom& Literal = inLiteralExpressionNode.GetValue();

    // Literal ID
    static constexpr bool   ShouldDoubleQuoteIDString = false;
    const std::string       LiteralIDString           = Literal.ToString(ShouldDoubleQuoteIDString);
    static constexpr ImVec4 LiteralIDColor            = HTNImGuiHelpers::kArgumentColor;

    // Literal value
    static constexpr bool ShouldDoubleQuoteValueString = true;
    const std::string     LiteralValueString           = Literal.ToString(ShouldDoubleQuoteValueString);

    return HTNAtomList({LiteralIDString, HTNAtomList({LiteralIDColor.x, LiteralIDColor.y, LiteralIDColor.z, LiteralIDColor.w}), LiteralValueString});
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext) const
{
    HTNAtomList Result;

    HTNDecompositionWatchPrinterContextBase& DecompositionWatchPrinterContext = GetDecompositionWatchPrinterContext(ioDecompositionWatchPrinterContext);

    const std::string& VariableID = inVariableExpressionNode.GetValue().GetValue<std::string>();

    // Variable ID
    {
        static constexpr bool ShouldDoubleQuoteString = false;
        const std::string     VariableIDString        = std::format("?{}", inVariableExpressionNode.GetValue().ToString(ShouldDoubleQuoteString));
        Result.PushBack(VariableIDString);
        const ImVec4 VariableIDColor = HTNImGuiHelpers::GetVariableColor(VariableID);
        Result.PushBack(HTNAtomList({VariableIDColor.x, VariableIDColor.y, VariableIDColor.z, VariableIDColor.w}));
    }

    // Variable value
    {
        const HTNDecompositionNode& Node                 = DecompositionWatchPrinterContext.GetNode();
        HTNPathHandler              VariablesPathHandler = Node.GetVariablesPathHandler();
        const HTNScope              VariableScope        = HTNScope(VariableID, VariablesPathHandler);
        const std::string&          VariablePath         = VariablesPathHandler.GetPath();
        DecompositionWatchPrinterContext.AddVariablePath(VariablePath);

        const HTNVariables&   Variables               = Node.GetNodeSnapshot()->GetVariables();
        static constexpr bool ShouldDoubleQuoteString = true;
        const std::string     VariableValueString     = Variables.FindVariable(VariablePath).ToString(ShouldDoubleQuoteString);
        Result.PushBack(VariableValueString);
    }

    return Result;
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext) const
{
    HTNAtomList Result;

    HTNDecompositionWatchPrinterContextBase& DecompositionWatchPrinterContext = GetDecompositionWatchPrinterContext(ioDecompositionWatchPrinterContext);

    // Constant ID
    {
        static constexpr bool ShouldDoubleQuoteString = false;
        const std::string     ConstantIDString        = std::format("@{}", inConstantExpressionNode.GetValue().ToString(ShouldDoubleQuoteString));
        Result.PushBack(ConstantIDString);
        static constexpr ImVec4 ConstantIDColor = HTNImGuiHelpers::kArgumentColor;
        Result.PushBack(HTNAtomList({ConstantIDColor.x, ConstantIDColor.y, ConstantIDColor.z, ConstantIDColor.w}));
    }

    // Constant value
    {
        const std::shared_ptr<const HTNDomainNode>& DomainNode              = DecompositionWatchPrinterContext.GetDomainNode();
        const std::string&                          ConstantID              = inConstantExpressionNode.GetValue().GetValue<std::string>();
        std::shared_ptr<const HTNConstantNode>      ConstantNode            = DomainNode->FindConstantNodeByID(ConstantID);
        static constexpr bool                       ShouldDoubleQuoteString = true;
        const std::string                           ConstantValueString = GetNodeValue(*ConstantNode, ioDecompositionWatchPrinterContext).ToString(ShouldDoubleQuoteString);
        Result.PushBack(ConstantValueString);
    }

    return Result;
}
#endif
