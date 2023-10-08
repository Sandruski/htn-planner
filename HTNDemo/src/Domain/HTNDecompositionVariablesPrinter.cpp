#include "Domain/HTNDecompositionVariablesPrinter.h"

#include "Domain/Interpreter/HTNDecompositionContextHelpers.h"
#include "Domain/Nodes/HTNValueNode.h"
#include "HTNImGuiHelpers.h"

#include "imgui.h"

void HTNDecompositionVariablesPrinter::Print(const HTNVariables& inVariables, const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArguments)
{
    if (ImGui::BeginTable("Variables", 2, HTNImGuiHelpers::kDefaultTableFlags))
    {
        for (const std::pair<std::string, HTNAtom>& VariablePair : inVariables)
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            const std::string& VariablePath = VariablePair.first;
            std::string        VariableID;
            const bool         FindVariableIDResult = HTNDecompositionContextHelpers::FindVariableID(VariablePath, VariableID);
            assert(FindVariableIDResult);
            const bool IsArgumentResult = IsArgument(VariableID, inArguments);
            if (IsArgumentResult)
            {
                ImGui::Text(VariableID.c_str());
            }
            else
            {
                ImGui::TextDisabled(VariableID.c_str());
            }

            ImGui::TableNextColumn();
            const HTNAtom&    VariableValue           = VariablePair.second;
            constexpr bool    ShouldDoubleQuoteString = true;
            const std::string VariableValueString     = VariableValue.ToString(ShouldDoubleQuoteString);
            if (IsArgumentResult)
            {
                ImGui::Text(VariableValueString.c_str());
            }
            else
            {
                ImGui::TextDisabled(VariableValueString.c_str());
            }
        }

        ImGui::EndTable();
    }
}

HTNAtom HTNDecompositionVariablesPrinter::Visit(const HTNVariableValueNode& inVariableValueNode)
{
    return inVariableValueNode.GetValue();
}

bool HTNDecompositionVariablesPrinter::IsArgument(const std::string&                                          inVariableID,
                                                  const std::vector<std::shared_ptr<const HTNValueNodeBase>>& inArguments)
{
    for (const std::shared_ptr<const HTNValueNodeBase>& Argument : inArguments)
    {
        const HTNAtom ArgumentNodeValue = GetNodeValue(*Argument);
        if (!ArgumentNodeValue.IsSet())
        {
            continue;
        }

        const std::string VariableID = ArgumentNodeValue.GetValue<std::string>();
        if (inVariableID == VariableID)
        {
            return true;
        }
    }

    return false;
}
