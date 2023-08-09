#pragma once

class HTNPlanningUnit;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNPlanningUnit& inPlanningUnit);

    void Render(bool& _IsOpen);

private:
    HTNPlanningUnit* mPlanningUnit = nullptr;
};

inline HTNDebuggerWindow::HTNDebuggerWindow(HTNPlanningUnit& inPlanningUnit)
    : mPlanningUnit(&inPlanningUnit)
{
}
