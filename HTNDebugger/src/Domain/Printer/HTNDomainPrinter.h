// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

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
    bool Print(HTNDomainPrinterContext& ioDomainPrinterContext) const;

    HTNAtom Visit(const HTNDomainNode& inDomainNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNConstantsNode& inConstantsNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNConstantNode& inConstantNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNAxiomNode& inAxiomNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNMethodNode& inMethodNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNBranchNode& inBranchNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNConditionNode& inConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNAxiomConditionNode& inAxiomConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNAndConditionNode& inAndConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNOrConditionNode& inOrConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNAltConditionNode& inAltConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNNotConditionNode& inNotConditionNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNCompoundTaskNode& inCompoundTaskNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
    HTNAtom Visit(const HTNPrimitiveTaskNode& inPrimitiveTaskNode, HTNNodeVisitorContextBase& ioDomainPrinterContext) const final;
};
#endif