#include "PassengerUI.h"
#include "Order.h"
#include "Seat.h" // Added for SeatType usage in booking
#include <vector>
#include <commctrl.h>
#include <sstream>
#include <string>

#pragma comment(lib, "comctl32.lib")

// 全局实例指针，用于在窗口过程中访�?
static PassengerUI* g_pUI = nullptr;

PassengerUI::PassengerUI(AirlineReservationSystem* system) 
    : system(system), hwndMain(nullptr) {
    g_pUI = this;
}

PassengerUI::~PassengerUI() {
    if (hwndMain) {
        DestroyWindow(hwndMain);
    }

}

void PassengerUI::show() {
    // 初始�?Common Controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);
    
    if (createMainWindow()) {
        ShowWindow(hwndMain, SW_SHOW);
        UpdateWindow(hwndMain);
        
        // 消息循环
        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

bool PassengerUI::createMainWindow() {
    const char CLASS_NAME[] = "AirlineReservationPassengerWindow";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClass(&wc);
    
    hwndMain = CreateWindowEx(
        0,
        CLASS_NAME,
        "Airline Reservation System - Passenger",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 660,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        this
    );
    
    if (hwndMain == NULL) {
        return false;
    }
    
    // 居中 Passenger 主窗口
    {
        RECT rc; GetWindowRect(hwndMain, &rc);
        int winW = rc.right - rc.left; int winH = rc.bottom - rc.top;
        int scrW = GetSystemMetrics(SM_CXSCREEN); int scrH = GetSystemMetrics(SM_CYSCREEN);
        int posX = (scrW - winW) / 2; int posY = (scrH - winH) / 2;
        MoveWindow(hwndMain, posX, posY, winW, winH, FALSE);
    }


    // 显示登录界面
    showLoginScreen();
    
    return true;
}

void PassengerUI::showLoginScreen() {
    clearWindow();
    
    // 标题
    CreateWindow("STATIC", "Airline Reservation System",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        200, 50, 400, 40,
        hwndMain, NULL, NULL, NULL);
    
    // 护照号标�?
    CreateWindow("STATIC", "Passport Number:",
        WS_VISIBLE | WS_CHILD,
        200, 150, 150, 25,
        hwndMain, NULL, NULL, NULL);
    
    // 护照号输入框
    hwndPassportEdit = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        350, 150, 250, 25,
        hwndMain, (HMENU)ID_EDIT_PASSPORT, NULL, NULL);
    
    // 姓名标签
    CreateWindow("STATIC", "Name :",
        WS_VISIBLE | WS_CHILD,
        200, 200, 150, 25,
        hwndMain, NULL, NULL, NULL);
    
    // 姓名输入�?
    hwndNameEdit = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        350, 200, 250, 25,
        hwndMain, (HMENU)ID_EDIT_NAME, NULL, NULL);
    
    // 登录按钮
    hwndLoginButton = CreateWindow("BUTTON", "Login",
        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        250, 280, 120, 40,
        hwndMain, (HMENU)ID_BTN_LOGIN, NULL, NULL);
    
    // 注册按钮
    hwndRegisterButton = CreateWindow("BUTTON", "Register",
        WS_VISIBLE | WS_CHILD,
        400, 280, 120, 40,
        hwndMain, (HMENU)ID_BTN_REGISTER, NULL, NULL);

    // 返回选择界面按钮（放在登录/注册下面）
    CreateWindow("BUTTON", "Back to Selection",
        WS_VISIBLE | WS_CHILD,
        325, 330, 160, 32,
        hwndMain, (HMENU)ID_BTN_BACK_TO_SELECTION, NULL, NULL);
    // 统一三按钮为等宽居中纵向排列
    int btnW = 200, btnH = 40, gap = 10;
    int centerX = (800 - btnW) / 2;
    if (hwndLoginButton) SetWindowPos(hwndLoginButton, NULL, centerX, 280, btnW, btnH, SWP_NOZORDER);
    if (hwndRegisterButton) SetWindowPos(hwndRegisterButton, NULL, centerX, 330, btnW, btnH, SWP_NOZORDER);
    HWND hBackSel = GetDlgItem(hwndMain, ID_BTN_BACK_TO_SELECTION);
    if (hBackSel) SetWindowPos(hBackSel, NULL, centerX, 380, btnW, btnH, SWP_NOZORDER);
    // 已移除自定义字体，使用系统默认字体
}

void PassengerUI::showMainMenu() {
    clearWindow();
    
    // 标题
    CreateWindow("STATIC", "Welcome to Airline Reservation System",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        150, 30, 500, 30,
        hwndMain, NULL, NULL, NULL);
    
    int yPos = 100;
    int btnWidth = 250;
    int btnHeight = 45;
    int spacing = 15;
    int xPos = (800 - btnWidth) / 2;
    
    // 搜索航班按钮
    CreateWindow("BUTTON", "Search Flights",
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, btnWidth, btnHeight,
        hwndMain, (HMENU)ID_BTN_SEARCH, NULL, NULL);
    yPos += btnHeight + spacing;
    
    // 支付按钮
    CreateWindow("BUTTON", "Pay for Ticket",
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, btnWidth, btnHeight,
        hwndMain, (HMENU)ID_BTN_PAY, NULL, NULL);
    yPos += btnHeight + spacing;
    
    // 值机按钮
    CreateWindow("BUTTON", "Check In",
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, btnWidth, btnHeight,
        hwndMain, (HMENU)ID_BTN_CHECKIN, NULL, NULL);
    yPos += btnHeight + spacing;
    
    // 查看订单按钮
    CreateWindow("BUTTON", "View My Orders",
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, btnWidth, btnHeight,
        hwndMain, (HMENU)ID_BTN_VIEW_ORDERS, NULL, NULL);
    yPos += btnHeight + spacing;
    
    // 候补查看按钮（与纵向布局对齐）
    CreateWindow("BUTTON", "My Waitlist",
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, btnWidth, btnHeight,
        hwndMain, (HMENU)(ID_BTN_VIEW_ORDERS + 500), NULL, NULL);
    yPos += btnHeight + spacing;

    // 查看个人信息按钮
    CreateWindow("BUTTON", "My Profile",
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, btnWidth, btnHeight,
        hwndMain, (HMENU)ID_BTN_PROFILE, NULL, NULL);
    yPos += btnHeight + spacing;

    // 取消订单按钮
    CreateWindow("BUTTON", "Cancel Order",
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, btnWidth, btnHeight,
        hwndMain, (HMENU)ID_BTN_CANCEL, NULL, NULL);
    yPos += btnHeight + spacing;

    // 返回选择界面按钮（底部对齐，和其它按钮同宽居中）
    CreateWindow("BUTTON", "Back to Selection",
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, btnWidth, btnHeight,
        hwndMain, (HMENU)ID_BTN_BACK_TO_SELECTION, NULL, NULL);
    
    // 使用系统默认字体，无需额外处理
}

void PassengerUI::showSearchScreen() {
    clearWindow();
    
    // 标题
    CreateWindow("STATIC", "Search Flights",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        200, 30, 400, 30,
        hwndMain, NULL, NULL, NULL);
    
    // 出发�?
    CreateWindow("STATIC", "Origin (e.g., PEK):",
        WS_VISIBLE | WS_CHILD,
        100, 100, 150, 25,
        hwndMain, NULL, NULL, NULL);
    
    // 预填示例：常用城市三字码
    HWND hwndOrigin = CreateWindow("EDIT", "PEK",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        250, 100, 150, 25,
        hwndMain, (HMENU)ID_EDIT_ORIGIN, NULL, NULL);
    
    // 目的�?
    CreateWindow("STATIC", "Destination (e.g., LAX):",
        WS_VISIBLE | WS_CHILD,
        100, 140, 150, 25,
        hwndMain, NULL, NULL, NULL);
    
    // 预填示例：目标城市三字码
    HWND hwndDest = CreateWindow("EDIT", "LAX",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        250, 140, 150, 25,
        hwndMain, (HMENU)ID_EDIT_DEST, NULL, NULL);
    
    // 日期
    CreateWindow("STATIC", "Date (YYYY-MM-DD):",
        WS_VISIBLE | WS_CHILD,
        100, 180, 150, 25,
        hwndMain, NULL, NULL, NULL);
    
    // 预填示例：未来日期
    HWND hwndDate = CreateWindow("EDIT", "2025-11-15",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        250, 180, 150, 25,
        hwndMain, (HMENU)ID_EDIT_DATE, NULL, NULL);
    
    // 搜索按钮（单一入口，包含直飞与联程结果）
    CreateWindow("BUTTON", "Search",
        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        450, 120, 100, 40,
        hwndMain, (HMENU)ID_BTN_SEARCH, NULL, NULL);
    
    // 航班列表
    HWND hwndList = CreateWindow(WC_LISTVIEW, "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        50, 240, 700, 250,
        hwndMain, (HMENU)ID_LIST_FLIGHTS, NULL, NULL);
    // 增强可用性：整行选中 + 网格线
    ListView_SetExtendedListViewStyle(hwndList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    
    // 设置列表视图的列
    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    
    lvc.cx = 100;
    lvc.pszText = (LPSTR)"Flight No";
    ListView_InsertColumn(hwndList, 0, &lvc);
    
    lvc.cx = 100;
    lvc.pszText = (LPSTR)"Origin";
    ListView_InsertColumn(hwndList, 1, &lvc);
    
    lvc.cx = 100;
    lvc.pszText = (LPSTR)"Destination";
    ListView_InsertColumn(hwndList, 2, &lvc);
    
    lvc.cx = 80;
    lvc.pszText = (LPSTR)"Departure";
    ListView_InsertColumn(hwndList, 3, &lvc);
    
    lvc.cx = 80;
    lvc.pszText = (LPSTR)"Arrival";
    ListView_InsertColumn(hwndList, 4, &lvc);
    
    lvc.cx = 100;
    lvc.pszText = (LPSTR)"Date";
    ListView_InsertColumn(hwndList, 5, &lvc);
    
    lvc.cx = 90;
    lvc.pszText = (LPSTR)"Min Price";
    ListView_InsertColumn(hwndList, 6, &lvc);

    lvc.cx = 100;
    lvc.pszText = (LPSTR)"Cabins";
    ListView_InsertColumn(hwndList, 7, &lvc);
    
    // 预订与返回按钮：等宽对称
    CreateWindow("BUTTON", "Book Selected", WS_VISIBLE | WS_CHILD,
        300, 510, 200, 40, hwndMain, (HMENU)ID_BTN_BOOK, NULL, NULL);
    CreateWindow("BUTTON", "Back to Menu", WS_VISIBLE | WS_CHILD,
        550, 510, 150, 40, hwndMain, (HMENU)ID_BTN_BACK_TO_MENU, NULL, NULL);
    int w = 180, space = 40; int leftX = (800 - (2*w + space)) / 2;
    HWND hBook = GetDlgItem(hwndMain, ID_BTN_BOOK);
    HWND hBackMenu = GetDlgItem(hwndMain, ID_BTN_BACK_TO_MENU);
    if (hBook) MoveWindow(hBook, leftX, 510, w, 40, TRUE);
    if (hBackMenu) MoveWindow(hBackMenu, leftX + w + space, 510, w, 40, TRUE);
    // 使用系统默认字体
}

void PassengerUI::clearWindow() {
    // 销毁所有子窗口
    HWND hwndChild = GetWindow(hwndMain, GW_CHILD);
    while (hwndChild) {
        HWND hwndNext = GetWindow(hwndChild, GW_HWNDNEXT);
        DestroyWindow(hwndChild);
        hwndChild = hwndNext;
    }
}

std::string PassengerUI::getEditText(HWND hwndEdit) {
    int len = GetWindowTextLength(hwndEdit);
    if (len > 0) {
        char* buffer = new char[len + 1];
        GetWindowText(hwndEdit, buffer, len + 1);
        std::string result(buffer);
        delete[] buffer;
        return result;
    }
    return "";
}

void PassengerUI::setEditText(HWND hwndEdit, const std::string& text) {
    SetWindowText(hwndEdit, text.c_str());
}

void PassengerUI::showMessage(const std::string& message, const std::string& title) {
    MessageBox(hwndMain, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
}

// 已移除字体应用函数，保持系统默认外观

void PassengerUI::handleLogin() {
    std::string passport = getEditText(hwndPassportEdit);
    
    if (passport.empty()) {
        showMessage("Please enter your passport number!", "Error");
        return;
    }
    
    // 调用系统登录
    if (system->loginPassenger(passport)) {
        Passenger* passenger = system->getCurrentPassenger();
        std::string msg = "Welcome back, " + passenger->getName() + "!";
        showMessage(msg, "Login Success");
        showMainMenu();
    } else {
        showMessage("Passenger not found. Please register first.", "Login Failed");
    }
}

void PassengerUI::handleRegister() {
    std::string passport = getEditText(hwndPassportEdit);
    std::string name = getEditText(hwndNameEdit);
    
    if (passport.empty() || name.empty()) {
        showMessage("Please enter both passport number and name!", "Error");
        return;
    }
    
    // 调用系统注册
    if (system->registerPassenger(name, passport)) {
        std::string msg = "Registration successful! Welcome, " + name + "!";
        showMessage(msg, "Registration Success");
        showMainMenu();
    } else {
        showMessage("This passport number is already registered. Please login instead.", "Registration Failed");
    }
}

void PassengerUI::handleSearch() {
    // 获取搜索条件
    HWND hwndOrigin = GetDlgItem(hwndMain, ID_EDIT_ORIGIN);
    HWND hwndDest = GetDlgItem(hwndMain, ID_EDIT_DEST);
    HWND hwndDate = GetDlgItem(hwndMain, ID_EDIT_DATE);
    
    if (!hwndOrigin || !hwndDest || !hwndDate) {
        // 如果控件不存在，显示搜索界面
        showSearchScreen();
        return;
    }
    
    std::string origin = getEditText(hwndOrigin);
    std::string dest = getEditText(hwndDest);
    std::string date = getEditText(hwndDate);
    
    if (origin.empty() || dest.empty() || date.empty()) {
        showMessage("Please fill in all search fields!", "Error");
        return;
    }
    
    // 搜索航班（直飞 + 联程），并将结果合并显示在同一列表中
    std::vector<Flight*> flights = system->searchFlights(origin, dest, date);
    auto routes = system->searchConnectingFlightsUI(origin, dest, date);

    // 缓存以便后续 Book 操作引用
    lastSearchFlights_.clear();
    lastConnectingRoutes_.clear();
    lastSearchFlights_ = flights;
    lastConnectingRoutes_ = routes;

    // 显示结果（直飞条目先列出，随后列出联程路线）
    HWND hwndList = GetDlgItem(hwndMain, ID_LIST_FLIGHTS);
    if (hwndList) {
        ListView_DeleteAllItems(hwndList);

        if (flights.empty() && routes.empty()) {
            showMessage("No flights (direct or connecting) found for the selected route and date.", "Search Result");
            return;
        }

        int row = 0;
        // 直飞航班
        for (size_t i = 0; i < flights.size(); ++i) {
            Flight* flight = flights[i];

            LVITEM lvi = {};
            lvi.mask = LVIF_TEXT | LVIF_PARAM;
            lvi.iItem = row;
            lvi.lParam = (LPARAM)((int)i + 1); // 正数 => 直飞, index = lParam-1

            // 航班号
            lvi.pszText = const_cast<LPSTR>(flight->getFlightNumber().c_str());
            ListView_InsertItem(hwndList, &lvi);

            // 出发地
            ListView_SetItemText(hwndList, row, 1, const_cast<LPSTR>(flight->getOrigin().c_str()));

            // 目的地
            ListView_SetItemText(hwndList, row, 2, const_cast<LPSTR>(flight->getDestination().c_str()));

            // 出发时间
            ListView_SetItemText(hwndList, row, 3, const_cast<LPSTR>(flight->getDepartureTime().c_str()));

            // 到达时间
            ListView_SetItemText(hwndList, row, 4, const_cast<LPSTR>(flight->getArrivalTime().c_str()));

            // 日期
            ListView_SetItemText(hwndList, row, 5, const_cast<LPSTR>(flight->getFlightDate().c_str()));

            // 计算最低价格 (对该机型存在的各舱位取最小最终价；未登录显示 N/A)
            std::string minPriceStr = (system->getCurrentPassenger()) ? "" : "N/A";
            if (system->getCurrentPassenger()) {
                double minP = -1.0;
                Aircraft* ac = flight->getAircraft();
                if (ac) {
                    const SeatType types[4] = { SeatType::FIRST_CLASS, SeatType::BUSINESS_CLASS, SeatType::PREMIUM_ECONOMY, SeatType::ECONOMY_CLASS };
                    for (SeatType st : types) {
                        if (ac->hasCabinClass(st)) {
                            auto pb = system->getPriceBreakdown(flight->getFlightNumber(), flight->getFlightDate(), st);
                            if (pb.finalPrice >= 0 && (minP < 0 || pb.finalPrice < minP)) minP = pb.finalPrice;
                        }
                    }
                }
                if (minP >= 0) {
                    char priceBuf[64];
                    sprintf(priceBuf, "$%.2f", minP);
                    minPriceStr = priceBuf;
                } else {
                    minPriceStr = "-"; // 航班或价格不可用
                }
            }
            ListView_SetItemText(hwndList, row, 6, const_cast<LPSTR>(minPriceStr.c_str()));

            // 构建 Cabins 字符串 (按 F/B/P/E 顺序, 有则列出缩写; 无则留空)
            std::string cabinsStr;
            Aircraft* ac = flight->getAircraft();
            if (ac) {
                struct Cab { SeatType st; const char* tag; } cs[4] = {
                    {SeatType::FIRST_CLASS, "F"},
                    {SeatType::BUSINESS_CLASS, "B"},
                    {SeatType::PREMIUM_ECONOMY, "P"},
                    {SeatType::ECONOMY_CLASS, "E"}
                };
                for (auto &c : cs) {
                    if (ac->hasCabinClass(c.st)) {
                        if (!cabinsStr.empty()) cabinsStr += "/";
                        cabinsStr += c.tag;
                    }
                }
            }
            if (cabinsStr.empty()) cabinsStr = "-"; // 未配置
            ListView_SetItemText(hwndList, row, 7, const_cast<LPSTR>(cabinsStr.c_str()));

            row++;
        }

        // 联程路线：在列表中附加为行，标记为负 lParam
        for (size_t j = 0; j < routes.size(); ++j) {
            const auto& r = routes[j];

            LVITEM lvi = {};
            lvi.mask = LVIF_TEXT | LVIF_PARAM;
            lvi.iItem = row;
            lvi.lParam = (LPARAM)(-((int)j + 1)); // 负数 => 联程路线, index = -(lParam)-1

            // 将航班号列显示为合并的航班串，例如 "AA100 > BB200"
            std::string flightsConc;
            for (size_t k = 0; k < r.flights.size(); ++k) {
                if (k) flightsConc += " > ";
                flightsConc += r.flights[k]->getFlightNumber();
            }
            lvi.pszText = const_cast<LPSTR>(flightsConc.c_str());
            ListView_InsertItem(hwndList, &lvi);

            // 起始/结束/时间/日期等显示为首段/末段信息
            if (!r.flights.empty()) {
                Flight* ffirst = r.flights.front();
                Flight* flast = r.flights.back();
                ListView_SetItemText(hwndList, row, 1, const_cast<LPSTR>(ffirst->getOrigin().c_str()));
                ListView_SetItemText(hwndList, row, 2, const_cast<LPSTR>(flast->getDestination().c_str()));
                ListView_SetItemText(hwndList, row, 3, const_cast<LPSTR>(ffirst->getDepartureTime().c_str()));
                ListView_SetItemText(hwndList, row, 4, const_cast<LPSTR>(flast->getArrivalTime().c_str()));
                // 使用搜索日期作为路线日期展示
                ListView_SetItemText(hwndList, row, 5, const_cast<LPSTR>(date.c_str()));
            }

            // 对联程暂不显示价格/舱位聚合，留空或“-"
            ListView_SetItemText(hwndList, row, 6, const_cast<LPSTR>(std::string("-").c_str()));
            ListView_SetItemText(hwndList, row, 7, const_cast<LPSTR>(std::string("-").c_str()));

            row++;
        }

        showMessage("Found " + std::to_string(flights.size()) + " direct flight(s) and " + std::to_string(routes.size()) + " connecting route(s).", "Search Result");
    }
}

// 合并的转机搜索入口：现在直接复用普通搜索处理逻辑
void PassengerUI::handleConnectingSearch() {
    // 目前合并搜索逻辑：将转机搜索点击转发到统一的搜索处理函数
    // 若未来需要区分直飞/联程，可在此加入参数或分支。
    handleSearch();
}

// 合并的预订入口：将转机预订转发到统一的单程/预订处理
void PassengerUI::handleConnectingBook() {
    // 目前合并预订逻辑：使用与直飞相同的预订流程
    // 若未来需支持基于路线的联程创建，请在此添加参数或改为调用相应系统接口
    handleBook();
}

void PassengerUI::handleBook() {
    // 检查是否有登录
    if (!system->getCurrentPassenger()) {
        showMessage("Please login first!", "Error");
        showLoginScreen();
        return;
    }

    // 获取选中的航班
    HWND hwndList = GetDlgItem(hwndMain, ID_LIST_FLIGHTS);
    if (!hwndList) {
        showMessage("Please search for flights first!", "Error");
        showSearchScreen();
        return;
    }

    int selectedIndex = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
    if (selectedIndex == -1) {
        showMessage("Please select a flight to book!", "Error");
        return;
    }

    // 读取该行的 lParam（用于区分直飞/联程）
    LVITEM lvi = {};
    lvi.mask = LVIF_PARAM;
    lvi.iItem = selectedIndex;
    if (!ListView_GetItem(hwndList, &lvi)) {
        showMessage("Failed to read selected item metadata.", "Error");
        return;
    }
    LPARAM meta = lvi.lParam;

    // 单窗口舱位选择逻辑（复用）
    struct CabinOpt { const char* text; SeatType st; int id; } opts[4] = {
        {"First", SeatType::FIRST_CLASS, ID_BTN_CABIN_FIRST},
        {"Business", SeatType::BUSINESS_CLASS, ID_BTN_CABIN_BUSINESS},
        {"PremiumEco", SeatType::PREMIUM_ECONOMY, ID_BTN_CABIN_PREM},
        {"Economy", SeatType::ECONOMY_CLASS, ID_BTN_CABIN_ECO}
    };
    HWND cabinDlg = CreateWindowEx(WS_EX_DLGMODALFRAME, "AirlineReservationPassengerWindow", "Select Cabin",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        300, 220, 360, 220,
        hwndMain, NULL, GetModuleHandle(NULL), NULL);
    if (!cabinDlg) { showMessage("Cannot open cabin dialog.", "Error"); return; }
    cabinDlgWindow_ = cabinDlg; cabinPicked_ = false; cabinPickedTypeVal_ = (int)SeatType::ECONOMY_CLASS;
    CreateWindow("STATIC", "Choose cabin class:", WS_VISIBLE|WS_CHILD,
        20, 20, 200, 24, cabinDlg, NULL, GetModuleHandle(NULL), NULL);
    for (int i=0;i<4;++i) {
        CreateWindow("BUTTON", opts[i].text, WS_VISIBLE|WS_CHILD | BS_PUSHBUTTON,
            40 + (i%2)*140, 60 + (i/2)*60, 120, 40,
            cabinDlg, (HMENU)(INT_PTR)opts[i].id, GetModuleHandle(NULL), NULL);
    }
    ShowWindow(cabinDlg, SW_SHOW); UpdateWindow(cabinDlg);
    // Modal wait: WindowProc will set cabinPicked_ on button click
    MSG msg;
    while (IsWindow(cabinDlg) && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); DispatchMessage(&msg);
        if (!IsWindow(cabinDlg) || cabinPicked_) break;
    }
    if (!cabinPicked_) { showMessage("Booking cancelled.", "Info"); return; }
    SeatType seatType = (SeatType)cabinPickedTypeVal_;

    if (meta > 0) {
        // 直飞：meta = index+1
        int idx = (int)meta - 1;
        if (idx < 0 || idx >= (int)lastSearchFlights_.size()) { showMessage("Selected flight data stale. Please search again.", "Error"); return; }
        Flight* flight = lastSearchFlights_[idx];
        if (!flight) { showMessage("Flight not found.", "Error"); return; }

        std::string flightNumber = flight->getFlightNumber();
        std::string flightDate = flight->getFlightDate();

        // 精确价格预览（后端提供价格拆分）
        auto pb = system->getPriceBreakdown(flightNumber, flightDate, seatType);
        if (pb.basePrice < 0 || pb.finalPrice < 0) { showMessage("Unable to quote price for this flight.", "Error"); return; }
        char priceMsg[512];
        std::snprintf(priceMsg, sizeof(priceMsg),
            "Cabin: %s\nBase Price: $%.2f\nLoyalty Discount: -$%.2f (%.0f%%)\nFinal Price: $%.2f\n\nProceed with booking?",
            Order::seatTypeToString(seatType).c_str(), pb.basePrice, pb.discountAmount, pb.discountRate * 100.0, pb.finalPrice);
        int confirm = MessageBox(hwndMain, priceMsg, "Price Breakdown", MB_YESNO | MB_ICONINFORMATION);
        if (confirm != IDYES) { showMessage("Booking cancelled.", "Info"); return; }

        // 创建订单（若仓位满会返回空并加入候补）
        Order* order = system->createBooking(flightNumber, flightDate, seatType);
        if (order) {
            std::string orderMsg = "Booking successful!\nOrder Number: " + order->getOrderNumber() + 
                                  "\nTicket Price: $" + std::to_string(order->getTicketPrice()) +
                                  "\n\nPlease proceed to payment.";
            showMessage(orderMsg, "Booking Success");
            showMainMenu();
        } else {
            showMessage("Selected cabin is full. You have been added to the waitlist with priority based on your membership.", "Waitlist");
            showMainMenu();
        }
    } else if (meta < 0) {
        // 联程：meta = -(index+1)
        int ridx = -((int)meta) - 1;
        if (ridx < 0 || ridx >= (int)lastConnectingRoutes_.size()) { showMessage("Selected route data stale. Please search again.", "Error"); return; }
        const auto& route = lastConnectingRoutes_[ridx];

        // 直接创建联程订单
        Order* order = system->createConnectingBookingFromRoute(route, seatType);
        if (!order) { showMessage("Unable to create connecting booking (capacity or cabin mismatch).", "Error"); return; }
        std::string successMsg = "Connecting booking success!\nOrder: " + order->getOrderNumber() + "\nSegments: " + std::to_string(route.flights.size()) + "\nCabin: " + Order::seatTypeToString(seatType) + "\nPrice: $" + std::to_string(order->getTicketPrice()) + "\nStatus: Pending Payment";
        showMessage(successMsg, "Booking Success");
        showMainMenu();
    } else {
        showMessage("Invalid selection metadata. Please search again.", "Error");
    }
}

void PassengerUI::handleCheckIn() {
    if (!system->getCurrentPassenger()) {
        showMessage("Please login first!", "Error");
        showLoginScreen();
        return;
    }
    
    clearWindow();
    
    CreateWindow("STATIC", "Check In",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        200, 50, 400, 30,
        hwndMain, NULL, NULL, NULL);
    
    // 可值机订单列表（已付款、未值机）
    CreateWindow("STATIC", "Select a PAID order to check-in:",
        WS_VISIBLE | WS_CHILD,
        50, 90, 300, 25,
        hwndMain, NULL, NULL, NULL);

    HWND hwndList = CreateWindow(WC_LISTVIEW, "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        50, 120, 680, 200,
        hwndMain, (HMENU)ID_LIST_ORDERS, NULL, NULL);

    {
        LVCOLUMN lvc = {};
        lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 160; lvc.pszText = (LPSTR)"Order Number"; ListView_InsertColumn(hwndList, 0, &lvc);
    lvc.cx = 110; lvc.pszText = (LPSTR)"Flight";        ListView_InsertColumn(hwndList, 1, &lvc);
    lvc.cx = 110; lvc.pszText = (LPSTR)"Date";          ListView_InsertColumn(hwndList, 2, &lvc);
    lvc.cx = 120; lvc.pszText = (LPSTR)"Seat";          ListView_InsertColumn(hwndList, 3, &lvc);
    lvc.cx = 90;  lvc.pszText = (LPSTR)"Type";          ListView_InsertColumn(hwndList, 4, &lvc);
    lvc.cx = 140; lvc.pszText = (LPSTR)"Status";        ListView_InsertColumn(hwndList, 5, &lvc);

        // 填充订单
        std::vector<Order*> orders = system->getPassengerOrders(system->getCurrentPassenger()->getPassportNumber());
        int idx = 0;
        for (auto* o : orders) {
            if (o->getStatus() == OrderStatus::PAID) {
                LVITEM it = {}; it.mask = LVIF_TEXT; it.iItem = idx; it.pszText = const_cast<LPSTR>(o->getOrderNumber().c_str());
                ListView_InsertItem(hwndList, &it);
                if (o->isConnectingFlight()) {
                    auto nums = o->getConnectingFlightNumbers();
                    std::string main = nums.empty()? o->getFlightNumber() : nums[0];
                    ListView_SetItemText(hwndList, idx, 1, const_cast<LPSTR>(main.c_str()));
                } else {
                    ListView_SetItemText(hwndList, idx, 1, const_cast<LPSTR>(o->getFlightNumber().c_str()));
                }
                ListView_SetItemText(hwndList, idx, 2, const_cast<LPSTR>(o->getFlightDate().c_str()));
                if (o->isConnectingFlight()) {
                    auto seats = o->getConnectingSeatNumbers();
                    std::string seatDisplay = (!seats.empty() && !seats[0].empty()) ? seats[0] + "..." : std::string("");
                    ListView_SetItemText(hwndList, idx, 3, const_cast<LPSTR>(seatDisplay.c_str()));
                    std::string typeStr = "Connecting"; ListView_SetItemText(hwndList, idx, 4, const_cast<LPSTR>(typeStr.c_str()));
                } else {
                    ListView_SetItemText(hwndList, idx, 3, const_cast<LPSTR>(o->getSeatNumber().c_str()));
                    std::string typeStr = "Direct"; ListView_SetItemText(hwndList, idx, 4, const_cast<LPSTR>(typeStr.c_str()));
                }
                std::string st = "Paid"; ListView_SetItemText(hwndList, idx, 5, const_cast<LPSTR>(st.c_str()));
                idx++;
            }
        }
    }
    
    CreateWindow("STATIC", "Seat Number(s) :",
            WS_VISIBLE | WS_CHILD,
            200, 340, 150, 25,
        hwndMain, NULL, NULL, NULL);
    
    HWND hwndSeatEdit = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        350, 340, 250, 25,
        hwndMain, (HMENU)ID_EDIT_DEST, NULL, NULL);
    // 统一的座位图按钮：对直飞打开座位图，对联程打开分段座位对话
    CreateWindow("BUTTON", "Seat Map",
        WS_VISIBLE | WS_CHILD,
        610, 340, 100, 25,
        hwndMain, (HMENU)ID_BTN_OPEN_MAP, NULL, NULL);
    
    CreateWindow("BUTTON", "Check In Now",
        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        300, 380, 120, 40,
        hwndMain, (HMENU)(ID_BTN_CHECKIN + 1000), NULL, NULL);
    CreateWindow("BUTTON", "Back",
        WS_VISIBLE | WS_CHILD,
        450, 380, 120, 40,
        hwndMain, (HMENU)ID_BTN_BACK_TO_MENU, NULL, NULL);
    // 对称排列两个按钮
    {
        int w = 140, gap = 40, left = (800 - (2*w + gap)) / 2;
        HWND hDo = GetDlgItem(hwndMain, ID_BTN_CHECKIN + 1000);
        HWND hBack = GetDlgItem(hwndMain, ID_BTN_BACK_TO_MENU);
        if (hDo) MoveWindow(hDo, left, 380, w, 40, TRUE);
        if (hBack) MoveWindow(hBack, left + w + gap, 380, w, 40, TRUE);
    }
}

void PassengerUI::handleViewOrders() {
    if (!system->getCurrentPassenger()) {
        showMessage("Please login first!", "Error");
        showLoginScreen();
        return;
    }
    
    clearWindow();
    
    // 标题
    CreateWindow("STATIC", "My Orders",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        200, 30, 400, 30,
        hwndMain, NULL, NULL, NULL);
    
    // 订单列表
    HWND hwndList = CreateWindow(WC_LISTVIEW, "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        50, 80, 700, 350,
        hwndMain, (HMENU)ID_LIST_ORDERS, NULL, NULL);
    
    // 设置�?
    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    
    lvc.cx = 120;
    lvc.pszText = (LPSTR)"Order Number";
    ListView_InsertColumn(hwndList, 0, &lvc);
    
    lvc.cx = 100;
    lvc.pszText = (LPSTR)"Flight";
    ListView_InsertColumn(hwndList, 1, &lvc);
    
    lvc.cx = 80;
    lvc.pszText = (LPSTR)"Date";
    ListView_InsertColumn(hwndList, 2, &lvc);
    
    lvc.cx = 80;
    lvc.pszText = (LPSTR)"Seat";
    ListView_InsertColumn(hwndList, 3, &lvc);
    
    lvc.cx = 80;
    lvc.pszText = (LPSTR)"Price";
    ListView_InsertColumn(hwndList, 4, &lvc);
    
    lvc.cx = 100;
    lvc.pszText = (LPSTR)"Status";
    ListView_InsertColumn(hwndList, 5, &lvc);

    lvc.cx = 130;
    lvc.pszText = (LPSTR)"Type"; // CONNECTING or DIRECT
    ListView_InsertColumn(hwndList, 6, &lvc);
    
    // 获取订单
    std::vector<Order*> orders = system->getPassengerOrders(
        system->getCurrentPassenger()->getPassportNumber()
    );
    
    // 显示订单
    for (size_t i = 0; i < orders.size(); ++i) {
        Order* order = orders[i];
        
        LVITEM lvi = {};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = i;
        
    // 订单号
        lvi.pszText = const_cast<LPSTR>(order->getOrderNumber().c_str());
        ListView_InsertItem(hwndList, &lvi);
        
        // 航班或主航段
        if (order->isConnectingFlight()) {
            std::string mainFlight = order->getConnectingFlightNumbers().empty() ? order->getFlightNumber() : order->getConnectingFlightNumbers()[0];
            ListView_SetItemText(hwndList, i, 1, const_cast<LPSTR>(mainFlight.c_str()));
        } else {
            ListView_SetItemText(hwndList, i, 1, const_cast<LPSTR>(order->getFlightNumber().c_str()));
        }
        
        // 日期
        ListView_SetItemText(hwndList, i, 2, const_cast<LPSTR>(order->getFlightDate().c_str()));
        
        // 座位号（若联程显示第一段或“Multi”）
        if (order->isConnectingFlight()) {
            std::string seatDisplay;
            auto seats = order->getConnectingSeatNumbers();
            if (!seats.empty() && !seats[0].empty()) seatDisplay = seats[0] + "..."; else seatDisplay = "Multi";
            ListView_SetItemText(hwndList, i, 3, const_cast<LPSTR>(seatDisplay.c_str()));
        } else {
            ListView_SetItemText(hwndList, i, 3, const_cast<LPSTR>(order->getSeatNumber().c_str()));
        }
        
        // 价格
        std::string priceStr = std::to_string(order->getTicketPrice());
        ListView_SetItemText(hwndList, i, 4, const_cast<LPSTR>(priceStr.c_str()));
        
        // 状态
        std::string statusStr;
        switch (order->getStatus()) {
            case OrderStatus::PENDING_PAYMENT: statusStr = "Pending Payment"; break;
            case OrderStatus::PAID: statusStr = "Paid"; break;
            case OrderStatus::CHECKED_IN: statusStr = "Checked In"; break;
            case OrderStatus::CANCELLED: statusStr = "Cancelled"; break;
        }
        ListView_SetItemText(hwndList, i, 5, const_cast<LPSTR>(statusStr.c_str()));

        // 类型
        std::string typeStr = order->isConnectingFlight() ? "Connecting" : "Direct";
        ListView_SetItemText(hwndList, i, 6, const_cast<LPSTR>(typeStr.c_str()));
    }
    
    // 底部操作按钮（等宽对称居中）
    CreateWindow("BUTTON", "View Details",
        WS_VISIBLE | WS_CHILD,
        200, 450, 160, 40,
        hwndMain, (HMENU)ID_BTN_VIEW_DETAILS, NULL, NULL);

    CreateWindow("BUTTON", "Back to Menu",
        WS_VISIBLE | WS_CHILD,
        400, 450, 160, 40,
        hwndMain, (HMENU)ID_BTN_BACK_TO_MENU, NULL, NULL);

    {
        int w = 160, space = 30; int leftX = (800 - (2*w + space)) / 2; int y = 450;
        HWND h1 = GetDlgItem(hwndMain, ID_BTN_VIEW_DETAILS);
        HWND h2 = GetDlgItem(hwndMain, ID_BTN_BACK_TO_MENU);
        if (h1) MoveWindow(h1, leftX, y, w, 40, TRUE);
        if (h2) MoveWindow(h2, leftX + w + space, y, w, 40, TRUE);
    }
    
    if (orders.empty()) {
        showMessage("You have no orders yet.", "Info");
    }
}

void PassengerUI::handlePay() {
    if (!system->getCurrentPassenger()) {
        showMessage("Please login first!", "Error");
        showLoginScreen();
        return;
    }
    
    clearWindow();
    
    CreateWindow("STATIC", "Pay for Ticket",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        200, 50, 400, 30,
        hwndMain, NULL, NULL, NULL);
    
    CreateWindow("STATIC", "Pending payment orders:",
        WS_VISIBLE | WS_CHILD,
        50, 100, 220, 25,
        hwndMain, NULL, NULL, NULL);

    // 待支付订单列表
    HWND hwndList = CreateWindow(WC_LISTVIEW, "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        50, 130, 680, 200,
        hwndMain, (HMENU)ID_LIST_ORDERS, NULL, NULL);

    {
        LVCOLUMN lvc = {};
        lvc.mask = LVCF_TEXT | LVCF_WIDTH;
        lvc.cx = 160; lvc.pszText = (LPSTR)"Order Number"; ListView_InsertColumn(hwndList, 0, &lvc);
        lvc.cx = 110; lvc.pszText = (LPSTR)"Flight";        ListView_InsertColumn(hwndList, 1, &lvc);
        lvc.cx = 110; lvc.pszText = (LPSTR)"Date";          ListView_InsertColumn(hwndList, 2, &lvc);
        lvc.cx = 110; lvc.pszText = (LPSTR)"Price";         ListView_InsertColumn(hwndList, 3, &lvc);
        lvc.cx = 90;  lvc.pszText = (LPSTR)"Type";          ListView_InsertColumn(hwndList, 4, &lvc);
        lvc.cx = 120; lvc.pszText = (LPSTR)"Status";        ListView_InsertColumn(hwndList, 5, &lvc);

        // 填充订单
        std::vector<Order*> orders = system->getPassengerOrders(system->getCurrentPassenger()->getPassportNumber());
        int idx = 0;
        for (auto* o : orders) {
            if (o->getStatus() == OrderStatus::PENDING_PAYMENT) {
                LVITEM it = {}; it.mask = LVIF_TEXT; it.iItem = idx; it.pszText = const_cast<LPSTR>(o->getOrderNumber().c_str());
                ListView_InsertItem(hwndList, &it);
                if (o->isConnectingFlight()) {
                    auto nums = o->getConnectingFlightNumbers();
                    std::string main = nums.empty()? o->getFlightNumber() : nums[0];
                    ListView_SetItemText(hwndList, idx, 1, const_cast<LPSTR>(main.c_str()));
                } else {
                    ListView_SetItemText(hwndList, idx, 1, const_cast<LPSTR>(o->getFlightNumber().c_str()));
                }
                ListView_SetItemText(hwndList, idx, 2, const_cast<LPSTR>(o->getFlightDate().c_str()));
                std::string priceStr = std::to_string(o->getTicketPrice());
                ListView_SetItemText(hwndList, idx, 3, const_cast<LPSTR>(priceStr.c_str()));
                std::string typeStr = o->isConnectingFlight()? "Connecting" : "Direct";
                ListView_SetItemText(hwndList, idx, 4, const_cast<LPSTR>(typeStr.c_str()));
                std::string st = "Pending Payment"; ListView_SetItemText(hwndList, idx, 5, const_cast<LPSTR>(st.c_str()));
                idx++;
            }
        }
    }

    // 信用卡输入：标签+输入框整体水平居中
    HWND hLblCard = CreateWindow("STATIC", "Credit Card (xxxx):",
        WS_VISIBLE | WS_CHILD,
        200, 350, 150, 25,
        hwndMain, NULL, NULL, NULL);
    
    HWND hwndCardEdit = CreateWindow("EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        350, 350, 250, 25,
        hwndMain, (HMENU)ID_EDIT_DEST, NULL, NULL);

    {
        int labelW = 150, editW = 250, gap = 10; int total = labelW + gap + editW;
        int leftX = (800 - total) / 2; int y = 350;
        if (hLblCard) MoveWindow(hLblCard, leftX, y, labelW, 25, TRUE);
        if (hwndCardEdit) MoveWindow(hwndCardEdit, leftX + labelW + gap, y, editW, 25, TRUE);
    }
    
    // 底部双按钮：等宽对称居中
    HWND hBtnPay = CreateWindow("BUTTON", "Pay Selected",
        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        300, 390, 160, 40,
        hwndMain, (HMENU)(ID_BTN_PAY + 1000), NULL, NULL);
    
    HWND hBtnBack = CreateWindow("BUTTON", "Back",
        WS_VISIBLE | WS_CHILD,
        480, 390, 160, 40,
        hwndMain, (HMENU)ID_BTN_BACK_TO_MENU, NULL, NULL);

    {
        int w = 160, space = 30; int leftX = (800 - (2*w + space)) / 2; int y = 390;
        if (hBtnPay) MoveWindow(hBtnPay, leftX, y, w, 40, TRUE);
        if (hBtnBack) MoveWindow(hBtnBack, leftX + w + space, y, w, 40, TRUE);
    }
}

void PassengerUI::handleProfile() {
    if (!system->getCurrentPassenger()) {
        showMessage("Please login first!", "Error");
        showLoginScreen();
        return;
    }
    
    Passenger* p = system->getCurrentPassenger();
    
    // 基本资料
    std::string info = "Passenger Profile\n\n";
    info += "Name: " + p->getName() + "\n";
    info += "Passport: " + p->getPassportNumber() + "\n";
    
    // 会员等级（与控制台版本一致）
    std::string tierName;
    switch (p->getLoyaltyStatus()) {
        case LoyaltyStatus::PLATINUM: tierName = "PLATINUM"; break;
        case LoyaltyStatus::GOLD: tierName = "GOLD"; break;
        case LoyaltyStatus::SILVER: tierName = "SILVER"; break;
        case LoyaltyStatus::NONE: default: tierName = "REGULAR"; break;
    }
    info += "Membership Tier: " + tierName + "\n";

    // 会员权益（摘自 AirlineReservationSystem::viewPassengerProfile）
    info += "\nMember Benefits:\n";
    if (p->getLoyaltyStatus() == LoyaltyStatus::PLATINUM) {
        info += "- Ticket Discount: 20% OFF\n";
        info += "- Priority Check-in\n";
        info += "- Priority Boarding\n";
        info += "- Lounge Access (Premium)\n";
        info += "- Extra Baggage Allowance (40kg)\n";
        info += "- Free Seat Selection\n";
        info += "- Fast Track Security\n";
        info += "- Complimentary Upgrades (Subject to availability)\n";
        info += "- 24/7 Dedicated Customer Service\n";
        info += "- Flexible Booking Changes\n";
    } else if (p->getLoyaltyStatus() == LoyaltyStatus::GOLD) {
        info += "- Ticket Discount: 15% OFF\n";
        info += "- Priority Check-in\n";
        info += "- Priority Boarding\n";
        info += "- Lounge Access (Standard)\n";
        info += "- Extra Baggage Allowance (30kg)\n";
        info += "- Free Seat Selection\n";
        info += "- Fast Track Security\n";
        info += "- Flexible Booking Changes\n";
    } else if (p->getLoyaltyStatus() == LoyaltyStatus::SILVER) {
        info += "- Ticket Discount: 10% OFF\n";
        info += "- Priority Check-in\n";
        info += "- Priority Boarding\n";
        info += "- Extra Baggage Allowance (25kg)\n";
        info += "- Free Seat Selection\n";
    } else {
        info += "- Standard Baggage Allowance (20kg)\n";
        info += "\nUpgrade your membership to enjoy more benefits!\n";
    }

    // 订单统计（来自 CSV 的 orders.csv）
    auto orders = system->getPassengerOrders(p->getPassportNumber());
    int completed = 0;
    for (auto* o : orders) {
        if (o->getStatus() == OrderStatus::CHECKED_IN) completed++;
    }
    info += "\nTotal Orders: " + std::to_string(orders.size()) + "\n";
    info += "Completed Flights: " + std::to_string(completed) + "\n";

    showMessage(info, "My Profile");
}

void PassengerUI::handleWaitlistView() {
    if (!system->getCurrentPassenger()) { showMessage("Please login first!", "Error"); showLoginScreen(); return; }
    clearWindow();
    CreateWindow("STATIC", "My Waitlist Entries",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        200, 40, 400, 30,
        hwndMain, NULL, NULL, NULL);

    HWND hwndList = CreateWindow(WC_LISTVIEW, "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        50, 90, 700, 320,
        hwndMain, (HMENU)ID_LIST_ORDERS, NULL, NULL);

    LVCOLUMN lvc = {}; lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 120; lvc.pszText = (LPSTR)"Flight"; ListView_InsertColumn(hwndList, 0, &lvc);
    lvc.cx = 100; lvc.pszText = (LPSTR)"Date"; ListView_InsertColumn(hwndList, 1, &lvc);
    lvc.cx = 140; lvc.pszText = (LPSTR)"Cabin"; ListView_InsertColumn(hwndList, 2, &lvc);
    lvc.cx = 100; lvc.pszText = (LPSTR)"Priority"; ListView_InsertColumn(hwndList, 3, &lvc);
    lvc.cx = 160; lvc.pszText = (LPSTR)"Loyalty"; ListView_InsertColumn(hwndList, 4, &lvc);

    auto entries = system->getPassengerWaitlist(system->getCurrentPassenger()->getPassportNumber());
    // 若为空，自动注入2条演示候补记录（选取系统前两个航班，经济舱）
    if (entries.empty()) {
        auto all = system->getAllFlights();
        int seeded = 0;
        for (auto* f : all) {
            if (!f) continue;
            if (system->addToWaitlistForCurrent(f->getFlightNumber(), f->getFlightDate(), SeatType::ECONOMY_CLASS)) {
                seeded++;
                if (seeded >= 2) break;
            }
        }
        if (seeded > 0) {
            entries = system->getPassengerWaitlist(system->getCurrentPassenger()->getPassportNumber());
        }
    }
    int idx = 0;
    for (auto &e : entries) {
        LVITEM it = {}; it.mask = LVIF_TEXT; it.iItem = idx; it.pszText = const_cast<LPSTR>(e.flight->getFlightNumber().c_str());
        ListView_InsertItem(hwndList, &it);
        ListView_SetItemText(hwndList, idx, 1, const_cast<LPSTR>(e.flight->getFlightDate().c_str()));
        std::string cabin = Order::seatTypeToString(e.seatType); ListView_SetItemText(hwndList, idx, 2, const_cast<LPSTR>(cabin.c_str()));
        std::string pr = std::to_string(e.priority); ListView_SetItemText(hwndList, idx, 3, const_cast<LPSTR>(pr.c_str()));
        std::string loyalty;
        switch (e.passenger->getLoyaltyStatus()) {
            case LoyaltyStatus::PLATINUM: loyalty = "PLATINUM"; break;
            case LoyaltyStatus::GOLD: loyalty = "GOLD"; break;
            case LoyaltyStatus::SILVER: loyalty = "SILVER"; break;
            case LoyaltyStatus::NONE: loyalty = "NONE"; break;
        }
        ListView_SetItemText(hwndList, idx, 4, const_cast<LPSTR>(loyalty.c_str()));
        idx++;
    }

    if (entries.empty()) {
        showMessage("You have no active waitlist entries.", "Info");
    }

    HWND hBtnRemove = CreateWindow("BUTTON", "Remove Selected",
        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        200, 430, 160, 40,
        hwndMain, (HMENU)(ID_BTN_VIEW_ORDERS + 501), NULL, NULL);

    HWND hBtnBackWL = CreateWindow("BUTTON", "Back",
        WS_VISIBLE | WS_CHILD,
        400, 430, 160, 40,
        hwndMain, (HMENU)ID_BTN_BACK_TO_MENU, NULL, NULL);

    {
        int w = 160, space = 30; int leftX = (800 - (2*w + space)) / 2; int y = 430;
        if (hBtnRemove) MoveWindow(hBtnRemove, leftX, y, w, 40, TRUE);
        if (hBtnBackWL) MoveWindow(hBtnBackWL, leftX + w + space, y, w, 40, TRUE);
    }
}

void PassengerUI::handleCancel() {
    if (!system->getCurrentPassenger()) {
        showMessage("Please login first!", "Error");
        showLoginScreen();
        return;
    }
    
    clearWindow();
    
    CreateWindow("STATIC", "Cancel Order",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        200, 50, 400, 30,
        hwndMain, NULL, NULL, NULL);
    
    CreateWindow("STATIC", "Select an order to cancel:",
        WS_VISIBLE | WS_CHILD,
        50, 100, 250, 25,
        hwndMain, NULL, NULL, NULL);

    // 可取消订单列表（仅允许未付款 Pending Payment）
    HWND hwndList = CreateWindow(WC_LISTVIEW, "",
        WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL,
        50, 130, 680, 200,
        hwndMain, (HMENU)ID_LIST_ORDERS, NULL, NULL);

    {
    LVCOLUMN lvc = {};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 160; lvc.pszText = (LPSTR)"Order Number"; ListView_InsertColumn(hwndList, 0, &lvc);
    lvc.cx = 110; lvc.pszText = (LPSTR)"Flight";        ListView_InsertColumn(hwndList, 1, &lvc);
    lvc.cx = 110; lvc.pszText = (LPSTR)"Date";          ListView_InsertColumn(hwndList, 2, &lvc);
    lvc.cx = 90;  lvc.pszText = (LPSTR)"Type";          ListView_InsertColumn(hwndList, 3, &lvc);
    lvc.cx = 140; lvc.pszText = (LPSTR)"Status";        ListView_InsertColumn(hwndList, 4, &lvc);

        std::vector<Order*> orders = system->getPassengerOrders(system->getCurrentPassenger()->getPassportNumber());
        int idx = 0;
        for (auto* o : orders) {
            if (o->getStatus() == OrderStatus::PENDING_PAYMENT) {
                LVITEM it = {}; it.mask = LVIF_TEXT; it.iItem = idx; it.pszText = const_cast<LPSTR>(o->getOrderNumber().c_str());
                ListView_InsertItem(hwndList, &it);
                if (o->isConnectingFlight()) {
                    auto nums = o->getConnectingFlightNumbers();
                    std::string main = nums.empty() ? o->getFlightNumber() : nums[0];
                    ListView_SetItemText(hwndList, idx, 1, const_cast<LPSTR>(main.c_str()));
                    ListView_SetItemText(hwndList, idx, 2, const_cast<LPSTR>(o->getFlightDate().c_str()));
                    std::string typeStr = "Connecting";
                    ListView_SetItemText(hwndList, idx, 3, const_cast<LPSTR>(typeStr.c_str()));
                } else {
                    ListView_SetItemText(hwndList, idx, 1, const_cast<LPSTR>(o->getFlightNumber().c_str()));
                    ListView_SetItemText(hwndList, idx, 2, const_cast<LPSTR>(o->getFlightDate().c_str()));
                    std::string typeStr = "Direct";
                    ListView_SetItemText(hwndList, idx, 3, const_cast<LPSTR>(typeStr.c_str()));
                }
                std::string st = "Pending Payment";
                ListView_SetItemText(hwndList, idx, 4, const_cast<LPSTR>(st.c_str()));
                idx++;
            }
        }
    }

    HWND hCancel = CreateWindow("BUTTON", "Cancel Selected",
        WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        300, 360, 150, 40,
        hwndMain, (HMENU)(ID_BTN_CANCEL + 1000), NULL, NULL);
    
    HWND hDetails = CreateWindow("BUTTON", "View Details",
        WS_VISIBLE | WS_CHILD,
        140, 360, 150, 40,
        hwndMain, (HMENU)ID_BTN_VIEW_DETAILS, NULL, NULL);
    
    HWND hBackCancel = CreateWindow("BUTTON", "Back",
        WS_VISIBLE | WS_CHILD,
        460, 360, 150, 40,
        hwndMain, (HMENU)ID_BTN_BACK_TO_MENU, NULL, NULL);

    {
        // 三按钮等宽 150, 间距 30 居中
        int w = 150, space = 30; int total = 3*w + 2*space; int leftX = (800 - total) / 2; int y = 360;
        if (hDetails) MoveWindow(hDetails, leftX, y, w, 40, TRUE);
        if (hCancel) MoveWindow(hCancel, leftX + w + space, y, w, 40, TRUE);
        if (hBackCancel) MoveWindow(hBackCancel, leftX + 2*(w + space), y, w, 40, TRUE);
    }
}

// 备用：预订界面（当前统一使用 Search 界面进行选航班与下单）
void PassengerUI::showBookingScreen() {
    // 直接复用搜索界面
    showSearchScreen();
}

// 备用：订单展示界面（统一由 handleViewOrders 构建控件）
void PassengerUI::showOrdersScreen() {
    handleViewOrders();
}

// 座位图选择窗口：按指定舱位显示可选座位，点击即回填目标编辑框
void PassengerUI::openSeatMapForFlight(const std::string& flightNo, const std::string& flightDate, SeatType desiredType, HWND targetEdit) {
    if (!system) { showMessage("System not available.", "Error"); return; }
    Flight* f = system->getFlightByNumber(flightNo);
    if (!f || f->getFlightDate() != flightDate) { showMessage("Flight not found for the selected date.", "Error"); return; }
    Aircraft* ac = f->getAircraft(); if (!ac) { showMessage("Aircraft not available.", "Error"); return; }

    // 若该机型不存在所选舱位，尝试降级（在 UI 侧简单处理）
    SeatType cabin = desiredType;
    auto hasClass = [&](SeatType t){ return ac->hasCabinClass(t); };
    if (!hasClass(cabin)) {
        if (cabin == SeatType::FIRST_CLASS)      cabin = hasClass(SeatType::BUSINESS_CLASS) ? SeatType::BUSINESS_CLASS : (hasClass(SeatType::PREMIUM_ECONOMY) ? SeatType::PREMIUM_ECONOMY : SeatType::ECONOMY_CLASS);
        else if (cabin == SeatType::BUSINESS_CLASS) cabin = hasClass(SeatType::PREMIUM_ECONOMY) ? SeatType::PREMIUM_ECONOMY : SeatType::ECONOMY_CLASS;
        else if (cabin == SeatType::PREMIUM_ECONOMY) cabin = SeatType::ECONOMY_CLASS;
    }

    // 行范围根据舱位划分
    int fc = ac->getFirstClassRows();
    int bc = ac->getBusinessClassRows();
    int pe = ac->getPremiumEconomyRows();
    int ec = ac->getEconomyClassRows();
    int startRow = 1, endRow = 0;
    switch (cabin) {
        case SeatType::FIRST_CLASS:        startRow = 1;                   endRow = fc; break;
        case SeatType::BUSINESS_CLASS:     startRow = fc + 1;              endRow = fc + bc; break;
        case SeatType::PREMIUM_ECONOMY:    startRow = fc + bc + 1;         endRow = fc + bc + pe; break;
        case SeatType::ECONOMY_CLASS:      startRow = fc + bc + pe + 1;    endRow = fc + bc + pe + ec; break;
    }
    if (endRow < startRow) { showMessage("Selected cabin not available on this aircraft.", "Info"); return; }

    // 创建座位图窗口
    if (seatMapWindow_) { DestroyWindow(seatMapWindow_); seatMapWindow_ = nullptr; }
    seatMapTargetEdit_ = targetEdit;
    seatMapFlightNo_ = flightNo;
    seatMapFlightDate_ = flightDate;

    const int colCount = ac->getSeatsPerRow();
    int rowsToShow = endRow - startRow + 1;
    int btnW = 32, btnH = 26, pad = 6;
    int contentW = 40 + colCount * (btnW + pad);
    int contentH = 80 + rowsToShow * (btnH + pad);
    int maxH = 500; // 限制窗口高度，超出则启用滚动
    int winW = contentW;
    int winH = contentH > maxH ? maxH : contentH;

    // 使用主窗体同类窗口过程，便于在同一 WindowProc 中处理按钮事件
    seatMapWindow_ = CreateWindowEx(WS_EX_DLGMODALFRAME, "AirlineReservationPassengerWindow", "Seat Map",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        200, 120, winW, winH,
        hwndMain, NULL, GetModuleHandle(NULL), NULL);
    if (!seatMapWindow_) { showMessage("Cannot open seat map window.", "Error"); return; }

    std::string title = std::string("Seat Map ") + flightNo + " (" + f->getAircraft()->getType() + ") - " + Order::seatTypeToString(cabin);
    SetWindowText(seatMapWindow_, title.c_str());
    seatMapButtons_.clear();
    seatMapBtnBaseY_.clear();
    // 视口可用于滚动的内容高度（底部预留约 40px 给固定的 Close 按钮区域）
    int viewportContentH = winH - 40;
    if (viewportContentH < 1) viewportContentH = winH; // 兜底
    seatMapViewportH_ = viewportContentH;
    seatMapScrollPos_ = 0;
    seatMapMaxScroll_ = contentH > viewportContentH ? (contentH - viewportContentH) : 0;
    if (seatMapMaxScroll_ > 0) {
        LONG style = GetWindowLong(seatMapWindow_, GWL_STYLE);
        SetWindowLong(seatMapWindow_, GWL_STYLE, style | WS_VSCROLL);
        SCROLLINFO si = {}; si.cbSize = sizeof(si); si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS; si.nMin = 0; si.nMax = contentH - 1; si.nPage = viewportContentH; si.nPos = 0; SetScrollInfo(seatMapWindow_, SB_VERT, &si, TRUE);
    }

    // 绘制座位按钮
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = 0; c < colCount; ++c) {
            char colChar = 'A' + c;
            Seat* seat = ac->getSeat(r, colChar);
            if (!seat) continue;
            char label[12]; sprintf(label, "%d%c", r, colChar);
            int x = 20 + c * (btnW + pad);
            int y = 40 + (r - startRow) * (btnH + pad);
            int seatId = ID_BTN_SEATMAP_BASE + r * 64 + c; // 编码行列
            HWND hBtn = CreateWindow("BUTTON", label,
                WS_VISIBLE | WS_CHILD | (seat->isBooked() ? WS_DISABLED : 0),
                x, y, btnW, btnH,
                seatMapWindow_, (HMENU)(INT_PTR)seatId, GetModuleHandle(NULL), NULL);
            (void)hBtn;
            seatMapButtons_.push_back(hBtn);
            seatMapBtnBaseY_.push_back(y);
        }
    }

    // 底部固定关闭按钮已移除：关闭请使用窗口标题栏的 X（将在 WM_CLOSE 中处理）

    ShowWindow(seatMapWindow_, SW_SHOW);
    UpdateWindow(seatMapWindow_);
}

LRESULT CALLBACK PassengerUI::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PassengerUI* pUI = g_pUI;
    
    switch (uMsg) {
        case WM_CREATE:
            return 0;
            
        case WM_COMMAND:
            if (pUI) {
                // 先处理动态按钮范围（座位图按钮与分段座位图按钮）
                int dynId = LOWORD(wParam);
                if (dynId >= ID_BTN_SEATMAP_BASE && dynId < ID_BTN_SEATMAP_BASE + 100000) {
                    // 动态座位按钮点击：读取按钮文本回填目标编辑框并关闭座位图窗口
                    if (pUI->seatMapTargetEdit_ && (HWND)lParam) {
                        char seatBuf[32] = {0};
                        GetWindowText((HWND)lParam, seatBuf, 31);
                        pUI->setEditText(pUI->seatMapTargetEdit_, std::string(seatBuf));
                        pUI->showMessage(std::string("Selected seat: ") + seatBuf, "Seat Selected");
                        if (pUI->seatMapWindow_) { DestroyWindow(pUI->seatMapWindow_); pUI->seatMapWindow_ = nullptr; }
                        pUI->seatMapTargetEdit_ = nullptr;
                    }
                    return 0;
                }
                if (dynId >= ID_BTN_SEG_MAP_BASE && dynId < ID_BTN_SEG_MAP_BASE + 200) {
                    // 分段座位图按钮：打开对应航段的座位图，目标为对应的分段座位输入框
                    int segIdx = dynId - ID_BTN_SEG_MAP_BASE;
                    if (segIdx >= 0 && segIdx < (int)pUI->currentSegFlights_.size()) {
                        // 找到当前订单与舱位
                        SeatType st = SeatType::ECONOMY_CLASS;
                        if (!pUI->currentSegOrder_.empty() && pUI->system->getCurrentPassenger()) {
                            auto orders = pUI->system->getPassengerOrders(pUI->system->getCurrentPassenger()->getPassportNumber());
                            for (auto* o: orders) {
                                if (o->getOrderNumber() == pUI->currentSegOrder_) { st = o->getSeatType(); break; }
                            }
                        }
                        HWND hTarget = nullptr;
                        if (pUI->segDialogWindow_ && segIdx < (int)pUI->segDialogEdits_.size()) {
                            hTarget = pUI->segDialogEdits_[segIdx];
                        }
                        if (!hTarget) {
                            hTarget = GetDlgItem(hwnd, ID_EDIT_SEG_BASE + segIdx);
                        }
                        if (hTarget) {
                            pUI->openSeatMapForFlight(pUI->currentSegFlights_[segIdx], pUI->currentSegDate_, st, hTarget);
                        } else {
                            pUI->showMessage("Target segment seat field not found.", "Error");
                        }
                        return 0; // 只有在处理了有效 segIdx 时才返回，避免拦截其他同一范围内的命令（如 OK/Cancel）
                    }
                    // segIdx 无效时不要返回，这样后续的命令处理（比如 ID_BTN_SEG_OK）仍可执行
                }
                switch (LOWORD(wParam)) {
                    case ID_BTN_CABIN_FIRST:
                        pUI->cabinPickedTypeVal_ = (int)SeatType::FIRST_CLASS;
                        pUI->cabinPicked_ = true;
                        if (pUI->cabinDlgWindow_) { DestroyWindow(pUI->cabinDlgWindow_); pUI->cabinDlgWindow_ = nullptr; }
                        return 0;
                    case ID_BTN_CABIN_BUSINESS:
                        pUI->cabinPickedTypeVal_ = (int)SeatType::BUSINESS_CLASS;
                        pUI->cabinPicked_ = true;
                        if (pUI->cabinDlgWindow_) { DestroyWindow(pUI->cabinDlgWindow_); pUI->cabinDlgWindow_ = nullptr; }
                        return 0;
                    case ID_BTN_CABIN_PREM:
                        pUI->cabinPickedTypeVal_ = (int)SeatType::PREMIUM_ECONOMY;
                        pUI->cabinPicked_ = true;
                        if (pUI->cabinDlgWindow_) { DestroyWindow(pUI->cabinDlgWindow_); pUI->cabinDlgWindow_ = nullptr; }
                        return 0;
                    case ID_BTN_CABIN_ECO:
                        pUI->cabinPickedTypeVal_ = (int)SeatType::ECONOMY_CLASS;
                        pUI->cabinPicked_ = true;
                        if (pUI->cabinDlgWindow_) { DestroyWindow(pUI->cabinDlgWindow_); pUI->cabinDlgWindow_ = nullptr; }
                        return 0;
                    case ID_BTN_LOGIN:
                        pUI->handleLogin();
                        break;
                    case ID_BTN_REGISTER:
                        pUI->handleRegister();
                        break;
                    case ID_BTN_BACK_TO_MENU:
                        pUI->showMainMenu();
                        break;
                    case ID_BTN_BACK_TO_SELECTION: {
                        // 重新启动本程序以回到模式选择界面
                        char exePath[MAX_PATH] = {0};
                        GetModuleFileNameA(NULL, exePath, MAX_PATH);
                        STARTUPINFOA si; ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si);
                        PROCESS_INFORMATION pi; ZeroMemory(&pi, sizeof(pi));
                        if (CreateProcessA(exePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                            CloseHandle(pi.hThread);
                            CloseHandle(pi.hProcess);
                        }
                        PostQuitMessage(0);
                        break;
                    }
                    case ID_BTN_SEARCH:
                        pUI->handleSearch();
                        break;
                    case ID_BTN_CONNECTING_SEARCH:
                        pUI->handleConnectingSearch();
                        break;
                    case ID_BTN_BOOK:
                        pUI->handleBook();
                        break;
                    case ID_BTN_CONNECTING_BOOK:
                        pUI->handleConnectingBook();
                        break;
                    case ID_BTN_PAY: {
                        // 处理支付（从两个输入框）
                        HWND hwndOrderEdit = GetDlgItem(hwnd, ID_EDIT_ORIGIN);
                        HWND hwndCardEdit = GetDlgItem(hwnd, ID_EDIT_DEST);
                        if (hwndOrderEdit && hwndCardEdit) {
                            std::string orderNum = pUI->getEditText(hwndOrderEdit);
                            std::string cardNum = pUI->getEditText(hwndCardEdit);
                            if (orderNum.empty() || cardNum.empty()) {
                                pUI->showMessage("Please fill in all fields!", "Error");
                            } else if (pUI->system->payOrder(orderNum, cardNum)) {
                                pUI->showMessage("Payment successful! You can now check in.", "Success");
                                pUI->showMainMenu();
                            } else {
                                pUI->showMessage("Payment failed. Please check order number and status.", "Error");
                            }
                        } else {
                            pUI->handlePay();
                        }
                        break;
                    }
                    case ID_BTN_OPEN_MAP: {
                        // 打开直飞订单的座位图（联程订单请使用每段的 Seat Map 按钮）
                        HWND hwndList = GetDlgItem(hwnd, ID_LIST_ORDERS);
                        HWND hwndSeatEdit = GetDlgItem(hwnd, ID_EDIT_DEST);
                        if (!hwndList || !hwndSeatEdit) { pUI->showMessage("Order list or seat field not found.", "Error"); break; }
                        int sel = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
                        if (sel == -1) { pUI->showMessage("Select a PAID order first.", "Info"); break; }
                        char obuf[256]; ListView_GetItemText(hwndList, sel, 0, obuf, 256);
                        std::string orderNum(obuf);
                        auto orders = pUI->system->getPassengerOrders(pUI->system->getCurrentPassenger()->getPassportNumber());
                        Order* target = nullptr; for (auto* o: orders) if (o->getOrderNumber()==orderNum){ target=o; break; }
                        if (!target) { pUI->showMessage("Order not found.", "Error"); break; }
                        if (target->isConnectingFlight()) {
                            // 如果是联程，打开分段选座对话（与原先 Load Segments 的行为一致）
                            if (!pUI->system->getCurrentPassenger()) { pUI->showMessage("Please login first.", "Error"); break; }
                            // ensure we have the selected order
                            if (target->getStatus() != OrderStatus::PAID) { pUI->showMessage("Order must be PAID.", "Info"); break; }

                            auto flights2 = target->getConnectingFlightNumbers();
                            pUI->currentSegFlights_ = flights2;
                            pUI->currentSegOrder_ = target->getOrderNumber();
                            pUI->currentSegDate_ = target->getFlightDate();
                            pUI->pendingSegSeats_.clear();

                            // 创建分段选座小窗口
                            int segCount = (int)flights2.size();
                            int dlgW = 520; int dlgH = 120 + segCount * 36 + 50;
                            if (pUI->segDialogWindow_) { DestroyWindow(pUI->segDialogWindow_); pUI->segDialogWindow_ = nullptr; }
                            pUI->segDialogWindow_ = CreateWindowEx(WS_EX_DLGMODALFRAME, "AirlineReservationPassengerWindow", "Connecting Segments Seats",
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                220, 160, dlgW, dlgH,
                                hwnd, NULL, GetModuleHandle(NULL), NULL);
                            if (!pUI->segDialogWindow_) { pUI->showMessage("Cannot open segment dialog.", "Error"); break; }

                            pUI->segDialogEdits_.clear(); pUI->segDialogEdits_.reserve(segCount);
                            int baseY = 40; int gapY = 36;
                            for (int i=0;i<segCount;++i) {
                                std::string label = "Segment " + std::to_string(i+1) + " (" + flights2[i] + "):";
                                CreateWindow("STATIC", label.c_str(), WS_VISIBLE|WS_CHILD,
                                    20, baseY + i*gapY, 220, 24,
                                    pUI->segDialogWindow_, NULL, GetModuleHandle(NULL), NULL);
                                HWND hEdit = CreateWindow("EDIT", "", WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL,
                                    250, baseY + i*gapY, 120, 24,
                                    pUI->segDialogWindow_, (HMENU)(INT_PTR)(ID_EDIT_SEG_BASE + i), GetModuleHandle(NULL), NULL);
                                pUI->segDialogEdits_.push_back(hEdit);
                                // 每段座位图按钮
                                CreateWindow("BUTTON", "Seat Map", WS_VISIBLE|WS_CHILD,
                                    380, baseY + i*gapY, 90, 24,
                                    pUI->segDialogWindow_, (HMENU)(INT_PTR)(ID_BTN_SEG_MAP_BASE + i), GetModuleHandle(NULL), NULL);
                            }

                            // 添加确定/取消按钮
                            int btnY = baseY + segCount * gapY + 10;
                            CreateWindowW(L"BUTTON", L"Confirm", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                dlgW/2 - 90, btnY, 80, 30,
                                pUI->segDialogWindow_, (HMENU)ID_BTN_SEG_OK, GetModuleHandle(NULL), NULL);
                            CreateWindowW(L"BUTTON", L"Cancel", WS_VISIBLE | WS_CHILD,
                                dlgW/2 + 10, btnY, 80, 30,
                                pUI->segDialogWindow_, (HMENU)ID_BTN_SEG_CANCEL, GetModuleHandle(NULL), NULL);

                            ShowWindow(pUI->segDialogWindow_, SW_SHOW);
                            UpdateWindow(pUI->segDialogWindow_);
                            break;
                        }

                        // 直飞订单打开座位图
                        pUI->openSeatMapForFlight(target->getFlightNumber(), target->getFlightDate(), target->getSeatType(), hwndSeatEdit);
                        break;
                    }
                    case ID_BTN_PAY + 1000: {
                        // 从列表执行支付
                        HWND hwndList = GetDlgItem(hwnd, ID_LIST_ORDERS);
                        HWND hwndCardEdit = GetDlgItem(hwnd, ID_EDIT_DEST);
                        if (hwndList && hwndCardEdit) {
                            int sel = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
                            if (sel == -1) { pUI->showMessage("Please select an order.", "Error"); break; }
                            char buf[256]; ListView_GetItemText(hwndList, sel, 0, buf, 256);
                            std::string orderNum(buf);
                            std::string cardNum = pUI->getEditText(hwndCardEdit);
                            if (cardNum.empty()) { pUI->showMessage("Please enter credit card last 4.", "Error"); break; }
                            if (pUI->system->payOrder(orderNum, cardNum)) {
                                pUI->showMessage("Payment successful!", "Success");
                                pUI->handlePay();
                            } else {
                                pUI->showMessage("Payment failed.", "Error");
                            }
                        }
                        break;
                    }
                    case ID_BTN_CHECKIN:
                        pUI->handleCheckIn();
                        break;
                    case ID_BTN_LOAD_SEGMENTS: {
                        // 弹出小窗口进行联程分段选座（不在主页面展示）
                        if (!pUI->system->getCurrentPassenger()) { pUI->showMessage("Please login first.", "Error"); break; }
                        HWND hwndList2 = GetDlgItem(hwnd, ID_LIST_ORDERS);
                        if (!hwndList2) { pUI->showMessage("Order list not found.", "Error"); break; }
                        int sel2 = ListView_GetNextItem(hwndList2, -1, LVNI_SELECTED);
                        if (sel2 == -1) { pUI->showMessage("Select a PAID connecting order first.", "Info"); break; }
                        char obuf2[256]; ListView_GetItemText(hwndList2, sel2, 0, obuf2, 256);
                        std::string orderNum2(obuf2);
                        auto orders2 = pUI->system->getPassengerOrders(pUI->system->getCurrentPassenger()->getPassportNumber());
                        Order* target2 = nullptr; for (auto* o: orders2) if (o->getOrderNumber()==orderNum2){ target2=o; break; }
                        if (!target2) { pUI->showMessage("Order not found.", "Error"); break; }
                        if (!target2->isConnectingFlight()) { pUI->showMessage("This is not a connecting flight order.", "Info"); break; }
                        if (target2->getStatus() != OrderStatus::PAID) { pUI->showMessage("Order must be PAID.", "Info"); break; }

                        auto flights2 = target2->getConnectingFlightNumbers();
                        pUI->currentSegFlights_ = flights2;
                        pUI->currentSegOrder_ = target2->getOrderNumber();
                        pUI->currentSegDate_ = target2->getFlightDate();
                        pUI->pendingSegSeats_.clear();

                        // 创建分段选座小窗口
                        int segCount = (int)flights2.size();
                        int dlgW = 520; int dlgH = 120 + segCount * 36 + 50;
                        if (pUI->segDialogWindow_) { DestroyWindow(pUI->segDialogWindow_); pUI->segDialogWindow_ = nullptr; }
                        pUI->segDialogWindow_ = CreateWindowEx(WS_EX_DLGMODALFRAME, "AirlineReservationPassengerWindow", "Connecting Segments Seats",
                            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                            220, 160, dlgW, dlgH,
                            hwnd, NULL, GetModuleHandle(NULL), NULL);
                        if (!pUI->segDialogWindow_) { pUI->showMessage("Cannot open segment dialog.", "Error"); break; }

                        pUI->segDialogEdits_.clear(); pUI->segDialogEdits_.reserve(segCount);
                        int baseY = 40; int gapY = 36;
                        for (int i=0;i<segCount;++i) {
                            std::string label = "Segment " + std::to_string(i+1) + " (" + flights2[i] + "):";
                            CreateWindow("STATIC", label.c_str(), WS_VISIBLE|WS_CHILD,
                                20, baseY + i*gapY, 220, 24,
                                pUI->segDialogWindow_, NULL, GetModuleHandle(NULL), NULL);
                            HWND hEdit = CreateWindow("EDIT", "", WS_VISIBLE|WS_CHILD|WS_BORDER|ES_AUTOHSCROLL,
                                250, baseY + i*gapY, 120, 24,
                                pUI->segDialogWindow_, (HMENU)(INT_PTR)(ID_EDIT_SEG_BASE + i), GetModuleHandle(NULL), NULL);
                            pUI->segDialogEdits_.push_back(hEdit);
                            // 每段座位图按钮
                            CreateWindow("BUTTON", "Seat Map", WS_VISIBLE|WS_CHILD,
                                380, baseY + i*gapY, 90, 24,
                                pUI->segDialogWindow_, (HMENU)(INT_PTR)(ID_BTN_SEG_MAP_BASE + i), GetModuleHandle(NULL), NULL);
                        }

                        // 添加确定/取消按钮，便于用户直接在对话框中确认或取消分段座位选择
                        int btnY = baseY + segCount * gapY + 10;
                        // '确定' 按钮（使用 Unicode 宽字符串以避免编码问题）
                        CreateWindowW(L"BUTTON", L"确定", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                            dlgW/2 - 90, btnY, 80, 30,
                            pUI->segDialogWindow_, (HMENU)ID_BTN_SEG_OK, GetModuleHandle(NULL), NULL);
                        // '取消' 按钮（使用 Unicode 宽字符串）
                        CreateWindowW(L"BUTTON", L"取消", WS_VISIBLE | WS_CHILD,
                            dlgW/2 + 10, btnY, 80, 30,
                            pUI->segDialogWindow_, (HMENU)ID_BTN_SEG_CANCEL, GetModuleHandle(NULL), NULL);

                        ShowWindow(pUI->segDialogWindow_, SW_SHOW);
                        UpdateWindow(pUI->segDialogWindow_);
                        break;
                    }
                    case ID_BTN_SEG_OK: {
                        // 收集分段座位输入并直接执行值机（与 "Check In Now" 按钮相同的逻辑）
                        pUI->pendingSegSeats_.clear();
                        for (HWND hEdit : pUI->segDialogEdits_) {
                            char sbuf[64]; GetWindowText(hEdit, sbuf, 63);
                            pUI->pendingSegSeats_.push_back(std::string(sbuf));
                        }
                        if (pUI->segDialogWindow_) { DestroyWindow(pUI->segDialogWindow_); pUI->segDialogWindow_ = nullptr; }

                        // 使用当前保存的分段订单号（在 Load Segments 时已设置 currentSegOrder_）执行联程值机
                        std::string orderNum = pUI->currentSegOrder_;
                        if (orderNum.empty()) {
                            pUI->showMessage("No segment order context. Please select the order and load segments again.", "Error");
                            break;
                        }

                        // 验证分段座位数量
                        if (pUI->pendingSegSeats_.empty() || pUI->pendingSegSeats_.size() != pUI->currentSegFlights_.size()) {
                            pUI->showMessage("Segments seat count mismatch or seats missing. Please ensure all segment seats are filled.", "Error");
                            break;
                        }

                        // 调用系统的联程值机接口
                        if (pUI->system->checkInConnectingOrder(orderNum, pUI->pendingSegSeats_)) {
                            pUI->showMessage("Connecting flight check-in successful!", "Success");
                            pUI->showMainMenu();
                        } else {
                            pUI->showMessage("Check-in failed for connecting flight.", "Error");
                        }
                        break;
                    }
                    case ID_BTN_SEG_CANCEL:
                        if (pUI->segDialogWindow_) { DestroyWindow(pUI->segDialogWindow_); pUI->segDialogWindow_ = nullptr; }
                        break;
                    case ID_BTN_SEATMAP_CLOSE:
                        if (pUI->seatMapWindow_) { DestroyWindow(pUI->seatMapWindow_); pUI->seatMapWindow_ = nullptr; pUI->seatMapTargetEdit_ = nullptr; }
                        break;
                    case ID_BTN_CHECKIN + 1000: {
                        // 实际执行值机（优先从列表选择）
                        HWND hwndList3 = GetDlgItem(hwnd, ID_LIST_ORDERS);
                        HWND hwndSeatEdit = GetDlgItem(hwnd, ID_EDIT_DEST);
                        if (hwndSeatEdit) {
                            std::string orderNum;
                            if (hwndList3) {
                                int sel = ListView_GetNextItem(hwndList3, -1, LVNI_SELECTED);
                                if (sel == -1) { pUI->showMessage("Please select an order.", "Error"); break; }
                                char buf[256]; ListView_GetItemText(hwndList3, sel, 0, buf, 256);
                                orderNum = buf;
                            } else {
                                HWND hwndOrderEdit = GetDlgItem(hwnd, ID_EDIT_ORIGIN);
                                orderNum = hwndOrderEdit ? pUI->getEditText(hwndOrderEdit) : "";
                            }
                            std::string seatInput = pUI->getEditText(hwndSeatEdit);
                            if (orderNum.empty() || seatInput.empty()) { pUI->showMessage("Please fill in all fields!", "Error"); break; }
                            // 获取订单对象判断是否联程
                            Order* target = nullptr;
                            auto orders = pUI->system->getPassengerOrders(pUI->system->getCurrentPassenger()->getPassportNumber());
                            for (auto* o : orders) if (o->getOrderNumber() == orderNum) { target = o; break; }
                            if (!target) { pUI->showMessage("Order not found.", "Error"); break; }
                            if (target->isConnectingFlight()) {
                                auto segFlights = target->getConnectingFlightNumbers();
                                std::vector<std::string> seats;
                                // 优先从临时窗口收集（pendingSegSeats_），否则尝试主界面隐藏编辑框
                                if (!pUI->pendingSegSeats_.empty()) {
                                    seats = pUI->pendingSegSeats_;
                                } else {
                                    for (size_t i=0;i<segFlights.size();++i){
                                        HWND hSegEdit = GetDlgItem(hwnd, ID_EDIT_SEG_BASE + (int)i);
                                        if (!hSegEdit){ pUI->showMessage("Please click 'Load Segments' first.", "Error"); seats.clear(); break; }
                                        std::string segSeat = pUI->getEditText(hSegEdit);
                                        if (segSeat.empty()){ pUI->showMessage("Seat missing for segment " + std::to_string(i+1), "Error"); seats.clear(); break; }
                                        seats.push_back(segSeat);
                                    }
                                }
                                if (seats.empty()) break;
                                // 基本长度校验
                                if (seats.size() != segFlights.size()) { pUI->showMessage("Segments seat count mismatch.", "Error"); break; }
                                if (pUI->system->checkInConnectingOrder(orderNum, seats)) {
                                    pUI->showMessage("Connecting flight check-in successful!", "Success");
                                    pUI->showMainMenu();
                                } else {
                                    pUI->showMessage("Check-in failed for connecting flight.", "Error");
                                }
                            } else {
                                if (pUI->system->checkInOrder(orderNum, seatInput)) {
                                    pUI->showMessage("Check-in successful! Have a nice flight!", "Success");
                                    pUI->showMainMenu();
                                } else {
                                    pUI->showMessage("Check-in failed. Please check seat availability.", "Error");
                                }
                            }
                        }
                        break;
                    }
                    case ID_BTN_VIEW_ORDERS:
                        pUI->handleViewOrders();
                        break;
                    case ID_BTN_VIEW_ORDERS + 500:
                        pUI->handleWaitlistView();
                        break;
                    case ID_BTN_VIEW_ORDERS + 501: {
                        // 从候补列表移除选中条目
                        HWND hwndList = GetDlgItem(hwnd, ID_LIST_ORDERS);
                        if (!hwndList) break;
                        int sel = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
                        if (sel == -1) { pUI->showMessage("Please select a waitlist entry.", "Info"); break; }
                        char f[128], d[128], c[128];
                        ListView_GetItemText(hwndList, sel, 0, f, 128); // Flight
                        ListView_GetItemText(hwndList, sel, 1, d, 128); // Date
                        ListView_GetItemText(hwndList, sel, 2, c, 128); // Cabin
                        SeatType st = Order::stringToSeatType(std::string(c));
                        std::string passport = pUI->system->getCurrentPassenger()->getPassportNumber();
                        if (pUI->system->removeFromWaitlist(passport, std::string(f), std::string(d), st)) {
                            pUI->showMessage("Removed from waitlist.", "Success");
                            pUI->handleWaitlistView();
                        } else {
                            pUI->showMessage("Failed to remove. Entry may have changed.", "Error");
                        }
                        break;
                    }
                    case ID_BTN_VIEW_DETAILS: {
                        // 查看选中订单详情（支持联程）
                        HWND hwndList = GetDlgItem(hwnd, ID_LIST_ORDERS);
                        if (!hwndList) break;
                        int sel = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
                        if (sel == -1) { pUI->showMessage("Please select an order first.", "Info"); break; }
                        char buf[256]; ListView_GetItemText(hwndList, sel, 0, buf, 256);
                        std::string orderNum(buf);
                        std::vector<Order*> orders = pUI->system->getPassengerOrders(pUI->system->getCurrentPassenger()->getPassportNumber());
                        Order* target = nullptr;
                        for (auto* o : orders) if (o->getOrderNumber() == orderNum) { target = o; break; }
                        if (!target) { pUI->showMessage("Order not found.", "Error"); break; }
                        std::string info = "Order Details\n\n";
                        info += "Order: " + target->getOrderNumber() + "\n";
                        info += "Status: " + Order::statusToString(target->getStatus()) + "\n";
                        info += "Cabin: " + Order::seatTypeToString(target->getSeatType()) + "\n";
                        info += "Date: " + target->getFlightDate() + "\n";
                        info += "Price: $" + std::to_string(target->getTicketPrice()) + "\n";
                        if (target->isConnectingFlight()) {
                            info += "Type: CONNECTING\nSegments:";
                            auto flights = target->getConnectingFlightNumbers();
                            auto seats = target->getConnectingSeatNumbers();
                            for (size_t i = 0; i < flights.size(); ++i) {
                                info += "\n  " + std::to_string(i+1) + ". " + flights[i];
                                if (i < seats.size() && !seats[i].empty()) info += " (Seat " + seats[i] + ")";
                            }
                        } else {
                            info += "Type: DIRECT\nFlight: " + target->getFlightNumber() + "\n";
                            info += "Seat: " + (target->getSeatNumber().empty() ? std::string("Not selected") : target->getSeatNumber()) + "\n";
                        }
                        if (!target->getPaymentTime().empty()) info += "Paid At: " + target->getPaymentTime() + "\n";
                        if (!target->getCheckInTime().empty()) info += "Check-In: " + target->getCheckInTime() + "\n";
                        pUI->showMessage(info, "Order Details");
                        break;
                    }
                    case ID_BTN_CANCEL:
                        pUI->handleCancel();
                        break;
                    case ID_BTN_CANCEL + 1000: {
                        // 实际执行取消（从列表）
                        HWND hwndList = GetDlgItem(hwnd, ID_LIST_ORDERS);
                        if (hwndList) {
                            int sel = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
                            if (sel == -1) { pUI->showMessage("Please select an order to cancel!", "Error"); break; }
                            char buf[256]; ListView_GetItemText(hwndList, sel, 0, buf, 256);
                            std::string orderNum(buf);
                            if (pUI->system->cancelOrder(orderNum)) {
                                pUI->showMessage("Order cancelled successfully!", "Success");
                                pUI->handleCancel();
                            } else {
                                pUI->showMessage("Cancel failed.", "Error");
                            }
                        }
                        break;
                    }
                    case ID_BTN_PROFILE:
                        pUI->handleProfile();
                        break;
                }
            }
            return 0;
            
        case WM_DESTROY:
            // 仅当销毁的是主窗口时才退出消息循环
            if (pUI && hwnd == pUI->hwndMain) {
                PostQuitMessage(0);
            }
            return 0;
        case WM_CLOSE:
            // 在关闭分段对话框时，收集分段座位输入（如果存在），以保持原有 OK 行为
            if (pUI) {
                if (hwnd == pUI->segDialogWindow_) {
                    pUI->pendingSegSeats_.clear();
                    for (HWND hEdit : pUI->segDialogEdits_) {
                        if (hEdit) {
                            char sbuf[128] = {0};
                            GetWindowText(hEdit, sbuf, sizeof(sbuf));
                            pUI->pendingSegSeats_.push_back(std::string(sbuf));
                        } else {
                            pUI->pendingSegSeats_.push_back(std::string());
                        }
                    }
                    // 销毁窗口并清理引用
                    DestroyWindow(pUI->segDialogWindow_);
                    pUI->segDialogWindow_ = nullptr;
                    pUI->segDialogEdits_.clear();
                    pUI->showMessage("Segment seats captured.", "Info");
                    return 0;
                }
                if (hwnd == pUI->seatMapWindow_) {
                    // 关闭座位图窗口时，清理引用
                    if (pUI->seatMapWindow_) { DestroyWindow(pUI->seatMapWindow_); pUI->seatMapWindow_ = nullptr; }
                    pUI->seatMapTargetEdit_ = nullptr;
                    return 0;
                }
            }
            break;
        case WM_VSCROLL:
            if (pUI && hwnd == pUI->seatMapWindow_) {
                int action = LOWORD(wParam);
                SCROLLINFO si = {}; si.cbSize = sizeof(si); si.fMask = SIF_ALL; GetScrollInfo(hwnd, SB_VERT, &si);
                int pos = si.nPos;
                switch (action) {
                    case SB_LINEUP: pos -= 20; break;
                    case SB_LINEDOWN: pos += 20; break;
                    case SB_PAGEUP: pos -= (int)si.nPage; break;
                    case SB_PAGEDOWN: pos += (int)si.nPage; break;
                    case SB_THUMBTRACK: pos = si.nTrackPos; break;
                    default: break;
                }
                if (pos < 0) pos = 0; if (pos > pUI->seatMapMaxScroll_) pos = pUI->seatMapMaxScroll_;
                si.fMask = SIF_POS; si.nPos = pos; SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
                if (pos != pUI->seatMapScrollPos_) {
                    int delta = pos - pUI->seatMapScrollPos_;
                    pUI->seatMapScrollPos_ = pos;
                    // 重新定位座位按钮
                    for (size_t i=0;i<pUI->seatMapButtons_.size();++i){
                        int newY = pUI->seatMapBtnBaseY_[i] - pUI->seatMapScrollPos_;
                        RECT rc; GetWindowRect(pUI->seatMapButtons_[i], &rc);
                        MapWindowPoints(NULL, hwnd, (LPPOINT)&rc, 2);
                        MoveWindow(pUI->seatMapButtons_[i], rc.left, newY, rc.right - rc.left, rc.bottom - rc.top, TRUE);
                    }
                }
                return 0;
            }
            break;
        case WM_MOUSEWHEEL:
            if (pUI && hwnd == pUI->seatMapWindow_) {
                int zDelta = (short)HIWORD(wParam);
                SCROLLINFO si = {}; si.cbSize = sizeof(si); si.fMask = SIF_ALL; GetScrollInfo(hwnd, SB_VERT, &si);
                int pos = si.nPos;
                int step = 30; // 每格滚动像素
                if (zDelta > 0) pos -= step; else if (zDelta < 0) pos += step;
                if (pos < 0) pos = 0; if (pos > pUI->seatMapMaxScroll_) pos = pUI->seatMapMaxScroll_;
                si.fMask = SIF_POS; si.nPos = pos; SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
                if (pos != pUI->seatMapScrollPos_) {
                    pUI->seatMapScrollPos_ = pos;
                    for (size_t i=0;i<pUI->seatMapButtons_.size();++i){
                        int newY = pUI->seatMapBtnBaseY_[i] - pUI->seatMapScrollPos_;
                        RECT rc; GetWindowRect(pUI->seatMapButtons_[i], &rc);
                        MapWindowPoints(NULL, hwnd, (LPPOINT)&rc, 2);
                        MoveWindow(pUI->seatMapButtons_[i], rc.left, newY, rc.right - rc.left, rc.bottom - rc.top, TRUE);
                    }
                }
                return 0;
            }
            break;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

