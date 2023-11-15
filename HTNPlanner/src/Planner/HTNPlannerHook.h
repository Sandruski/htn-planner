#pragma once

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

    bool MakePlan(const std::string& inEntryPointID, HTNDecompositionContext& ioDecompositionContext);

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;

    const HTNDomainLexer       mDomainLexer;
    const HTNDomainParser      mDomainParser;
    const HTNDomainInterpreter mDomainInterpreter;
};

inline const std::shared_ptr<const HTNDomainNode>& HTNPlannerHook::GetDomainNode() const
{
    return mDomainNode;
}
