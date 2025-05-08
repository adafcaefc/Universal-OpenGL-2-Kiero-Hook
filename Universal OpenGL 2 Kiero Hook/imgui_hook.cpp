#include "imgui_hook.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/gl.h>

#include "external/kiero/kiero.h"
#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_win32.h"
#include "external/imgui/imgui_impl_opengl2.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam);

namespace ImGuiHook 
{
    typedef BOOL(__stdcall* wglSwapBuffers_t) (HDC hDc);

    typedef LRESULT(CALLBACK* WNDPROC) (
        IN  HWND   hwnd,
        IN  UINT   uMsg,
        IN  WPARAM wParam,
        IN  LPARAM lParam);

    // Original functions variables
    static WNDPROC          g_WndProc_o        = nullptr;
    static wglSwapBuffers_t g_wglSwapBuffers_o = nullptr;

    // Global variables
    static HGLRC g_wglContext = nullptr;
    static bool	 g_initImGui  = false;
    static HWND  g_hWnd       = nullptr;

    // Render function variables
    static std::function<void()> g_renderMain = []() {};
    static std::function<void()> g_extraInit  = []() {};

    // Last error status
    static std::string g_lastError;

    // WndProc callback ImGui handler
    static LRESULT CALLBACK ImGui_WndProc(
        const HWND	hWnd, 
        UINT		uMsg, 
        WPARAM		wParam, 
        LPARAM		lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) 
            return true;

        return CallWindowProc(g_WndProc_o, hWnd, uMsg, wParam, lParam);
    }

    static bool InitPlatform()
    {
        if (!ImGui_ImplWin32_Init(g_hWnd))
        {
            g_lastError = "Failed to init ImGui_ImplWin32, g_initImGui = " + std::to_string(g_initImGui);
            return false;
        }
        if (!ImGui_ImplOpenGL2_Init())
        {
            g_lastError = "Failed to init ImGui_ImplOpenGL2, g_initImGui = " + std::to_string(g_initImGui);
            return false;
        }
        return true;
    }

    // Initialisation for ImGui
    bool Init_ImGui_OpenGL2(const HDC hDc) 
    {
        if (WindowFromDC(hDc) == g_hWnd && g_initImGui) 
            return true;

        g_hWnd = WindowFromDC(hDc);

        if (!g_hWnd) 
        {
            g_lastError = "Failed to get window handle from HDC";
            return false;
        }

        g_WndProc_o = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)ImGui_WndProc);

        if (!g_WndProc_o) 
        {
            g_lastError = "Failed to set WndProc";
            return false;
        }

        if (g_initImGui) 
            return InitPlatform();
        
        g_wglContext = wglCreateContext(hDc);
        if (!g_wglContext)
        {
            g_lastError = "Failed to create OpenGL context";
            return false;
        }

        IMGUI_CHECKVERSION();
        if (!ImGui::CreateContext())
        {
            g_lastError = "Failed to create ImGui context";
            return false;
        }

        if (!InitPlatform())
            return false;

        g_extraInit();

        g_initImGui = true;

        return true;
    }

    // Generic ImGui renderer for OpenGL2 backend
    bool Render_ImGui(const HDC hDc)
    {
        auto o_WglContext = wglGetCurrentContext();
        if (!o_WglContext)
        {
            g_lastError = "Failed to get current OpenGL context";
            return false;
        }
        if (!wglMakeCurrent(hDc, g_wglContext))
        {
            g_lastError = "Failed to make OpenGL context current";
            return false;
        }

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        g_renderMain();
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        if (!wglMakeCurrent(hDc, o_WglContext))
        {
            g_lastError = "Failed to make original OpenGL context current";
            return false;
        }

        return true;
    }

    // Hooked wglSwapBuffers function
    static BOOL WINAPI wglSwapBuffers_h(const HDC hDc)
    {
        Init_ImGui_OpenGL2(hDc);
        Render_ImGui(hDc);
        return g_wglSwapBuffers_o(hDc);
    }

    // Initialise hook
    static bool InitHook()
    {
        if (auto hMod = GetModuleHandleA("OPENGL32.dll")) 
            g_wglSwapBuffers_o = (wglSwapBuffers_t)GetProcAddress(hMod, "wglSwapBuffers");
        
        const auto status = kiero::init(kiero::RenderType::OpenGL);

        if (g_wglSwapBuffers_o && status == kiero::Status::Success)
            return kiero::bind(g_wglSwapBuffers_o, (void**)&g_wglSwapBuffers_o, wglSwapBuffers_h) == kiero::Status::Success;
        else
            g_lastError = "Failed to do kiero::init, error code: " + std::to_string(status);

        return false;
    }

    std::string GetLastError()
    {
        return g_lastError;
    }

    // Main load function
    bool Load(const std::function<void()>& render, const std::function<void()>& init)
    {
        g_renderMain = render;
        g_extraInit = init;
        return InitHook();
    }

    // Main unload function
    void Unload()
    {
        kiero::shutdown();
    }
}


