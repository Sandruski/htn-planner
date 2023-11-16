#pragma once

#ifdef HTN_DEBUG
#include "HTNCoreMinimal.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

class HTNDecompositionWatchPrinterBase : public HTNNodeVisitorBase
{
public:
    virtual ~HTNDecompositionWatchPrinterBase() = 0;

    HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioContext) const final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioContext) const final;
};
#endif
