#include <Windows.h>
#include "include/imgui_hook.h"
#include "include/imgui/imgui.h"

void RenderMain() 
{
	ImGui::Begin("Window Title");
	ImGui::Text("Hello world!");
	ImGui::End();
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		ImGuiHook::Load(RenderMain);
		break;
	case DLL_PROCESS_DETACH:
		ImGuiHook::Unload();
		break;
	}
	return TRUE;
}