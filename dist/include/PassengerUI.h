#ifndef PASSENGER_UI_H
#define PASSENGER_UI_H

#undef UNICODE
#undef _UNICODE
#include <windows.h>
#include <string>
#include <vector>
#include "AirlineReservationSystem.h"
// For SeatType value storage
#include "Seat.h"

// 乘客UI类 - 使用Win32 API创建图形界面
class PassengerUI {
public:
    PassengerUI(AirlineReservationSystem* system);
    ~PassengerUI();

    // 显示主窗口
    void show();
    
    // 窗口过程
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    enum class Screen { LOGIN, MAIN_MENU, SEARCH, BOOKING, ORDERS, PAY, CHECKIN, CANCEL, PROFILE, WAITLIST };
    Screen currentScreen_ = Screen::LOGIN;
    AirlineReservationSystem* system;
    HWND hwndMain;
    HWND hwndLoginButton;
    HWND hwndRegisterButton;
    HWND hwndPassportEdit;
    HWND hwndNameEdit;
    HWND hwndPasswordEdit;
    
    // 控件ID定义
    static const int ID_BTN_LOGIN = 101;
    static const int ID_BTN_REGISTER = 102;
    static const int ID_BTN_SEARCH = 103;
    static const int ID_BTN_BOOK = 104;
    static const int ID_BTN_PAY = 105;
    static const int ID_BTN_CHECKIN = 106;
    static const int ID_BTN_VIEW_ORDERS = 107;
    static const int ID_BTN_CANCEL = 108;
    static const int ID_BTN_PROFILE = 109;
    static const int ID_BTN_BACK_TO_MENU = 110; // Back button used across screens
    static const int ID_BTN_BACK_TO_SELECTION = 114; // Back to system selection (mode chooser)
    static const int ID_BTN_VIEW_DETAILS = 111; // View details of selected order
    static const int ID_BTN_LOAD_SEGMENTS = 112; // Generate per-segment seat inputs for connecting flights
    static const int ID_BTN_OPEN_MAP = 113; // Open seat map for direct flight
    static const int ID_BTN_SEG_MAP_BASE = 900; // Base id for per-segment seat-map buttons
    static const int ID_BTN_SEG_OK = 920;       // Confirm seats in segment dialog
    static const int ID_BTN_SEG_CANCEL = 921;   // Cancel segment dialog
    static const int ID_BTN_SEATMAP_BASE = 20000; // Dynamic seat buttons base id
    static const int ID_BTN_SEATMAP_CLOSE = 21000; // Close button in seat map popup
    // Cabin selection dialog button IDs
    static const int ID_BTN_CABIN_FIRST = 5001;
    static const int ID_BTN_CABIN_BUSINESS = 5002;
    static const int ID_BTN_CABIN_PREM = 5003;
    static const int ID_BTN_CABIN_ECO = 5004;
    static const int ID_EDIT_PASSPORT = 201;
    static const int ID_EDIT_NAME = 202;
    static const int ID_EDIT_PASSWORD = 1005;
    static const int ID_EDIT_ORIGIN = 203;
    static const int ID_EDIT_DEST = 204;
    static const int ID_EDIT_DATE = 205;
    static const int ID_LIST_FLIGHTS = 301;
    static const int ID_LIST_ORDERS = 302;
    // 连接航班相关控件ID（避免与已存在ID冲突，使用 650+ 段）
    static const int ID_LIST_CONNECTING = 650; // 转机路线列表
    static const int ID_BTN_CONNECTING_SEARCH = 651; // 触发转机搜索
    static const int ID_BTN_CONNECTING_BOOK = 652;   // 预订选中转机路线
    static const int ID_EDIT_ORDER_NUM = 303; // optional explicit order edit id
    static const int ID_EDIT_SEG_BASE = 700;   // dynamic edits for segment seats
    static const int ID_STATIC_SEG_BASE = 800; // dynamic labels for segment seats
    
    // 创建窗口
    bool createMainWindow();
    
    // 显示不同的界面
    void showLoginScreen();
    void showMainMenu();
    void showSearchScreen();
    void showBookingScreen();
    void showOrdersScreen();
    
    // 处理按钮点击
    void handleLogin();
    void handleRegister();
    void handleSearch();
    void handleBook();
    void handleConnectingSearch();
    void handleConnectingBook();
    void handlePay();
    void handleCheckIn();
    void handleViewOrders();
    void handleCancel();
    void handleProfile();
    void handleWaitlistView();
    
    // 辅助函数
    void clearWindow();
    std::string getEditText(HWND hwndEdit);
    void setEditText(HWND hwndEdit, const std::string& text);
    void showMessage(const std::string& message, const std::string& title = "Info");

    // Seat map helpers
    void openSeatMapForFlight(const std::string& flightNo, const std::string& flightDate, SeatType desiredType, HWND targetEdit);

    // State for connecting segments seat-map integration
    std::vector<std::string> currentSegFlights_;
    std::string currentSegOrder_;
    std::string currentSegDate_;
    // 每段对应的日期（根据订单+系统航班推断），与 currentSegFlights_ 一一对应
    std::vector<std::string> currentSegDates_;

    // 解析并填充 currentSegDates_：基于订单的第一段日期与系统航班时刻，推断每段的实际日期
    void computeSegmentDatesFromOrder(Order* order);
    // 工具：将 HH:MM 或 HH:MM+1 解析为分钟与天偏移
    static void parseTimeWithDayOffset(const std::string& t, int& minutesOut, int& dayOffsetOut);

    // Seat map transient state
    HWND seatMapWindow_ = nullptr;
    HWND seatMapTargetEdit_ = nullptr;
    std::string seatMapFlightNo_;
    std::string seatMapFlightDate_;
    // Seat map scroll/controls
    std::vector<HWND> seatMapButtons_;
    std::vector<int> seatMapBtnBaseY_;
    int seatMapScrollPos_ = 0;
    int seatMapMaxScroll_ = 0;
    int seatMapViewportH_ = 0;

    // Segment seat dialog state
    HWND segDialogWindow_ = nullptr;
    std::vector<HWND> segDialogEdits_;
    std::vector<std::string> pendingSegSeats_;

    // Cache of last connecting routes from the most recent search so list items can reference them
    std::vector<ConnectingFlightRoute> lastConnectingRoutes_;
    // Cache of last direct-flight search results (pointers owned by system)
    std::vector<Flight*> lastSearchFlights_;

    // Cabin selection dialog transient state
    HWND cabinDlgWindow_ = nullptr;
    bool cabinPicked_ = false;
    int cabinPickedTypeVal_ = (int)SeatType::ECONOMY_CLASS;
};

#endif // PASSENGER_UI_H
