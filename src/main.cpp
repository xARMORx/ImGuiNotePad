#include "gui.h"
#include <chrono>
#include <thread>

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	gui::CreateHWindow(L"ImGui Window", L"ImGuiClass");
	DragAcceptFiles(gui::hWnd, true);
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::bExit) {
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	gui::DestroyImGui();
	gui::DestroyDevice();
	DragAcceptFiles(gui::hWnd, false);
	gui::DestroyHWidow();

	return EXIT_SUCCESS;
}