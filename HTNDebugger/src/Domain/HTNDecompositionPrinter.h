// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionWatchTooltipPrinter.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "HTNCoreMinimal.h"

#include <functional>
#include <string>

class HTNDecompositionPrinterContext;
class HTNNodeBase;
class HTNNodeSnapshotDebug;
class HTNNodeVisitorContextBase;
enum class HTNNodeStep : uint8;

using HTNNodeTitleFunction    = std::function<void(const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep)>;
using HTNNodeBehaviorFunction = std::function<void()>;
using HTNNodeFunction         = std::function<HTNDecompositionNode(const HTNNodeSnapshotDebug& inNodeSnapshot, const std::string& inNodeLabel)>;

typedef int32 ImGuiTreeNodeFlags;

/**
 * Prints a decomposition
 */
class HTNDecompositionPrinter final : public HTNNodeVisitorBase
{
public:
    bool Print(HTNDecompositionPrinterContext& ioDecompositionPrinterContext) const;

    HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNIdentifierExpressionNode& inIdentifierExpressionNode,
                  HTNNodeVisitorContextBase&         ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNLiteralExpressionNode& inLiteralExpressionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNVariableExpressionNode& inVariableExpressionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;
    HTNAtom Visit(const HTNConstantExpressionNode& inConstantExpressionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

private:
    bool PrintNode(const HTNNodeBase& inNode, const HTNNodeTitleFunction inNodeTitleFunction, const HTNNodeBehaviorFunction inNodeBehaviorFunction,
                   const HTNNodeFunction inNodeFunction, const ImGuiTreeNodeFlags inTreeNodeFlags,
                   HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const;

    const HTNDecompositionWatchTooltipPrinter mDecompositionWatchTooltipPrinter = HTNDecompositionWatchTooltipPrinter();
};
#endif
