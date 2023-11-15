#include "Domain/HTNDecompositionWatchPrinterBase.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionWatchPrinterContextBase.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#include "Domain/Interpreter/HTNVariables.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Helpers/HTNImGuiHelpers.h"

#include "imgui.h"

namespace
{
HTNDecompositionWatchPrinterContextBase& GetDecompositionWatchPrinterContext(HTNNodeVisitorContextBase& ioContext)
{
    return static_cast<HTNDecompositionWatchPrinterContextBase&>(ioContext);
}
} // namespace

HTNDecompositionWatchPrinterBase::~HTNDecompositionWatchPrinterBase() = default;

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioContext) const
{
    // Constant value
    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode  = inConstantNode.GetValueNode();
    const HTNAtom                                          ValueValue = *GetNodeValue(*ValueNode, ioContext).FindListElement(0);
    return ValueValue;
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNLiteralExpressionNode&         inLiteralExpressionNode,
                                                MAYBE_UNUSED HTNNodeVisitorContextBase& ioContext) const
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

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNAtomList Result;

    HTNDecompositionWatchPrinterContextBase& DecompositionWatchPrinterContext = GetDecompositionWatchPrinterContext(ioContext);

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
        const std::string&          VariableID                = inVariableExpressionNode.GetValue().GetValue<std::string>();
        const HTNDecompositionNode* Node                      = DecompositionWatchPrinterContext.GetNode();
        const std::string&          NodeVariableScopeNodePath = Node->GetNodeVariableScopeNodePath().GetNodePath();
        std::string                 VariablePath;
        const bool MakeVariablePathResult = HTNDecompositionHelpers::MakeVariablePath(VariableID, NodeVariableScopeNodePath, VariablePath);
        assert(MakeVariablePathResult);
        DecompositionWatchPrinterContext.AddVariablePath(VariablePath);

        const HTNVariables& Variables               = Node->GetNodeSnapshot()->GetVariables();
        const HTNAtom       VariableValue           = Variables.FindVariable(VariablePath);
        constexpr bool      ShouldDoubleQuoteString = true;
        const std::string   VariableValueString     = VariableValue.ToString(ShouldDoubleQuoteString);
        Result.Add(VariableValueString);
    }

    return Result;
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioContext) const
{
    HTNAtomList Result;

    HTNDecompositionWatchPrinterContextBase& DecompositionWatchPrinterContext = GetDecompositionWatchPrinterContext(ioContext);

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
        const std::string&                          ConstantID              = inConstantExpressionNode.GetValue().GetValue<std::string>();
        const std::shared_ptr<const HTNDomainNode>& DomainNode              = DecompositionWatchPrinterContext.GetDomainNode();
        std::shared_ptr<const HTNConstantNode>      ConstantNode            = DomainNode->FindConstantNodeByID(ConstantID);
        const HTNAtom                               ConstantValue           = GetNodeValue(*ConstantNode, ioContext);
        constexpr bool                              ShouldDoubleQuoteString = true;
        const std::string                           ConstantValueString     = ConstantValue.ToString(ShouldDoubleQuoteString);
        Result.Add(ConstantValueString);
    }

    return Result;
}
#endif
