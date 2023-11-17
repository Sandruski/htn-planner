# HTN
A hierarchical task network (HTN) planner with a custom domain language.

## Disclaimer
Note that this project is a work in progress (WIP). As such, certain features may be incomplete or subject to change. Feedback and contributions are welcomed.

## Getting Started
1. Clone the repository with `git clone https://github.com/Sandruski/HTN`.
2. Generate the solution by running [GenerateProjectFiles.bat](https://github.com/Sandruski/HTN/blob/main/GenerateProjectFiles.bat). By default, it is generated for Visual Studio 2022. You can change the version of Visual Studio by editing this file.
3. If you add or remove project files, regenerate the solution by rerunning [GenerateProjectFiles.bat](https://github.com/Sandruski/HTN/blob/main/GenerateProjectFiles.bat).

## Usage
The solution contains three projects, HTNPlanner, HTNDemo, and HTNTest. HTNPlanner is the core planner, HTNDemo is a playground for testing the planner, and HTNTest contains all the unit tests of the planner.

## Third-Party
- [SDL](https://www.libsdl.org/)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GoogleTest](https://google.github.io/googletest/)
- [Premake](https://premake.github.io/)
- [Optick](https://github.com/bombomby/optick)

## Reference
- [Exploring HTN Planners
through Example](https://www.gameaipro.com/GameAIPro/GameAIPro_Chapter12_Exploring_HTN_Planners_through_Example.pdf)
- [derplanner](https://github.com/alexshafranov/derplanner)
- [The AI of Horizon Zero Dawn](https://www.guerrilla-games.com/read/the-ai-of-horizon-zero-dawn)
- [Hierarchical AI for Multiplayer
Bots in Killzone 3](https://www.gameaipro.com/GameAIPro/GameAIPro_Chapter29_Hierarchical_AI_for_Multiplayer_Bots_in_Killzone_3.pdf)

## Future Work
- Call terms
- Includes
- Overrides