#include <Windows.h>
#include <GL/gl.h>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl2.h"

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC oWndProc;
twglSwapBuffers owglSwapBuffers;
void RenderMain();

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HGLRC ImGuiWglContext;
static bool initImGui = false;
BOOL __stdcall hkwglSwapBuffers(HDC hDc)
{
	if (!initImGui)
	{
		HWND mWindow = WindowFromDC(hDc);
		oWndProc = (WNDPROC)SetWindowLongPtr(mWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);
		ImGuiWglContext = wglCreateContext(hDc);
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		// You can apply your io styles here
		ImGui_ImplWin32_Init(mWindow);
		ImGui_ImplOpenGL2_Init();
		initImGui = true;
	}

	HGLRC OldWglContext = wglGetCurrentContext();
	wglMakeCurrent(hDc, ImGuiWglContext);

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();
	RenderMain();
	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	wglMakeCurrent(hDc, OldWglContext);
	return owglSwapBuffers(hDc);
}

DWORD WINAPI MainImGuiThread(LPVOID lpParam)
{
	bool initHook = false;
	do
	{
		if (kiero::init(kiero::RenderType::Auto) == kiero::Status::Success)
		{
			// 336 is the index for wglSwapBuffers on this modified version of kiero
			kiero::bind(336, (void**)&owglSwapBuffers, hkwglSwapBuffers);
			initHook = true;
		}
		Sleep(250);
	} 
	while (!initHook);
	return TRUE;
}

// this function may still crash, I am working to find a fix
void ImGuiUnload() {
	kiero::shutdown();
}

