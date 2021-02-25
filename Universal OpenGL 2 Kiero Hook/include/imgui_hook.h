#pragma once
#include <Windows.h>
#include <functional>

namespace ImGuiHook {
	void Load(HMODULE hMod, std::function<void()> render);
	void Unload();
}
