#include "Parser/AST/HTNAxiomNode.h"

#include "Parser/AST/HTNConditionNode.h"
#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

#include <format>

HTNAxiomNode::HTNAxiomNode(const std::shared_ptr<const HTNValueNode>&              inIDNode,
                           const std::vector<std::shared_ptr<const HTNValueNode>>& inArgumentNodes,
                           const std::shared_ptr<const HTNConditionNodeBase>&      inConditionNode)
    : mIDNode(inIDNode), mArgumentNodes(inArgumentNodes), mConditionNode(inConditionNode)
{
}

HTNAxiomNode::~HTNAxiomNode() = default;

std::vector<std::shared_ptr<const HTNTaskNode>> HTNAxiomNode::Accept(const HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNAxiomNode::GetID() const
{
    return GetIDNode()->ToString();
}

std::string HTNAxiomNode::ToString() const
{
    std::string Description = GetID();

    for (const std::shared_ptr<const HTNValueNode>& ArgumentNode : mArgumentNodes)
    {
        Description.append(std::format("{} ", ArgumentNode->ToString()));
    }

    // Remove last " "
    const size_t Position = Description.find_last_of(" ");
    if (Position != std::string::npos)
    {
        Description.erase(Position);
    }

    return Description;
}
