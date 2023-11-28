// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNDecompositionWatchPrinterBase.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Core/HTNScope.h"
#include "Domain/Interpreter/HTNNodeSnapshot.h"
#include "Domain/Interpreter/HTNVariablesManager.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "Domain/Printer/HTNDecompositionWatchPrinterContextBase.h"
#include "Domain/Printer/HTNNodeInstance.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

namespace
{
enum HTNValueExpressionNodeResult : uint8
{
    ID_STRING,
    ID_COLOR,
    VALUE_STRING
};

enum HTNValueExpressionNodeColorResult : uint8
{
    X,
    Y,
    Z,
    W
};

HTN_NODISCARD HTNDecompositionWatchPrinterContextBase& GetDecompositionWatchPrinterContext(
    HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext)
{
    return static_cast<HTNDecompositionWatchPrinterContextBase&>(ioDecompositionWatchPrinterContext);
}
} // namespace

HTNDecompositionWatchPrinterBase::~HTNDecompositionWatchPrinterBase() = default;

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantNode&     inConstantNode,
                                                HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext) const
{
    // Constant value
    const std::shared_ptr<const HTNLiteralExpressionNode>& ValueNode = inConstantNode.GetValueNode();
    return GetNodeValue(*ValueNode, ioDecompositionWatchPrinterContext).GetListElement(HTNValueExpressionNodeResult::ID_STRING);
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNLiteralExpressionNode&             inLiteralExpressionNode,
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

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNVariableExpressionNode& inVariableExpressionNode,
                                                HTNNodeVisitorContextBase&       ioDecompositionWatchPrinterContext) const
{
    HTNAtomList Result;

    HTNDecompositionWatchPrinterContextBase& DecompositionWatchPrinterContext =
        GetDecompositionWatchPrinterContext(ioDecompositionWatchPrinterContext);

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
        const HTNNodeInstance& NodeInstance         = DecompositionWatchPrinterContext.GetNodeInstance();
        HTNPathManager         VariablesPathManager = NodeInstance.GetVariablesPathManager();
        const HTNScope         VariableScope        = HTNScope(VariableID, VariablesPathManager);
        const std::string&     VariablePath         = VariablesPathManager.GetPath();
        DecompositionWatchPrinterContext.AddVariablePath(VariablePath);

        const HTNNodeSnapshot*     NodeSnapshot            = NodeInstance.GetNodeSnapshot();
        const HTNVariablesManager& VariablesManager        = NodeSnapshot->GetVariablesManager();
        static constexpr bool      ShouldDoubleQuoteString = true;
        const std::string          VariableValueString     = VariablesManager.FindVariable(VariablePath).ToString(ShouldDoubleQuoteString);
        Result.PushBack(VariableValueString);
    }

    return Result;
}

HTNAtom HTNDecompositionWatchPrinterBase::Visit(const HTNConstantExpressionNode& inConstantExpressionNode,
                                                HTNNodeVisitorContextBase&       ioDecompositionWatchPrinterContext) const
{
    HTNAtomList Result;

    HTNDecompositionWatchPrinterContextBase& DecompositionWatchPrinterContext =
        GetDecompositionWatchPrinterContext(ioDecompositionWatchPrinterContext);

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
        const std::shared_ptr<const HTNDomainNode>& DomainNode     = DecompositionWatchPrinterContext.GetDomainNode();
        const std::string&                          ConstantNodeID = inConstantExpressionNode.GetValue().GetValue<std::string>();
        std::shared_ptr<const HTNConstantNode>      ConstantNode   = DomainNode->FindConstantNodeByID(ConstantNodeID);
        if (!ConstantNode)
        {
            HTN_LOG_ERROR("Constant node [{}] could not be found", ConstantNodeID);
            const std::string ConstantValueString = "";
            Result.PushBack(ConstantValueString);
            return Result;
        }

        static constexpr bool ShouldDoubleQuoteString = true;
        const std::string     ConstantValueString = GetNodeValue(*ConstantNode, ioDecompositionWatchPrinterContext).ToString(ShouldDoubleQuoteString);
        Result.PushBack(ConstantValueString);
    }

    return Result;
}

void HTNDecompositionWatchPrinterBase::PrintColoredValueIDExpressionNode(
    const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    const HTNAtom      Value           = GetNodeValue(*inValueExpressionNode, ioDomainPrinterContext);
    const std::string& ValueString     = Value.GetListElement(HTNValueExpressionNodeResult::ID_STRING).GetValue<std::string>();
    const HTNAtom&     ValueColor      = Value.GetListElement(HTNValueExpressionNodeResult::ID_COLOR);
    const ImVec4       ValueImGuiColor = ImVec4(ValueColor.GetListElement(HTNValueExpressionNodeColorResult::X).GetValue<float>(),
                                                ValueColor.GetListElement(HTNValueExpressionNodeColorResult::Y).GetValue<float>(),
                                                ValueColor.GetListElement(HTNValueExpressionNodeColorResult::Z).GetValue<float>(),
                                                ValueColor.GetListElement(HTNValueExpressionNodeColorResult::W).GetValue<float>());
    ImGui::TextColored(ValueImGuiColor, ValueString.c_str());
}

void HTNDecompositionWatchPrinterBase::PrintValueValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode,
                                                                     HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    const std::string ValueString = GetNodeValue(*inValueExpressionNode, ioDomainPrinterContext)
                                        .GetListElement(HTNValueExpressionNodeResult::VALUE_STRING)
                                        .GetValue<std::string>();
    ImGui::Text(ValueString.c_str());
}
#endif
