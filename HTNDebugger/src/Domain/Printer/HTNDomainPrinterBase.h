// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <string>

class HTNNodeVisitorContextBase;
class HTNValueExpressionNodeBase;

struct ImVec4;

/**
 * Base class for printing a domain
 */
class HTNDomainPrinterBase : public HTNNodeVisitorBase
{
public:
    virtual ~HTNDomainPrinterBase() = 0;

    // Visits the given identifier expression node with the given domain printer context
    HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given literal expression node with the given domain printer context
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given variable expression node with the given domain printer context
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given constant expression node with the given domain printer context
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

protected:
    // Prints the value of the expression node
    void PrintValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode, const std::string& inPrefix,
                                  HTNNodeVisitorContextBase& ioDomainPrinterContext) const;

    // Prints the value of the expression node
    void PrintColoredValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode, const std::string& inPrefix,
                                         const ImVec4& inColor, HTNNodeVisitorContextBase& ioDomainPrinterContext) const;

    // Prints the value of the expression node
    void PrintColoredValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode, const std::string& inPrefix,
                                         HTNNodeVisitorContextBase& ioDomainPrinterContext) const;
};
#endif
