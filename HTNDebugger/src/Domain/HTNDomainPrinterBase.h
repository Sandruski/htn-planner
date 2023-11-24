// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "HTNCoreMinimal.h"

#include "imgui.h"

#include <memory>
#include <string>

class HTNIdentifierExpressionNode;
class HTNNodeVisitorContextBase;
class HTNValueExpressionNodeBase;

class HTNDomainPrinterBase : public HTNNodeVisitorBase
{
public:
    virtual ~HTNDomainPrinterBase() = 0;

    HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

protected:
    void PrintValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode, const std::string& inPrefix,
                                  HTNNodeVisitorContextBase& ioDomainPrinterContext) const;
    void PrintColoredValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode, const std::string& inPrefix,
                                         const ImVec4& inColor, HTNNodeVisitorContextBase& ioDomainPrinterContext) const;
    void PrintColoredValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode, const std::string& inPrefix,
                                         HTNNodeVisitorContextBase& ioDomainPrinterContext) const;
};
#endif
