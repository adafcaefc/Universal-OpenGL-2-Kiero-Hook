#include "app.h"

#include "imgui_hook.h"
#include "external/imgui/imgui.h"

namespace app
{
    // This function is called once when the hook is initialized
    // You can use it to set up ImGui styles, fonts, etc.
    void initializeImGui()
    {
        ImGui::StyleColorsDark();
    }

    // This function is called every frame to render the ImGui interface
    // You can use it to create windows, buttons, sliders, etc.
    void renderImGui()
    {
        ImGui::Begin("Window Title");
        ImGui::Text("Hello world!");
        ImGui::End();
    }

    // Initialize the ImGui hook with the render and init functions
    void initializeHook()
    {
        if (!ImGuiHook::Load(renderImGui, initializeImGui))
        {
            // Failed to load the ImGui hook!
            // You can use ImGuiHook::GetLastError() to get the last error message
            // Handle the error here, e.g., log it or show a message box
        }
    }
}