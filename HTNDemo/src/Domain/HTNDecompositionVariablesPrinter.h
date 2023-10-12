#pragma once

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"

#include <memory>
#include <string>
#include <vector>

class HTNDomainNode;
class HTNValueNodeBase;

/**
 * Prints the variables of a decomposition
 */
class HTNDecompositionVariablesPrinter final : public HTNNodeVisitorBase
{
public:
    explicit HTNDecompositionVariablesPrinter(const std::shared_ptr<const HTNDomainNode>& inDomainNode, const HTNVariables& inVariables,
                                              const std::string& inVariableScopePath);

    void Print(const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArguments);

    HTNAtom Visit(const HTNConstantNode& inConstantNode) final;
    HTNAtom Visit(const HTNVariableValueNode& inVariableValueNode) final;
    HTNAtom Visit(const HTNConstantValueNode& inConstantValueNode) final;

private:
    bool IsArgument(const std::string& inVariableID, const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArguments);

    std::shared_ptr<const HTNDomainNode> mDomainNode;
    const HTNVariables&                  mVariables;
    const std::string&                   mVariableScopePath;

    HTNDecompositionContext mDecompositionContext;
};
