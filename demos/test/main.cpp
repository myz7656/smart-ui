#include <windows.h>

#include "base/task_center.hpp"
#include "maindlg.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    CMainDlg impl;
    impl.Parse(L"settings.xml");
    RECT rect = {100, 100, 703, 700};
    impl.Create(NULL, &rect);
    impl.ShowWindow(SW_SHOWNORMAL);
    impl.UpdateWindow();

    base::Singleton<base::TaskCenterUI>::Instance().Run();
    return 0;
}