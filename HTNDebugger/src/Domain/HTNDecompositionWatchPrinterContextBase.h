#pragma once

#ifdef HTN_DEBUG
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"

#include <memory>
#include <string>
#include <vector>

class HTNDecompositionNode;
class HTNDomainNode;

class HTNDecompositionWatchPrinterContextBase : public HTNNodeVisitorContextBase
{
public:
    explicit HTNDecompositionWatchPrinterContextBase(const std::shared_ptr<const HTNDomainNode>& inDomainNode,
                                                     const HTNDecompositionNode&                 inDecompositionNode);

    void                                        AddVariablePath(const std::string& inVariablePath);
    const std::vector<std::string>&             GetNodeVariablePaths() const;
    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;
    const HTNDecompositionNode*                 GetNode() const;

private:
    std::vector<std::string>             mNodeVariablePaths;
    std::shared_ptr<const HTNDomainNode> mDomainNode;
    const HTNDecompositionNode*          mNode = nullptr;
};

inline void HTNDecompositionWatchPrinterContextBase::AddVariablePath(const std::string& inVariablePath)
{
    mNodeVariablePaths.emplace_back(inVariablePath);
}

inline const std::vector<std::string>& HTNDecompositionWatchPrinterContextBase::GetNodeVariablePaths() const
{
    return mNodeVariablePaths;
}

inline const std::shared_ptr<const HTNDomainNode>& HTNDecompositionWatchPrinterContextBase::GetDomainNode() const
{
    return mDomainNode;
}

inline const HTNDecompositionNode* HTNDecompositionWatchPrinterContextBase::GetNode() const
{
    return mNode;
}
#endif
