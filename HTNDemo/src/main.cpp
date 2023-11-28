// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "HTNCoreMinimal.h"

#include "SDL.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "optick.h"

#include <cstdio>

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Hook/HTNDatabaseHook.h"
#include "Hook/HTNPlannerHook.h"
#include "Hook/HTNPlanningUnit.h"
#include "Window/HTNDebuggerWindow.h"
#endif

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

// Main code
int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    const SDL_WindowFlags WindowFlags = static_cast<SDL_WindowFlags>(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window*           Window      = SDL_CreateWindow("HTN Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, WindowFlags);
    SDL_Renderer*         Renderer    = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!Renderer)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& IO = ImGui::GetIO();
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(Window, Renderer);
    ImGui_ImplSDLRenderer2_Init(Renderer);

    static constexpr ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.6f, 1.f);

#ifdef HTN_DEBUG_DECOMPOSITION
    HTNDatabaseHook DatabaseHook;
    HTNPlannerHook  PlannerHook;

    // Planning units for a human, the main one and the upper body one
    static const std::string MainDefaultTopLevelMethodID      = "behave";
    HTNPlanningUnit          MainPlanningUnit                 = HTNPlanningUnit(DatabaseHook, PlannerHook, MainDefaultTopLevelMethodID);
    static const std::string UpperBodyDefaultTopLevelMethodID = "behave_upper_body";
    HTNPlanningUnit          UpperBodyPlanningUnit            = HTNPlanningUnit(DatabaseHook, PlannerHook, UpperBodyDefaultTopLevelMethodID);

    HTNDebuggerWindow DebuggerWindow = HTNDebuggerWindow(DatabaseHook, PlannerHook, MainPlanningUnit, UpperBodyPlanningUnit);
    DebuggerWindow.Init();
    bool ShouldShowHTNDebuggerWindow = true;
#endif

    // Main loop
    bool IsDone = false;
    while (!IsDone)
    {
        OPTICK_FRAME("MainThread");

        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            ImGui_ImplSDL2_ProcessEvent(&Event);
            if (Event.type == SDL_QUIT)
            {
                IsDone = true;
            }

            if (Event.type == SDL_WINDOWEVENT && Event.window.event == SDL_WINDOWEVENT_CLOSE && Event.window.windowID == SDL_GetWindowID(Window))
            {
                IsDone = true;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

#ifdef HTN_DEBUG_DECOMPOSITION
        if (ShouldShowHTNDebuggerWindow)
        {
            DebuggerWindow.Render();
        }
#endif

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(Renderer, IO.DisplayFramebufferScale.x, IO.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(Renderer, static_cast<Uint8>(ClearColor.x * 255), static_cast<Uint8>(ClearColor.y * 255),
                               static_cast<Uint8>(ClearColor.z * 255), static_cast<Uint8>(ClearColor.w * 255));
        SDL_RenderClear(Renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(Renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}
