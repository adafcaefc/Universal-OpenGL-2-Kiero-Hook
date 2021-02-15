#include <Windows.h>
#include "imgui_hook.h"
#include "imgui/imgui.h"

void RenderMain() {
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
		CreateThread(nullptr, NULL, MainImGuiThread, hMod, NULL, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		ImGuiUnload();
		break;
	}
	return TRUE;
}