// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNDomainPrinterBase.h"
#include "HTNCoreMinimal.h"

class HTNDomainPrinterContext;

/**
 * Prints a domain
 */
class HTNDomainPrinter final : public HTNDomainPrinterBase
{
public:
    // Main print member method
    bool Print(HTNDomainPrinterContext& ioDomainPrinterContext) const;

    // Visits the given domain node with the given domain printer context
    HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given constants node with the given domain printer context
    HTNAtom Visit(const HTNConstantsNode& inConstantsNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given constant node with the given domain printer context
    HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given axiom node with the given domain printer context
    HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given method node with the given domain printer context
    HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given branch node with the given domain printer context
    HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given condition node with the given domain printer context
    HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given axiom condition node with the given domain printer context
    HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given and condition node with the given domain printer context
    HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given or condition node with the given domain printer context
    HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given alt condition node with the given domain printer context
    HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given not condition node with the given domain printer context
    HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given compound task node with the given domain printer context
    HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;

    // Visits the given primitive task node with the given domain printer context
    HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
};
#endif
