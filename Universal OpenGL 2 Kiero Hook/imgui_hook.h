/*
 * -----------------------------------------------------------------------------
 * Universal OpenGL2 ImGui Hook using Kiero by Adafcaefc
 * -----------------------------------------------------------------------------
 *
 * This project is inspired by rdbo's Universal DirectX 11 ImGui Hook using Kiero
 * https://github.com/rdbo/ImGui-DirectX-11-Kiero-Hook
 *
 * This project is using a modified version of Rebzzel's Kiero
 * https://github.com/Rebzzel/kiero
 *
 * This project is using ImGui as it's main GUI library
 * https://github.com/ocornut/imgui
 *
 * Special thanks to Sleepyut for helping me with the context rendering issue
 *
 * -----------------------------------------------------------------------------
 * Requirements
 * -----------------------------------------------------------------------------
 *
 * Visual Studio        https://visualstudio.microsoft.com/
 *
 */

#pragma once

#include <functional> 
#include <string>


namespace ImGuiHook
{
    /**
     * @brief Load the ImGui hook.
     *
     * This function sets up the necessary hooks to render ImGui in the target environment.
     *
     * @param render A function to be called every frame for rendering.
     * @param init An optional function to be called once when the hook is initialized.
     * @return true if the hook was loaded successfully, false otherwise.
     */
    bool Load(
        const std::function<void()>& render, 
        const std::function<void()>& init = []() {});

    /**
     * @brief Unload the ImGui hook.
     *
     * This function should be called when the DLL is detached or the hook is no longer needed.
     */
    void Unload();

    /**
     * @brief Get the last error message.
     *
     * This function returns the most recent error message encountered during hook operations.
     *
     * @return A string containing the last error message.
     */
    std::string GetLastError();
}
