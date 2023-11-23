// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

class HTNDecompositionWatchPrinterBase : public HTNNodeVisitorBase
{
public:
    virtual ~HTNDecompositionWatchPrinterBase() = 0;

    HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext) const final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext) const final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode,
                  HTNNodeVisitorContextBase&       ioDecompositionWatchPrinterContext) const final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode,
                  HTNNodeVisitorContextBase&       ioDecompositionWatchPrinterContext) const final;
};
#endif
