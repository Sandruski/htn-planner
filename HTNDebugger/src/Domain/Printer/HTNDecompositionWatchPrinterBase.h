// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "HTNCoreMinimal.h"

#include <memory>

class HTNNodeVisitorContextBase;
class HTNValueExpressionNodeBase;

/**
 * Base class for a decomposition watch printer
 */
class HTNDecompositionWatchPrinterBase : public HTNNodeVisitorBase
{
public:
    virtual ~HTNDecompositionWatchPrinterBase() = 0;

    // Visits the given constant node with the given decomposition watch printer context
    HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext) const final;

    // Visits the given literal expression node with the given decomposition watch printer context
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioDecompositionWatchPrinterContext) const final;

    // Visits the given variable expression node with the given decomposition watch printer context
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode,
                  HTNNodeVisitorContextBase&       ioDecompositionWatchPrinterContext) const final;

    // Visits the given constant expression node with the given decomposition watch printer context
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode,
                  HTNNodeVisitorContextBase&       ioDecompositionWatchPrinterContext) const final;

protected:
    // Prints the ID of the value of the expression node
    void PrintColoredValueIDExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode,
                                           HTNNodeVisitorContextBase&                               ioDecompositionWatchPrinterContext) const;

    // Prints the value of the value of the expression node
    void PrintValueValueExpressionNode(const std::shared_ptr<const HTNValueExpressionNodeBase>& inValueExpressionNode,
                                       HTNNodeVisitorContextBase&                               ioDecompositionWatchPrinterContext) const;
};
#endif
