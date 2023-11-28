// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNDecompositionWatchTooltipPrinter.h"
#include "Domain/Printer/HTNDomainPrinterBase.h"
#include "HTNCoreMinimal.h"

#include <functional>
#include <string>

class HTNDecompositionPrinterContext;
class HTNNodeBase;
class HTNNodeInstance;
class HTNNodeSnapshot;
class HTNNodeVisitorContextBase;
enum class HTNNodeStep : uint8;

// Function that prints the title of the node
using HTNNodeTitleFunction = std::function<void(const HTNNodeSnapshot& inNodeSnapshot, const HTNNodeStep inNodeStep)>;

// Function that executes the behavior of the node
using HTNNodeBehaviorFunction = std::function<void()>;

// Function that returns an instance of the node
using HTNNodeInstanceFunction = std::function<HTNNodeInstance(const HTNNodeSnapshot& inNodeSnapshot, const std::string& inNodeLabel)>;

typedef int32 ImGuiTreeNodeFlags;

/**
 * Prints a decomposition
 */
class HTNDecompositionPrinter final : public HTNDomainPrinterBase
{
public:
    // Main print member method
    bool Print(HTNDecompositionPrinterContext& ioDecompositionPrinterContext) const;

    // Visits the given domain node with the given decomposition printer context
    HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given axiom node with the given decomposition printer context
    HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given method node with the given decomposition printer context
    HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given branch node with the given decomposition printer context
    HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given condition node with the given decomposition printer context
    HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given axiom condition node with the given decomposition printer context
    HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given and condition node with the given decomposition printer context
    HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given or condition node with the given decomposition printer context
    HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given alt condition node with the given decomposition printer context
    HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given not condition node with the given decomposition printer context
    HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given compound task node with the given decomposition printer context
    HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

    // Visits the given primitive task node with the given decomposition printer context
    HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const final;

private:
    // Prints a node
    HTN_NODISCARD bool PrintNode(const HTNNodeBase& inNode, const HTNNodeTitleFunction inNodeTitleFunction,
                                 const HTNNodeBehaviorFunction inNodeBehaviorFunction, const HTNNodeInstanceFunction inNodeInstanceFunction,
                                 const ImGuiTreeNodeFlags inTreeNodeFlags, HTNNodeVisitorContextBase& ioDecompositionPrinterContext) const;

    const HTNDecompositionWatchTooltipPrinter mDecompositionWatchTooltipPrinter = HTNDecompositionWatchTooltipPrinter();
};
#endif
