// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Interpreter/HTNDomainInterpreter.h"
#include "Domain/Parser/HTNDomainLexer.h"
#include "Domain/Parser/HTNDomainParser.h"

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
    // Parses a domain file and builds a domain node
    bool ParseDomainFile(const std::string& inDomainFilePath);

    bool MakePlan(HTNDecompositionContext& ioDecompositionContext) const;

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;

    const HTNDomainLexer mDomainLexer = HTNDomainLexer();
    const HTNDomainParser mDomainParser = HTNDomainParser();
    const HTNDomainInterpreter mDomainInterpreter = HTNDomainInterpreter();
};

inline const std::shared_ptr<const HTNDomainNode>& HTNPlannerHook::GetDomainNode() const
{
    return mDomainNode;
}
