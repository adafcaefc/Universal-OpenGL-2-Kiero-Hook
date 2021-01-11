#pragma once
#include <Windows.h>
#include <GL/gl.h>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl2.h"

typedef BOOL(__stdcall* twglSwapBuffers) (HDC hDc);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);