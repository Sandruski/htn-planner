// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#include "Domain/Interpreter/HTNDomainInterpreter.h"
#include "Domain/Parser/HTNDomainLexer.h"
#include "Domain/Parser/HTNDomainParser.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <string>

class HTNDecompositionContext;
class HTNDomainNode;

/**
 * Runtime instance of an HTN Planner
 */
class HTNPlannerHook
{
public:
    // Parses a domain file, building a domain
    bool ParseDomainFile(const std::string& inDomainFilePath);

    // Performs a decomposition on the domain
    // - This member function is const, so it cannot modify member variables, and receives the data that it can modify via a context
    // - This makes it safe to be called in parallel, which allows to run multiple decompositions simultaneously
    bool MakePlan(HTNDecompositionContext& ioDecompositionContext) const;

    // Returns the domain node
    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;

    const HTNDomainLexer       mDomainLexer       = HTNDomainLexer();
    const HTNDomainParser      mDomainParser      = HTNDomainParser();
    const HTNDomainInterpreter mDomainInterpreter = HTNDomainInterpreter();
};

inline const std::shared_ptr<const HTNDomainNode>& HTNPlannerHook::GetDomainNode() const
{
    return mDomainNode;
}
