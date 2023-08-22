#include "Interpreter/AST/HTNBranch.h"

#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/AST/HTNValue.h"

#include <format>

HTNBranch::HTNBranch(const std::shared_ptr<const HTNValue>& inIDArgument, const std::shared_ptr<const HTNConditionBase>& inPreCondition,
                     const std::vector<std::shared_ptr<const HTNTask>>& inTasks)
    : mIDArgument(inIDArgument), mPreCondition(inPreCondition), mTasks(inTasks)
{
}

HTNBranch::~HTNBranch() = default;

std::vector<std::shared_ptr<const HTNTask>> HTNBranch::Accept(const HTNNodeVisitorBase& inVisitor) const
{
    return inVisitor.Visit(*this);
}

std::string HTNBranch::GetID() const
{
    return mIDArgument->ToString();
}

std::string HTNBranch::ToString() const
{
    return GetID();
}
