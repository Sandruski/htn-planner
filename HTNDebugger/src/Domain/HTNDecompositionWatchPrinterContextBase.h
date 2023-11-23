// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
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
    virtual ~HTNDecompositionWatchPrinterContextBase() = 0;

    void AddVariablePath(const std::string& inVariablePath);

    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;
    const HTNDecompositionNode&                 GetNode() const;
    const std::vector<std::string>&             GetNodeVariablePaths() const;

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const std::shared_ptr<const HTNDomainNode>& mDomainNode;
    const HTNDecompositionNode& mNode;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    std::vector<std::string> mNodeVariablePaths;
};

inline void HTNDecompositionWatchPrinterContextBase::AddVariablePath(const std::string& inVariablePath)
{
    mNodeVariablePaths.emplace_back(inVariablePath);
}

inline const std::shared_ptr<const HTNDomainNode>& HTNDecompositionWatchPrinterContextBase::GetDomainNode() const
{
    return mDomainNode;
}

inline const HTNDecompositionNode& HTNDecompositionWatchPrinterContextBase::GetNode() const
{
    return mNode;
}

inline const std::vector<std::string>& HTNDecompositionWatchPrinterContextBase::GetNodeVariablePaths() const
{
    return mNodeVariablePaths;
}
#endif
