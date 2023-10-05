#pragma once

#include <memory>
#include <string>

class HTNDecompositionContext;
class HTNDomainNode;

// Runtime instance of an HTN Planner.
//
class HTNPlannerHook
{
public:
    // Parses a domain file and builds a domain node
    bool ParseDomainFile(const std::string& inDomainFilePath);

    bool MakePlan(const std::string& inEntryPointName, HTNDecompositionContext& ioDecompositionContext) const;

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
};

inline const std::shared_ptr<const HTNDomainNode>& HTNPlannerHook::GetDomainNode() const
{
    return mDomainNode;
}
