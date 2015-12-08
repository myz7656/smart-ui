#include <windows.h>

#include "base/task_center.hpp"
#include "menudlg.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    CMenuDlg impl;
    impl.Parse(L"menu.xml");
    RECT rect = {100, 100, 280, 365};
    impl.Create(NULL, &rect);
    impl.ShowWindow(SW_SHOWNORMAL);
    impl.UpdateWindow();


    base::Singleton<base::TaskCenterUI>::Instance().Run();
    return 0;
}