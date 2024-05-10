#include "gui.h"
#include "imguiFunctions.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
);

void embraceTheDarkness();

LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{
	case WM_SIZE: {
		if (gui::pDevice && wParam != SIZE_MINIMIZED) {
			gui::presentParameters.BackBufferWidth = LOWORD(lParam);
			gui::presentParameters.BackBufferHeight = HIWORD(lParam);
			gui::ResetDevice();
		}
	} return 0;

	case WM_SYSCOMMAND: {
		if ((wParam & 0xFFF0) == SC_KEYMENU)
			return 0;
	} break;

	case WM_DESTROY: {
		PostQuitMessage(0);
		exit(0);
	} return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(lParam);
	} return 0;

	case WM_DROPFILES: {
		char szFileName[MAX_PATH];
		HDROP hDrop = (HDROP)wParam;
		int count = DragQueryFileA(hDrop, 0xFFFFFFFF, szFileName, MAX_PATH);
		for(int i = 0; i < count; i++)
		{
			// Grab the name of the file associated with index "i" in the list of files dropped.
			// Be sure you know that the name is attached to the FULL path of the file.
			DragQueryFileA(hDrop, i, szFileName, MAX_PATH);

			ImGui::OpenFile(szFileName);
		}
	} return 0;

	case WM_MOUSEMOVE: {
		if (wParam == MK_LBUTTON) {
			const auto points = MAKEPOINTS(lParam);
			auto rect = ::RECT{};

			GetWindowRect(gui::hWnd, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::hWnd,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}
	} return 0;
	}


	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void gui::CreateHWindow(const wchar_t* szWindowName, const wchar_t* szClassName) noexcept {
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = szClassName;
	windowClass.hIconSm = 0;

	RegisterClassExW(&windowClass);

	hWnd = CreateWindowW(
		szClassName,
		szWindowName,
		WS_POPUP,
		0,
		100,
		WIDTH,
		HEIGHT - 100,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
}

void gui::DestroyHWidow() noexcept {
	DestroyWindow(hWnd);
	UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept {
	pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D9)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferWidth = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (pD3D9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&pDevice) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept {
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = pDevice->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept {
	if (pDevice) {
		pDevice->Release();
		pDevice = nullptr;
	}

	if (pD3D9) {
		pD3D9->Release();
		pD3D9 = nullptr;
	}
}

void gui::CreateImGui() noexcept {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	#pragma warning(push)
	#pragma warning(disable: 4996)
	std::string font{ getenv("WINDIR") }; font += "\\Fonts\\Arialbd.TTF";
	#pragma warning(pop)
	io.Fonts->AddFontFromFileTTF(font.c_str(), 14.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	ImGui::StyleColorsLight();
	
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.31f, 0.25f, 0.24f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.62f, 0.70f, 0.72f, 0.56f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.33f, 0.14f, 0.47f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.97f, 0.31f, 0.13f, 0.81f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.79f, 0.18f, 0.78f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.79f, 0.18f, 0.30f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.40f, 0.62f, 0.80f, 0.15f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.39f, 0.64f, 0.80f, 0.30f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.67f, 0.80f, 0.59f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.48f, 0.53f, 0.67f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.48f, 0.47f, 0.47f, 0.71f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.31f, 0.47f, 0.99f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.79f, 0.18f, 0.78f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.79f, 0.18f, 0.38f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.79f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.65f, 0.78f, 0.84f, 0.80f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.75f, 0.88f, 0.94f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.55f, 0.68f, 0.74f, 0.80f);//ImVec4(0.46f, 0.84f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.60f, 0.60f, 0.80f, 0.30f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.99f, 0.54f, 0.43f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowRounding = 10.f;
	style.FrameRounding = 5.f;
	style.Alpha = 1.0f;
	style.FrameRounding = 4;
	style.IndentSpacing = 12.0f;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pDevice);
	ImGui_ImplWin32_EnableAlphaCompositing(hWnd);
}

void gui::DestroyImGui() noexcept {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept {
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept {
	ImGui::EndFrame();
	
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 0), 1.f, 0);

	if (pDevice->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		pDevice->EndScene();
	}

	const auto result = pDevice->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

void gui::Render() noexcept {
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT - 100), ImGuiCond_FirstUseEver);
	ImGui::Begin(
		"NotePad",
		&bExit,
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize
	);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")){
			if (ImGui::MenuItem("Open", "Ctrl+O")) {
				ImGui::OpenFile();
			}
			if (ImGui::MenuItem("New Text File", "Ctrl+N")) {
				ImGui::CreateNewTab(); // from include/imguiFunctions.h
			}
			if (ImGui::MenuItem("New File", "Ctrl+Shift+N")) {
				bIsNeedOpenNewFileWindow = true;
			}
			
			if (ImGui::MenuItem("Save", "Ctrl+S")) {
				ImGui::SaveFile();
			}
			if (ImGui::MenuItem("Close", "Ctrl+W")) {
				ImGui::CloseTab(nCurrentTab);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (ImGui::BeginTabBar("FilesTab", ImGuiTabBarFlags_AutoSelectNewTabs)) {
		for (int i{}; i < fileManager.size(); i++) {
			if (ImGui::BeginTabItem(ImGui::cp1251_to_utf8(fileManager[i].m_szName.c_str()).c_str())) {
				nCurrentTab = i;
				std::string szName = "##" + std::to_string(i);
				ImGui::InputTextMultiline(szName.c_str(), &fileManager[i].m_szBuffer, ImVec2(ImGui::GetWindowWidth() - 15, ImGui::GetWindowHeight() - 100), ImGuiInputTextFlags_AllowTabInput);
				ImGui::EndTabItem();
			}
		}
		if (ImGui::BeginTabItem("+", nullptr, ImGuiTabItemFlags_Button)) {
			ImGui::CreateNewTab();
			ImGui::EndTabItem();
		}

		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyReleased(ImGuiKey_O)) {
			ImGui::OpenFile();
		}
		
		else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_N)) {
			ImGui::CreateNewTab();
		}

		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_W) && fileManager.size() > 0) {
			ImGui::CloseTab(nCurrentTab);
		}

		if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyReleased(ImGuiKey_S) && fileManager.size() > 0) {
			ImGui::SaveFile();
		}

		ImGui::EndTabBar();
	}

	if (bIsNeedOpenNewFileWindow) {
		ImGui::OpenPopup("Create New File");
		bNewFileWindowEnable = true;
		bIsNeedOpenNewFileWindow = false;
	}

	ImGui::SetNextWindowSize(ImVec2(250.f, 100.f));
	if (ImGui::BeginPopupModal("Create New File", &bNewFileWindowEnable, ImGuiWindowFlags_NoResize)) {
		ImGui::PushItemWidth(ImGui::GetWindowWidth() - 20.f);
		ImGui::InputTextWithHint("##NewFileInput", "Input file name", szNewFileBuffer, sizeof(szNewFileBuffer));
		ImGui::NewLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 110.f);
		if (ImGui::Button("Create") || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
			if (strlen(szNewFileBuffer) > 0)
				ImGui::CreateNewTab(szNewFileBuffer);
			else 
				ImGui::CreateNewTab();
			bNewFileWindowEnable = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Close") || ImGui::IsKeyPressed(ImGuiKey_Escape))
			bNewFileWindowEnable = false;
		ImGui::EndPopup();
	}

	ImGui::End();
}