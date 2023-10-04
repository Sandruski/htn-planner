#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNDomainNode;
class HTNTaskResult;
class HTNWorldState;

// Runtime instance of an HTN Planner.
//
class HTNPlannerHook
{
public:
    // Parses a domain file and builds a domain node
    bool ParseDomainFile(const std::string& inDomainFilePath);

    bool MakePlan(const std::string& inEntryPointName, const HTNWorldState& inWorldState, std::vector<HTNTaskResult>& outPlan) const;

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
};

inline const std::shared_ptr<const HTNDomainNode>& HTNPlannerHook::GetDomainNode() const
{
    return mDomainNode;
}
