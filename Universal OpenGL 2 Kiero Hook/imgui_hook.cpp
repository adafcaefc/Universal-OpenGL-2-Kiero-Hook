#include <Windows.h>
#include <functional> 
#include <GL/gl.h>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl2.h"

#define _CAST(t,v)  reinterpret_cast<t>(v)
#define _VOID(v)	std::function<void(v)>

typedef BOOL(__stdcall* wglSwapBuffers_t) (
	HDC hDc
);

typedef LRESULT(CALLBACK* WNDPROC) (
	IN HWND   hwnd,
	IN UINT   uMsg,
	IN WPARAM wParam,
	IN LPARAM lParam
);

extern LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam
);

extern void RenderMain();

namespace ImGuiHook 
{
	static WNDPROC			 oWndProc;
	static wglSwapBuffers_t  owglSwapBuffers;
	static HGLRC			 ImGuiWglContext;
	static bool				 initImGui = false;

	LRESULT __stdcall WndProc(
		const HWND	hWnd, 
		UINT		uMsg, 
		WPARAM		wParam, 
		LPARAM		lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;
		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
	}

	void InitOpenGL2(HDC hDc, bool* init) 
	{
		if (*init) return;
		HWND hWnd = WindowFromDC(hDc);
		oWndProc = _CAST(WNDPROC, SetWindowLongPtr(hWnd, GWLP_WNDPROC, _CAST(LONG_PTR, WndProc)));
		ImGuiWglContext = wglCreateContext(hDc);
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplOpenGL2_Init();
		*init = true;
	}

	void RenderWin32(std::function<void()> render)
	{
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		render();
		ImGui::EndFrame();
		ImGui::Render();
	}

	void RenderOpenGL2(
		HGLRC		   ImGuiWglContext,
		HDC			   hDc,
		_VOID(_VOID()) render,
		_VOID()		   render_inner)
	{
		HGLRC OldWglContext = wglGetCurrentContext();
		wglMakeCurrent(hDc, ImGuiWglContext);
		ImGui_ImplOpenGL2_NewFrame();
		render(render_inner);
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		wglMakeCurrent(hDc, OldWglContext);
		
	}

	BOOL __stdcall hkwglSwapBuffers(HDC hDc)
	{
		InitOpenGL2(hDc, &initImGui);
		RenderOpenGL2(ImGuiWglContext, hDc, RenderWin32, RenderMain);
		return owglSwapBuffers(hDc);
	}

	wglSwapBuffers_t* get_wglSwapBuffers()
	{
		auto hMod = GetModuleHandleA("OPENGL32.dll");
		if (!hMod) return nullptr;
		_CAST(wglSwapBuffers_t*, GetProcAddress(hMod, "wglSwapBuffers"));
	}

	DWORD WINAPI Main(LPVOID lpParam)
	{
		bool initHook = false;
		do
		{
			if (kiero::init(kiero::RenderType::Auto) == kiero::Status::Success)
			{
				kiero::bind(get_wglSwapBuffers(), (void**)&owglSwapBuffers, hkwglSwapBuffers);
				initHook = true;
			}
			Sleep(250);
		} while (!initHook);
		return TRUE;
	}

	// This function may still crashes, I am working to find a fix
	void Unload()
	{
		kiero::shutdown();
	}
}


