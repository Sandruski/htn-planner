#include "Domain/Nodes/HTNDomainNode.h"

#include "Domain/Nodes/HTNAxiomNode.h"
#include "Domain/Nodes/HTNConstantNode.h"
#include "Domain/Nodes/HTNConstantsNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Nodes/HTNNodeVisitorBase.h"
#include "Domain/Nodes/HTNValueExpressionNode.h"

HTNDomainNode::HTNDomainNode(const std::shared_ptr<const HTNIdentifierExpressionNode>&   inIDNode,
                                    const std::vector<std::shared_ptr<const HTNConstantsNode>>& inConstantNodes,
                                    const std::vector<std::shared_ptr<const HTNAxiomNode>>&     inAxiomNodes,
                                    const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes, const bool inIsTopLevel)
    : mIDNode(inIDNode), mConstantNodes(inConstantNodes), mAxiomNodes(inAxiomNodes), mMethodNodes(inMethodNodes), mIsTopLevel(inIsTopLevel)
{
}

HTNAtom HTNDomainNode::Accept(HTNNodeVisitorBase& ioNodeVisitor) const
{
    return ioNodeVisitor.Visit(*this);
}

std::string HTNDomainNode::GetID() const
{
    return mIDNode->ToString();
}

std::shared_ptr<const HTNConstantNode> HTNDomainNode::FindConstantNodeByID(const std::string& inConstantNodeID) const
{
    for (const std::shared_ptr<const HTNConstantsNode>& ConstantNodes : mConstantNodes)
    {
        const std::vector<std::shared_ptr<const HTNConstantNode>>& ConstantNodesContainer = ConstantNodes->GetConstantNodes();
        for (const std::shared_ptr<const HTNConstantNode>& ConstantNode : ConstantNodesContainer)
        {
            if (inConstantNodeID == ConstantNode->GetID())
            {
                return ConstantNode;
            }
        }
    }

    return nullptr;
}

std::shared_ptr<const HTNAxiomNode> HTNDomainNode::FindAxiomNodeByID(const std::string& inAxiomNodeID) const
{
    for (const std::shared_ptr<const HTNAxiomNode>& AxiomNode : mAxiomNodes)
    {
        if (inAxiomNodeID == AxiomNode->GetID())
        {
            return AxiomNode;
        }
    }

    return nullptr;
}

std::shared_ptr<const HTNMethodNode> HTNDomainNode::FindMethodNodeByID(const std::string& inMethodNodeID) const
{
    for (const std::shared_ptr<const HTNMethodNode>& MethodNode : mMethodNodes)
    {
        if (inMethodNodeID == MethodNode->GetID())
        {
            return MethodNode;
        }
    }

    return nullptr;
}
