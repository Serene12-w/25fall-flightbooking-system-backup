#ifndef RESERVATION_SYSTEM_H
#define RESERVATION_SYSTEM_H

#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include "Flight.h"
#include "Passenger.h"
#include "Reservation.h"
#include "Aircraft.h"
#include "Crew.h"
#include "FlightPlan.h"
#include "OrderManager.h"

// 乘客树节点
struct PassengerNode {
    Passenger* passenger;
    PassengerNode* left;
    PassengerNode* right;

    PassengerNode(Passenger* p) : passenger(p), left(nullptr), right(nullptr) {}
};

// 候补名单条目
struct WaitlistEntry {
    Passenger* passenger;
    Flight* flight;
    SeatType seatType;
    int priority; // 优先级，根据常旅客等级计算

    // 重载 > 运算符用于优先队列
    bool operator>(const WaitlistEntry& other) const {
        return priority < other.priority; // 优先级高的在前
    }
};

// 联程航班结构
struct ConnectingFlightRoute {
    std::vector<Flight*> flights;        // 航班序列
    double totalDistance;                // 总距离
    int totalMinutes;                    // 总时间（分钟）
    std::vector<int> layoverMinutes;     // 每次转机的停留时间（分钟）
    bool isValid;                        // 路线是否有效（时间衔接合理）
    
    ConnectingFlightRoute() : totalDistance(0.0), totalMinutes(0), isValid(true) {}
};

// 航空公司预订系统类
class AirlineReservationSystem {
public:
    struct PriceBreakdown {
        double basePrice;
        double discountRate;
        double discountAmount;
        double finalPrice;
        PriceBreakdown() : basePrice(-1), discountRate(0), discountAmount(0), finalPrice(-1) {}
    };
    // 构造函数
    AirlineReservationSystem();

    // 析构函数
    ~AirlineReservationSystem();

    // 运行系统
    void run();
    
    // 运行控制台管理员系统
    void runAdminConsole();
    
    // ========== 公共API供UI调用 ==========
    
    // 乘客登录/注册
    bool loginPassenger(const std::string& passportNumber);
    bool registerPassenger(const std::string& name, const std::string& passportNumber);
    Passenger* getCurrentPassenger() const { return currentPassenger; }
    
    // 航班搜索
    std::vector<Flight*> searchFlights(const std::string& origin, const std::string& destination, const std::string& date);
    
    // 订单操作
    Order* createBooking(const std::string& flightNumber, const std::string& flightDate, SeatType seatType);
    bool payOrder(const std::string& orderNumber, const std::string& creditCard);
    bool checkInOrder(const std::string& orderNumber, const std::string& seatNumber);
    // 联程值机：一次性提交所有航段座位号（与订单中联程航段数量一致）
    bool checkInConnectingOrder(const std::string& orderNumber, const std::vector<std::string>& seatNumbers);
    // 获取当前登录乘客的候补列表（航班号, 日期, 舱位, 优先级）
    std::vector<WaitlistEntry> getPassengerWaitlist(const std::string& passportNumber);
    // 从候补队列移除一条记录（按乘客+航班+日期+舱位匹配）
    bool removeFromWaitlist(const std::string& passportNumber, const std::string& flightNumber, const std::string& flightDate, SeatType seatType);
    bool cancelOrder(const std::string& orderNumber);
    std::vector<Order*> getPassengerOrders(const std::string& passportNumber);
    // 价格预估（含会员折扣），找不到航班返回负数
    double quotePrice(const std::string& flightNumber, const std::string& flightDate, SeatType seatType);
    // 精确价格拆分（基础价+折扣率+折扣额+最终价）；找不到航班返回 base/final 为负
    PriceBreakdown getPriceBreakdown(const std::string& flightNumber, const std::string& flightDate, SeatType seatType);
    // 供 UI 演示：为当前登录乘客添加候补条目（若航班与日期匹配）。成功返回 true
    bool addToWaitlistForCurrent(const std::string& flightNumber, const std::string& flightDate, SeatType seatType);
    
    // 航班信息
    Flight* getFlightByNumber(const std::string& flightNumber);
    // 按航班号+日期精确获取航班（当同一航班号存在多天时，避免 UI 选择错实例）
    Flight* getFlightByNumberAndDate(const std::string& flightNumber, const std::string& flightDate);
    std::vector<Flight*> getAllFlights() const { return flights; }
    
    // 管理员验证
    bool verifyAdmin(const std::string& username, const std::string& password);

    // ========== UI 辅助公共接口（联程） ==========
    // 为 UI 暴露的转机搜索（封装内部算法，返回候选路线）
    std::vector<ConnectingFlightRoute> searchConnectingFlightsUI(const std::string& origin, const std::string& destination, const std::string& travelDate);
    // 为 UI 创建联程订单（不进行交互输出，返回创建的订单，失败返回 nullptr）
    Order* createConnectingBookingFromRoute(const ConnectingFlightRoute& route, SeatType seatType);

private:
    // 成员变量
    std::vector<Airport*> airports;   // 机场列表
    std::vector<Aircraft*> aircrafts; // 飞机列表
    std::vector<Flight*> flights;     // 航班列表
    std::vector<Crew*> crewMembers;   // 机组成员列表
    std::vector<Passenger*> passengers; // 乘客列表
    std::unordered_map<std::string, Reservation*> reservations; // 预订信息 (哈希表)
    std::priority_queue<WaitlistEntry, std::vector<WaitlistEntry>, std::greater<WaitlistEntry>> waitlist; // 候补名单 (优先队列)
    PassengerNode* passengerTree; // 乘客信息 (树)
    std::unordered_map<std::string, std::vector<Flight*>> flightGraph; // 航班图
    OrderManager orderManager; // 订单管理器
    Passenger* currentPassenger; // 当前登录的乘客
    // 预处理: 飞机型号 -> 有资质的机长列表 (用于快速选择最近空闲的机长)
    std::unordered_map<std::string, std::vector<Crew*>> pilotsByAircraftType;
    // 预处理: 飞机型号 -> 有资质的乘务员列表 (用于快速选择最近空闲的乘务员)
    std::unordered_map<std::string, std::vector<Crew*>> cabinCrewByAircraftType;

    // 初始化数据
    void initializeData();

    // 主菜单
    void showMainMenu();

    // 乘客菜单
    void showPassengerMenu();

    // 管理员菜单
    void showAdminMenu();
    void manageCrew();
    void manageAircrafts();
    void manageFlights();

    // 乘客登录
    bool passengerLogin();

    // 管理员登录
    bool adminLogin();

    // 乘客功能
    void bookTicket();           // 步骤1: 预订(创建订单,状态为待付款)
    void payForTicket();         // 步骤2: 付款出票(输入信用卡,状态改为已付款)
    void checkIn();              // 步骤3: 值机选座(选择座位,状态改为已值机)
    void cancelReservation();
    void confirmReservation();
    void changeReservation();
    void searchFlights();
    void viewMyReservations();   // 查看我的订单
    void viewPassengerProfile(); // 查看个人信息和会员权益

    // 管理员功能
    void viewPassengerList();
    void viewFlightStatus();
    void generateFlightBriefing();
    void generateFlightPlan();

    // 辅助函数
    Flight* findFlight(const std::string& flightNumber);
    Passenger* findPassenger(const std::string& passportNumber);
    void addPassenger(Passenger* passenger);
    void displayPassengers(PassengerNode* node);
    void addAircraftModel();
    void addFlight();
    void removeFlight();
    void addCrewMember();
    void viewCrewMembers();
    void assignCrewToFlight();
    void scheduleNewFlight(); // 新增: 自动安排航班
    Aircraft* selectOptimalAircraft(double distanceKm); // 新增: 自动选择最优机型
    std::vector<Crew*> assignCrewForAircraft(const std::string& aircraftType); // 新增: 自动分配机组
    Airport* findNearestAlternateAirport(Airport* destination); // 新增: 查找最近备降机场
    
    // 转机航班搜索相关函数
    std::vector<ConnectingFlightRoute> findConnectingFlights(const std::string& origin, const std::string& destination, const std::string& travelDate);
    bool isValidLayover(Flight* flight1, Flight* flight2);
    int calculateFlightTimeMinutes(const std::string& departureTime, const std::string& arrivalTime, const std::string& flightDate);
    int calculateLayoverMinutes(Flight* flight1, Flight* flight2);
    std::string addMinutesToTime(const std::string& time, int minutes);
    int timeToMinutes(const std::string& time);
    std::string minutesToTime(int minutes);
    void displayConnectingRoute(const ConnectingFlightRoute& route, int index);
    void bookConnectingFlight(const ConnectingFlightRoute& route, SeatType seatType);
    
    std::vector<std::vector<Flight*>> findConnectingFlights(const std::string& origin, const std::string& destination);
    void findConnectingFlightsDFS(const std::string& current, const std::string& destination, std::vector<Flight*>& path, std::vector<bool>& visited, std::vector<std::vector<Flight*>>& result);
    int getLoyaltyPriority(LoyaltyStatus status);
    std::string generateReservationNumber();
    
    // 价格计算函数
    double calculateBasePrice(Flight* flight, SeatType seatType);
    double getLoyaltyDiscount(LoyaltyStatus status);
    double calculateFinalPrice(Flight* flight, SeatType seatType, LoyaltyStatus loyaltyStatus);
    
    // 舱位降级辅助函数
    SeatType getDowngradedCabinClass(SeatType originalClass, Aircraft* aircraft);
    std::string seatTypeToString(SeatType seatType);
    
    // 候补队列相关函数
    bool hasAvailableSeats(Flight* flight, SeatType seatType);
    void addToWaitlist(Passenger* passenger, Flight* flight, SeatType seatType);
    void processWaitlist(Flight* flight, SeatType seatType);
    void viewWaitlist(); // 查看候补队列（管理员功能）
    void saveWaitlistToCSV(const std::string& filename); // 保存候补队列到CSV
    void loadWaitlistFromCSV(const std::string& filename); // 从CSV加载候补队列
    
    // 数据保存函数
    void saveAllData();
    void saveAircraftsData();
    void saveFlightsData();
    void saveCrewData();
    void savePassengersData();
    void saveReservationsData();
    
    // 输入验证辅助函数
    int getValidIntInput(const std::string& prompt, int min, int max);
    char getValidCharInput(const std::string& prompt);
    std::string getValidStringInput(const std::string& prompt);
    bool getValidYesNoInput(const std::string& prompt);
    void clearInputBuffer();
};

#endif // RESERVATION_SYSTEM_H
