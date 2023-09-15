#include "Parser/AST/HTNDomainNode.h"

#include "Parser/AST/HTNAxiomNode.h"
#include "Parser/AST/HTNConstantNode.h"
#include "Parser/AST/HTNConstantsNode.h"
#include "Parser/AST/HTNMethodNode.h"
#include "Parser/AST/HTNNodeVisitorBase.h"
#include "Parser/AST/HTNValueNode.h"

HTNDomainNode::HTNDomainNode(const std::shared_ptr<const HTNValueNode>&                  inIDNode,
                             const std::vector<std::shared_ptr<const HTNConstantsNode>>& inConstantNodes,
                             const std::vector<std::shared_ptr<const HTNAxiomNode>>&     inAxiomNodes,
                             const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes, const bool inIsTopLevel)
    : mIDNode(inIDNode), mConstantNodes(inConstantNodes), mAxiomNodes(inAxiomNodes), mMethodNodes(inMethodNodes), mIsTopLevel(inIsTopLevel)
{
}

HTNDomainNode::~HTNDomainNode() = default;

HTNAtom HTNDomainNode::Accept(HTNNodeVisitorBase& inNodeVisitor) const
{
    return inNodeVisitor.Visit(*this);
}

std::string HTNDomainNode::GetID() const
{
    return mIDNode->ToString();
}

std::string HTNDomainNode::ToString() const
{
    std::string Description = GetID();

    if (mIsTopLevel)
    {
        Description.append(" top_level_domain");
    }

    return Description;
}

std::shared_ptr<const HTNConstantNode> HTNDomainNode::FindConstantNodeByID(const std::string& inID) const
{
    for (const std::shared_ptr<const HTNConstantsNode>& ConstantNodes : mConstantNodes)
    {
        const std::vector<std::shared_ptr<const HTNConstantNode>>& ConstantNodesContainer = ConstantNodes->GetConstantNodes();
        for (const std::shared_ptr<const HTNConstantNode>& ConstantNode : ConstantNodesContainer)
        {
            if (inID == ConstantNode->GetID())
            {
                return ConstantNode;
            }
        }
    }

    return nullptr;
}

std::shared_ptr<const HTNAxiomNode> HTNDomainNode::FindAxiomNodeByID(const std::string& inID) const
{
    for (const std::shared_ptr<const HTNAxiomNode>& AxiomNode : mAxiomNodes)
    {
        if (inID == AxiomNode->GetID())
        {
            return AxiomNode;
        }
    }

    return nullptr;
}

std::shared_ptr<const HTNMethodNode> HTNDomainNode::FindMethodNodeByID(const std::string& inID) const
{
    for (const std::shared_ptr<const HTNMethodNode>& MethodNode : mMethodNodes)
    {
        if (inID == MethodNode->GetID())
        {
            return MethodNode;
        }
    }

    return nullptr;
}
