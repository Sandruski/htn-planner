# HTN
 A hierarchical task network (HTN) planner with a custom domain language.

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