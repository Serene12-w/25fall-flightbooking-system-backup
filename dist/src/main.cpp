#include "AirlineReservationSystem.h"
#include "PassengerUI.h"
#include <iostream>
#include <windows.h>

constexpr int ID_BTN_CHOICE_PASSENGER = 3001;
constexpr int ID_BTN_CHOICE_ADMIN = 3002;

LRESULT CALLBACK ChoiceWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        RECT rc{}; GetClientRect(hwnd, &rc);
        int cx = rc.right - rc.left;
        int titleW = 360, titleH = 24;
        int btnW = 260, btnH = 40;
        int titleX = (cx - titleW) / 2;
        int btnX = (cx - btnW) / 2;

        CreateWindowA("STATIC", "Airline Reservation System",
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            titleX, 20, titleW, titleH,
            hwnd, NULL, GetModuleHandle(NULL), NULL);

        CreateWindowA("BUTTON", "Passenger",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            btnX, 70, btnW, btnH,
            hwnd, (HMENU)ID_BTN_CHOICE_PASSENGER, GetModuleHandle(NULL), NULL);

        CreateWindowA("BUTTON", "Administrator (Console)",
            WS_VISIBLE | WS_CHILD,
            btnX, 130, btnW, btnH,
            hwnd, (HMENU)ID_BTN_CHOICE_ADMIN, GetModuleHandle(NULL), NULL);
        return 0;
    }
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == ID_BTN_CHOICE_PASSENGER) {
            // 启动 Passenger GUI（不触发 WM_QUIT）
            ShowWindow(hwnd, SW_HIDE); // 隐藏选择窗口但暂不销毁，防止提前退出消息循环
            {
                AirlineReservationSystem ars; // 在这里构造，避免在无控制台时期打印消息
                PassengerUI ui(&ars);
                ui.show();
            }
            // PassengerUI 关闭后，销毁选择窗口并退出
            DestroyWindow(hwnd);
            return 0;
        } else if (id == ID_BTN_CHOICE_ADMIN) {
            // 管理员模式：动态创建控制台供输出
            ShowWindow(hwnd, SW_HIDE);
            if (AllocConsole()) {
                FILE* fpOut; freopen_s(&fpOut, "CONOUT$", "w", stdout);
                FILE* fpIn; freopen_s(&fpIn, "CONIN$", "r", stdin);
                {
                    // 在控制台建立后再构造系统，这样 CSV 加载日志会显示在控制台
                    AirlineReservationSystem ars;
                    std::cout << "Starting Administrator Console System..." << std::endl;
                    ars.runAdminConsole();
                }
            }
            FreeConsole();
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    }
    case WM_DESTROY:
        // 不在这里 PostQuitMessage，因为选择窗口在被销毁以启动 PassengerUI 时会触发
        // 如果此窗口是程序真正退出的窗口，后续的窗口（例如 PassengerUI）会在其自己的 WM_DESTROY 中
        // 调用 PostQuitMessage 来结束应用程序消息循环。
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nShowCmd) {
    // 解析命令行：支持 -admin 直接进入控制台；支持 -chooser-topmost 置顶选择窗口
    std::string cmd(lpCmdLine ? lpCmdLine : "");
    if (!cmd.empty()) {
        if (cmd == "admin" || cmd == "-admin") {
            if (AllocConsole()) {
                FILE* fpOut; freopen_s(&fpOut, "CONOUT$", "w", stdout);
                FILE* fpIn; freopen_s(&fpIn, "CONIN$", "r", stdin);
                {
                    AirlineReservationSystem ars;
                    std::cout << "Starting Administrator Console System..." << std::endl;
                    ars.runAdminConsole();
                }
            }
            FreeConsole();
            return 0;
        }
    }

    const char CLASS_NAME[] = "AirlineReservationModeChooser";
    WNDCLASSA wc = {};
    wc.lpfnWndProc = ChoiceWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    // 计算居中位置
    int winW = 420;
    int winH = 240;
    int scrW = GetSystemMetrics(SM_CXSCREEN);
    int scrH = GetSystemMetrics(SM_CYSCREEN);
    int posX = (scrW - winW) / 2;
    int posY = (scrH - winH) / 2;

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    bool topMost = (!cmd.empty() && (cmd.find("-chooser-topmost") != std::string::npos));
    HWND hwnd = CreateWindowExA(topMost ? WS_EX_TOPMOST : 0, CLASS_NAME, "Select Mode",
        style,
        posX, posY, winW, winH,
        NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!hwnd) {
        std::cerr << "Failed to create chooser window." << std::endl;
        return 1;
    }

    if (topMost) {
        // 置顶后再确保居中（已在计算时居中，这里只是显示稳定性处理）
        SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, winW, winH, SWP_SHOWWINDOW);
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // 标准消息循环；当选择 Passenger 时，会在按钮处理里销毁窗口并进入 PassengerUI 的消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
