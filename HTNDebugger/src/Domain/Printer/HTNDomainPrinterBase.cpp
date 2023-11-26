// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Printer/HTNDomainPrinterBase.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Nodes/HTNValueExpressionNode.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

namespace
{
enum HTNValueExpressionNodeResult : uint8
{
    STRING,
    COLOR
};

enum HTNValueExpressionNodeColorResult : uint8
{
    X,
    Y,
    Z,
    W
};
} // namespace

HTNDomainPrinterBase::~HTNDomainPrinterBase() = default;

HTNAtom HTNDomainPrinterBase::Visit(const HTNIdentifierExpressionNode&          inIdentifierExpressionNode,
                                    HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetIdentifierExpressionNodeValue");

    static constexpr bool ShouldDoubleQuoteString = false;
    const std::string     IdentifierString        = inIdentifierExpressionNode.GetValue().ToString(ShouldDoubleQuoteString);
    return HTNAtomList({IdentifierString});
}

HTNAtom HTNDomainPrinterBase::Visit(const HTNLiteralExpressionNode&             inLiteralExpressionNode,
                                    HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetLiteralExpressionNodeValue");

    static constexpr bool   ShouldDoubleQuoteString = true;
    const std::string       LiteralString           = inLiteralExpressionNode.GetValue().ToString(ShouldDoubleQuoteString);
    static constexpr ImVec4 LiteralColor            = HTNImGuiHelpers::kArgumentColor;
    return HTNAtomList({LiteralString, HTNAtomList({LiteralColor.x, LiteralColor.y, LiteralColor.z, LiteralColor.w})});
}

HTNAtom HTNDomainPrinterBase::Visit(const HTNVariableExpressionNode&            inVariableExpressionNode,
                                    HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetVariableExpressionNodeValue");

    static constexpr bool ShouldDoubleQuoteString = false;
    const std::string     VariableString          = std::format("?{}", inVariableExpressionNode.GetValue().ToString(ShouldDoubleQuoteString));
    const std::string&    VariableID              = inVariableExpressionNode.GetValue().GetValue<std::string>();
    const ImVec4          VariableColor           = HTNImGuiHelpers::GetVariableColor(VariableID);
    return HTNAtomList({VariableString, HTNAtomList({VariableColor.x, VariableColor.y, VariableColor.z, VariableColor.w})});
}

HTNAtom HTNDomainPrinterBase::Visit(const HTNConstantExpressionNode&            inConstantExpressionNode,
                                    HTN_MAYBE_UNUSED HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    OPTICK_EVENT("GetConstantExpressionNodeValue");

    static constexpr bool   ShouldDoubleQuoteString = false;
    const std::string       ConstantString          = std::format("@{}", inConstantExpressionNode.GetValue().ToString(ShouldDoubleQuoteString));
    static constexpr ImVec4 ConstantColor           = HTNImGuiHelpers::kArgumentColor;
    return HTNAtomList({ConstantString, HTNAtomList({ConstantColor.x, ConstantColor.y, ConstantColor.z, ConstantColor.w})});
}

void HTNDomainPrinterBase::PrintValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode,
                                                    const std::string& inPrefix, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    const std::string ValeString = std::format(
        "{}{}", inPrefix,
        GetNodeValue(*inValueExpressionNode, ioDomainPrinterContext).GetListElement(HTNValueExpressionNodeResult::STRING).GetValue<std::string>());
    ImGui::Text(ValeString.c_str());
}

void HTNDomainPrinterBase::PrintColoredValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode,
                                                           const std::string& inPrefix, const ImVec4& inColor,
                                                           HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    const std::string ValeString = std::format(
        "{}{}", inPrefix,
        GetNodeValue(*inValueExpressionNode, ioDomainPrinterContext).GetListElement(HTNValueExpressionNodeResult::STRING).GetValue<std::string>());
    ImGui::TextColored(inColor, ValeString.c_str());
}

void HTNDomainPrinterBase::PrintColoredValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode,
                                                           const std::string& inPrefix, HTNNodeVisitorContextBase& ioDomainPrinterContext) const
{
    const HTNAtom     Value       = GetNodeValue(*inValueExpressionNode, ioDomainPrinterContext);
    const std::string ValueString = std::format("{}{}", inPrefix, Value.GetListElement(HTNValueExpressionNodeResult::STRING).GetValue<std::string>());
    const HTNAtom&    ValueColor  = Value.GetListElement(HTNValueExpressionNodeResult::COLOR);
    const ImVec4      ValueImGuiColor = ImVec4(ValueColor.GetListElement(HTNValueExpressionNodeColorResult::X).GetValue<float>(),
                                               ValueColor.GetListElement(HTNValueExpressionNodeColorResult::Y).GetValue<float>(),
                                               ValueColor.GetListElement(HTNValueExpressionNodeColorResult::Z).GetValue<float>(),
                                               ValueColor.GetListElement(HTNValueExpressionNodeColorResult::W).GetValue<float>());
    ImGui::TextColored(ValueImGuiColor, ValueString.c_str());
}
#endif
