#pragma once
#include "BaseApp.h"

int CBaseApp::Run()
{
    MSG msg;
    HACCEL accel_table = 0;
    // HACCEL accel_table = LoadAccelerators(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_XXX));

    // Main message processing loop
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, accel_table, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}


