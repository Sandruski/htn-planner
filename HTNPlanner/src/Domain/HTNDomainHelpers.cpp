#include "Domain/HTNDomainHelpers.h"

#include "Domain/Nodes/HTNTaskNode.h"
#include "Domain/Nodes/HTNValueNode.h"

namespace HTNDomainHelpers
{
std::shared_ptr<const HTNCompoundTaskNode> MakeTopLevelCompoundTaskNode(const std::string& inEntryPointID)
{
    static constexpr bool IsIdentifier = true;
    static constexpr bool IsTopLevel   = true;
    return std::make_shared<HTNCompoundTaskNode>(std::make_shared<const HTNValueNode>(inEntryPointID, IsIdentifier),
                                                 std::vector<std::shared_ptr<const HTNValueNodeBase>>(), IsTopLevel);
}
} // namespace HTNDomainHelpers
