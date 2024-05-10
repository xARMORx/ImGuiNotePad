#pragma once
#include <Windows.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include <string>
#include <vector>
#include <fstream>
#pragma execution_character_set("utf-8")

namespace gui {
	class CFileManager {
	public:
		std::string m_szName;
		std::string m_szBuffer;
	};

	constexpr int WIDTH = 1366;
	constexpr int HEIGHT = 720;

	inline bool bExit = true;

	inline HWND hWnd = nullptr;
	inline WNDCLASSEXW windowClass = {};
	
	inline POINTS position = {};

	inline PDIRECT3D9 pD3D9 = nullptr;
	inline LPDIRECT3DDEVICE9 pDevice = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = {};
	
	inline std::vector<CFileManager> fileManager = {};
	inline int nNewFilesCount = {};
	inline int nCurrentTab = {};
	inline int nCharsCount = {};
	inline int nCursorPos = {};
	inline char szNewFileBuffer[256] = {}; // Буффер 
	inline bool bIsNeedOpenNewFileWindow = {}; // Для костиля
	inline bool bNewFileWindowEnable = {};

	void CreateHWindow(const wchar_t* szWindowName, const wchar_t* szClassName) noexcept;
	void DestroyHWidow() noexcept;

	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
};