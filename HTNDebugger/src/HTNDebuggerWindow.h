#pragma once

#ifdef HTN_DEBUG
class HTNDatabaseHook;
class HTNPlannerHook;
class HTNPlanningUnit;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNPlannerHook& inPlannerHook, HTNDatabaseHook& inDatabaseHook, HTNPlanningUnit& inMainPlanningUnit, HTNPlanningUnit& inUpperBodyPlanningUnit);

    void Render(bool& _IsOpen);

private:
    void RenderActivePlan();
    void RenderDecomposition();
    void RenderDomain();
    void RenderWorldState();

    HTNPlannerHook*  mPlannerHook           = nullptr;
    HTNDatabaseHook* mDatabaseHook          = nullptr;
    HTNPlanningUnit* mMainPlanningUnit      = nullptr;
    HTNPlanningUnit* mUpperBodyPlanningUnit = nullptr;
};

inline HTNDebuggerWindow::HTNDebuggerWindow(HTNPlannerHook& inPlannerHook, HTNDatabaseHook& inDatabaseHook, HTNPlanningUnit& inMainPlanningUnit,
                                            HTNPlanningUnit& inUpperBodyPlanningUnit)
    : mPlannerHook(&inPlannerHook), mDatabaseHook(&inDatabaseHook), mMainPlanningUnit(&inMainPlanningUnit), mUpperBodyPlanningUnit(&inUpperBodyPlanningUnit)
{
}
#endif
