#pragma once

#ifdef HTN_DEBUG
#include "Domain/Nodes/HTNNodeVisitorBase.h"

class HTNDecompositionWatchPrinterBase : public HTNNodeVisitorBase
{
public:
    HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioContext) final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioContext) final;
};
#endif
