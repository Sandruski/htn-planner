#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNAtom;
class HTNDomainNode;
class HTNTaskResult;
class HTNWorldState;

// Runtime instance of an HTN Planner.
//
class HTNPlannerHook
{
public:
    // Parses a domain file and builds an abstract syntax tree

    // The information needed would be: ConstantsGroups, Methods, Axioms.
    // Once we got all this information we need to know before hand what is the top_level method.
    // The top level method determines the root/parent node of the entire hierarchy.
    bool ParseDomainFile(const std::string& inDomainFilePath);

    // Returns an HTNAtom with the information about the constant requested.
    // Returns an unbinded atom if the constant wasn't found.
    const HTNAtom* GetConstantByName(const char* inConstantGroup, const char* inConstantId)
    {
        // TODO SANDRA.
        (void*)inConstantGroup;
        (void*)inConstantId;
        return nullptr;
    }

    bool MakePlan(const std::string& inEntryPointName, const HTNWorldState& inWorldState, std::vector<HTNTaskResult>& outPlan) const;

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

private:
    std::shared_ptr<const HTNDomainNode> mDomainNode;
};

inline const std::shared_ptr<const HTNDomainNode>& HTNPlannerHook::GetDomainNode() const
{
    return mDomainNode;
}
