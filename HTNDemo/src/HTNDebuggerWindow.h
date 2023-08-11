#pragma once

#include <vector>

class HTNPlannerHook;
class HTNPlanningUnit;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNPlannerHook& inPlanner, const std::vector<HTNPlanningUnit*>& inPlanningUnits);

    void Render(bool& _IsOpen);

private:
    void RenderPlan();
    void RenderDatabase();
    void RenderDecomposition();

    void SelectPlanningUnit(HTNPlanningUnit*& inSelectedPlanningUnit) const;

    HTNPlannerHook*               mPlanner = nullptr;
    std::vector<HTNPlanningUnit*> mPlanningUnits;
};

inline HTNDebuggerWindow::HTNDebuggerWindow(HTNPlannerHook& inPlanner, const std::vector<HTNPlanningUnit*>& inPlanningUnits)
    : mPlanner(&inPlanner), mPlanningUnits(inPlanningUnits)
{
}
