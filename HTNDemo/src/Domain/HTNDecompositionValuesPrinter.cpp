#include "Domain/HTNDecompositionValuesPrinter.h"

#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"

#include "imgui.h"

void HTNDecompositionValuesPrinter::Print() const
{
    // TODO salvarez Highlight current variables
    // const std::vector<std::shared_ptr<const HTNValueNodeBase>>& ArgumentNodes;

    const HTNVariables& Variables = mNodeSnapshotDebug.GetVariables();
    for (const std::pair<std::string, HTNAtom>& VariablePair : Variables)
    {
        const std::string& VariableID          = VariablePair.first;
        const std::string  VariableValueString = VariablePair.second.ToString(false);
        ImGui::Text(VariableID.c_str());
        ImGui::SameLine();
        ImGui::Text(VariableValueString.c_str());
    }
}
