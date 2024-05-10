#pragma once
#include "gui.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

namespace ImGui {
     std::string cp1251_to_utf8(const char* str) {
        std::string res;
        int result_u, result_c;
        result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
        if (!result_u) { return 0; }
        wchar_t* ures = new wchar_t[result_u];
        if (!MultiByteToWideChar(1251, 0, str, -1, ures, result_u)) {
            delete[] ures;
            return 0;
        }
        result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
        if (!result_c) {
            delete[] ures;
            return 0;
        }
        char* cres = new char[result_c];
        if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0)) {
            delete[] cres;
            return 0;
        }
        delete[] ures;
        res.append(cres);
        delete[] cres;
        return res;
    }

    void CreateNewTab(const char* szFileName = "") {
        gui::CFileManager newManager;
        if (szFileName == ""){
            newManager.m_szName = "New File-" + std::to_string(gui::nNewFilesCount);
            gui::nNewFilesCount++;
        }
        else
            newManager.m_szName = szFileName;
        newManager.m_szBuffer = "";
        gui::fileManager.push_back(newManager);
    }

    void CloseTab(int nTabIndex) {
        if (gui::fileManager.size() > 0)
            gui::fileManager.erase(gui::fileManager.begin() + nTabIndex);
    }

    void SaveFile() {
        if (gui::fileManager.size() <= 0)
            return;
        OPENFILENAMEA ofn;

        char szFileName[MAX_PATH] = "";
        strcpy(szFileName, gui::fileManager[gui::nCurrentTab].m_szName.c_str());

        ZeroMemory(&ofn, sizeof(ofn));

        ofn.lStructSize = sizeof(ofn); 
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = szFileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
        ofn.lpstrDefExt = "txt";
        if (GetSaveFileNameA(&ofn)) {
            std::ofstream outfile(ofn.lpstrFile);
            if (outfile.is_open()) {
                outfile << gui::fileManager[gui::nCurrentTab].m_szBuffer;
                outfile.close();
            }
        }
        gui::fileManager[gui::nCurrentTab].m_szName = szFileName;
    }

    void OpenFile(char* szFileName = "") {
        if (szFileName == ""){
            OPENFILENAMEA ofn;

            char szOpenFileName[MAX_PATH] = "";

            ZeroMemory(&ofn, sizeof(ofn));

            ofn.lStructSize = sizeof(ofn); 
            ofn.hwndOwner = NULL;
            ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = szOpenFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
            ofn.lpstrDefExt = "txt";
            if (GetOpenFileNameA(&ofn)) {
                for (int i{}; i < gui::fileManager.size(); i++) {
                    if (gui::fileManager[i].m_szName == ofn.lpstrFile)
                        return;
                    else
                        continue;
                }
                std::ifstream infile(ofn.lpstrFile);
                if (infile.is_open()) {
                    std::string szTempText;
                    std::string szText;
                    while (std::getline(infile, szTempText)) {
                        szText += szTempText + '\n';
                    }
                    CreateNewTab(ofn.lpstrFile);
                    gui::fileManager[gui::fileManager.size() - 1].m_szBuffer = cp1251_to_utf8(szText.c_str());
                    infile.close();
                }
            }
        }
        else {
            for (int i{}; i < gui::fileManager.size(); i++) {
                if (gui::fileManager[i].m_szName == szFileName)
                    return;
                else
                    continue;
            }
            std::ifstream infile(szFileName);
            if (infile.is_open()) {
                std::string szTempText;
                std::string szText;
                while (std::getline(infile, szTempText)) {
                    szText += szTempText + '\n';
                }
                CreateNewTab(szFileName);
                gui::fileManager[gui::fileManager.size() - 1].m_szBuffer = cp1251_to_utf8(szText.c_str());
                infile.close();
            }
        }
        
    }
};