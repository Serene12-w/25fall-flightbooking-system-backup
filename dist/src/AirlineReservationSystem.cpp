#include "AirlineReservationSystem.h"
#include "FlightPlan.h"
#include "AirlineData.h"
#include "DataLoader.h"
#include "FlightCalculator.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <queue>
#include <random>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

// 构造函数
AirlineReservationSystem::AirlineReservationSystem() : passengerTree(nullptr), currentPassenger(nullptr) {
    initializeData();
}

// 析构函数
AirlineReservationSystem::~AirlineReservationSystem() {
    // 释放内存
    for (auto aircraft : aircrafts) delete aircraft;
    for (auto flight : flights) delete flight;
    for (auto crew : crewMembers) delete crew;
    for (auto const& [key, val] : reservations) delete val;
    // 释放乘客树
    // (这里需要一个辅助函数来递归删除树节点)
}

// ==================== 输入验证辅助函数 ====================

// 清除输入缓冲区
void AirlineReservationSystem::clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// 获取有效的整数输入（带范围验证）
int AirlineReservationSystem::getValidIntInput(const std::string& prompt, int min, int max) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= min && value <= max) {
                clearInputBuffer(); // 清除换行符
                return value;
            } else {
                std::cout << "Invalid input! Please enter a number between " << min << " and " << max << "." << std::endl;
            }
        } else {
            std::cout << "Invalid input! Please enter a valid number." << std::endl;
            clearInputBuffer();
        }
    }
}

// 获取有效的字符输入
char AirlineReservationSystem::getValidCharInput(const std::string& prompt) {
    char value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInputBuffer(); // 清除换行符
            return value;
        } else {
            std::cout << "Invalid input! Please try again." << std::endl;
            clearInputBuffer();
        }
    }
}

// 获取有效的字符串输入（非空）
std::string AirlineReservationSystem::getValidStringInput(const std::string& prompt) {
    std::string value;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, value);
        if (!value.empty()) {
            return value;
        } else {
            std::cout << "Invalid input! Input cannot be empty." << std::endl;
        }
    }
}

// 获取有效的是/否输入
bool AirlineReservationSystem::getValidYesNoInput(const std::string& prompt) {
    char response;
    while (true) {
        std::cout << prompt;
        if (std::cin >> response) {
            clearInputBuffer(); // 清除换行符
            response = toupper(response);
            if (response == 'Y' || response == 'N') {
                return response == 'Y';
            } else {
                std::cout << "Invalid input! Please enter Y or N." << std::endl;
            }
        } else {
            std::cout << "Invalid input! Please try again." << std::endl;
            clearInputBuffer();
        }
    }
}

// ==================== 初始化数据 ====================

// 初始化数据
void AirlineReservationSystem::initializeData() {
    // 首先加载机场数据
    DataLoader::loadAirports("data/airports.csv", airports);
    
    // 使用 AirlineData 类初始化所有数据
    AirlineData::initializeAircrafts(aircrafts);
    
    // 为每种飞机设置性能参数(空重、燃油系数、巡航速度)
    for (auto* aircraft : aircrafts) {
        std::string type = aircraft->getType();
        
        // 根据飞机型号设置合理的性能参数
        if (type.find("A380") != std::string::npos) {
            aircraft->setEmptyWeight(276800);  // kg
            aircraft->setFuelCoefficient(16.5); // kg/km
            aircraft->setCruiseSpeed(900);      // km/h
        } else if (type.find("A350-1000") != std::string::npos) {
            aircraft->setEmptyWeight(193000);
            aircraft->setFuelCoefficient(11.8);
            aircraft->setCruiseSpeed(910);
        } else if (type.find("A350-900") != std::string::npos) {
            aircraft->setEmptyWeight(155200);
            aircraft->setFuelCoefficient(10.2);
            aircraft->setCruiseSpeed(910);
        } else if (type.find("A330-900") != std::string::npos) {
            aircraft->setEmptyWeight(132000);
            aircraft->setFuelCoefficient(9.8);
            aircraft->setCruiseSpeed(870);
        } else if (type.find("A330-300") != std::string::npos) {
            aircraft->setEmptyWeight(124500);
            aircraft->setFuelCoefficient(9.5);
            aircraft->setCruiseSpeed(870);
        } else if (type.find("A330-200") != std::string::npos) {
            aircraft->setEmptyWeight(120000);
            aircraft->setFuelCoefficient(9.2);
            aircraft->setCruiseSpeed(870);
        } else if (type.find("A321XLR") != std::string::npos) {
            aircraft->setEmptyWeight(50300);
            aircraft->setFuelCoefficient(4.2);
            aircraft->setCruiseSpeed(840);
        } else if (type.find("A321LR") != std::string::npos) {
            aircraft->setEmptyWeight(48500);
            aircraft->setFuelCoefficient(4.0);
            aircraft->setCruiseSpeed(840);
        } else if (type.find("A321neo") != std::string::npos) {
            aircraft->setEmptyWeight(48000);
            aircraft->setFuelCoefficient(3.8);
            aircraft->setCruiseSpeed(840);
        } else if (type.find("A321") != std::string::npos) {
            aircraft->setEmptyWeight(47900);
            aircraft->setFuelCoefficient(3.9);
            aircraft->setCruiseSpeed(840);
        } else if (type.find("A320neo") != std::string::npos) {
            aircraft->setEmptyWeight(42400);
            aircraft->setFuelCoefficient(3.5);
            aircraft->setCruiseSpeed(840);
        } else if (type.find("A320") != std::string::npos) {
            aircraft->setEmptyWeight(42600);
            aircraft->setFuelCoefficient(3.6);
            aircraft->setCruiseSpeed(840);
        } else if (type.find("A319") != std::string::npos) {
            aircraft->setEmptyWeight(40800);
            aircraft->setFuelCoefficient(3.4);
            aircraft->setCruiseSpeed(830);
        } else if (type.find("B787-10") != std::string::npos) {
            aircraft->setEmptyWeight(135800);
            aircraft->setFuelCoefficient(10.5);
            aircraft->setCruiseSpeed(903);
        } else if (type.find("B787-9") != std::string::npos) {
            aircraft->setEmptyWeight(129000);
            aircraft->setFuelCoefficient(9.8);
            aircraft->setCruiseSpeed(903);
        } else if (type.find("B787-8") != std::string::npos) {
            aircraft->setEmptyWeight(119950);
            aircraft->setFuelCoefficient(9.2);
            aircraft->setCruiseSpeed(903);
        } else if (type.find("B777-300ER") != std::string::npos) {
            aircraft->setEmptyWeight(167800);
            aircraft->setFuelCoefficient(12.5);
            aircraft->setCruiseSpeed(890);
        } else if (type.find("B777-200ER") != std::string::npos) {
            aircraft->setEmptyWeight(145150);
            aircraft->setFuelCoefficient(11.2);
            aircraft->setCruiseSpeed(890);
        } else if (type.find("B767-300ER") != std::string::npos) {
            aircraft->setEmptyWeight(90010);
            aircraft->setFuelCoefficient(8.5);
            aircraft->setCruiseSpeed(850);
        } else if (type.find("B737 MAX 9") != std::string::npos) {
            aircraft->setEmptyWeight(45900);
            aircraft->setFuelCoefficient(3.7);
            aircraft->setCruiseSpeed(839);
        } else if (type.find("B737 MAX 8") != std::string::npos) {
            aircraft->setEmptyWeight(45200);
            aircraft->setFuelCoefficient(3.6);
            aircraft->setCruiseSpeed(839);
        } else if (type.find("B737-800") != std::string::npos) {
            aircraft->setEmptyWeight(41413);
            aircraft->setFuelCoefficient(3.8);
            aircraft->setCruiseSpeed(828);
        } else {
            // 默认值(中型飞机)
            aircraft->setEmptyWeight(60000);
            aircraft->setFuelCoefficient(5.0);
            aircraft->setCruiseSpeed(850);
        }
    }
    
    AirlineData::initializeCrewMembers(crewMembers);
    AirlineData::initializeFlights(flights, aircrafts, crewMembers, airports);

    // --------- 预处理: 构建飞机型号 -> 机长/乘务员列表映射 ---------
    pilotsByAircraftType.clear();
    cabinCrewByAircraftType.clear();
    for (Crew* crew : crewMembers) {
        if (crew->getRole() == CrewRole::PILOT) {
            for (const auto& qual : crew->getQualifications()) {
                pilotsByAircraftType[qual].push_back(crew);
            }
        } else if (crew->getRole() == CrewRole::CABIN_CREW) {
            for (const auto& qual : crew->getQualifications()) {
                cabinCrewByAircraftType[qual].push_back(crew);
            }
        }
    }

    // --------- 推断并填充每个机组成员的上次分配时间 ---------
    // 将所有已存在航班的到达时间作为他们的上次分配时间的候选项
    auto parseDateTimeToTimeT = [](const std::string& date, const std::string& timeStr) -> std::time_t {
        // date: YYYY-MM-DD, timeStr: HH:MM or HH:MM+1
        int year = 0, month = 0, day = 0;
        if (date.size() >= 10) {
            year = std::stoi(date.substr(0,4));
            month = std::stoi(date.substr(5,2));
            day = std::stoi(date.substr(8,2));
        }

        std::string timeOnly = timeStr;
        int dayOffset = 0;
        size_t plusPos = timeStr.find('+');
        if (plusPos != std::string::npos) {
            timeOnly = timeStr.substr(0, plusPos);
            std::string offsetPart = timeStr.substr(plusPos + 1);
            try { dayOffset = std::stoi(offsetPart); } catch(...) { dayOffset = 0; }
        }

        int hour = 0, minute = 0;
        char colon;
        std::stringstream ss(timeOnly);
        ss >> hour >> colon >> minute;

        std::tm tm = {};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day + dayOffset;
        tm.tm_hour = hour;
        tm.tm_min = minute;
        tm.tm_sec = 0;

        std::time_t t = std::mktime(&tm);
        if (t == -1) return 0;
        return t;
    };

    for (Flight* f : flights) {
        const auto& assigned = f->getAssignedCrew();
        if (assigned.empty()) continue;
        std::time_t arrivalTs = parseDateTimeToTimeT(f->getFlightDate(), f->getArrivalTime());
        if (arrivalTs == 0) continue;
        for (Crew* c : assigned) {
            if (c->getLastAssigned() < arrivalTs) {
                c->setLastAssigned(arrivalTs);
            }
        }
    }

    // 加载乘客数据
    DataLoader::loadPassengers("data/passengers.csv", passengers);
    
    // 将加载的乘客添加到树中 (只添加到树,不重复添加到向量)
    for (auto* passenger : passengers) {
        PassengerNode* newNode = new PassengerNode(passenger);
        if (passengerTree == nullptr) {
            passengerTree = newNode;
        } else {
            PassengerNode* current = passengerTree;
            while (true) {
                if (passenger->getPassportNumber() < current->passenger->getPassportNumber()) {
                    if (current->left == nullptr) {
                        current->left = newNode;
                        break;
                    }
                    current = current->left;
                } else {
                    if (current->right == nullptr) {
                        current->right = newNode;
                        break;
                    }
                    current = current->right;
                }
            }
        }
    }
    
    // 加载预订数据 (这会自动恢复座位状态)
    DataLoader::loadReservations("data/reservations.csv", reservations, passengers, flights);

    // 加载订单数据
    orderManager.loadFromCSV("data/orders.csv");
    
    // 加载候补队列数据
    loadWaitlistFromCSV("data/waitlist.csv");

    // 构建航班图 (用于转机搜索)
    for (const auto& flight : flights) {
        flightGraph[flight->getOrigin()].push_back(flight);
    }
}

// 运行系统
void AirlineReservationSystem::run() {
    int choice;
    do {
        showMainMenu();
        choice = getValidIntInput("Enter your choice: ", 0, 2);
        switch (choice) {
            case 1:
                if (passengerLogin()) {
                    showPassengerMenu();
                }
                break;
            case 2:
                // 改为统一入口：进入管理员系统先展示登录/返回选择界面
                runAdminConsole();
                break;
            case 0:
                std::cout << "Thank you for using the airline reservation system. Goodbye!" << std::endl;
                break;
        }
    } while (choice != 0);
}

// 主菜单
void AirlineReservationSystem::showMainMenu() {
    std::cout << "\n--- Welcome to the Airline Reservation System ---" << std::endl;
    std::cout << "1. Passenger System" << std::endl;
    std::cout << "2. Administrator System" << std::endl;
    std::cout << "0. Exit" << std::endl;
}

// 乘客菜单
void AirlineReservationSystem::showPassengerMenu() {
    int choice;
    do {
        std::cout << "\n--- Passenger Menu ---" << std::endl;
        std::cout << "1. Search and Book Flight" << std::endl;
        std::cout << "2. Pay for Ticket" << std::endl;
        std::cout << "3. Check In" << std::endl;
        std::cout << "4. View My Orders" << std::endl;
        std::cout << "5. Cancel Order" << std::endl;
        std::cout << "6. Search for Flights" << std::endl;
        std::cout << "7. My Profile" << std::endl;
        std::cout << "8. Return to Mode Selection" << std::endl;
        std::cout << "0. Back to Main Menu" << std::endl;
        choice = getValidIntInput("Enter your choice: ", 0, 8);
        switch (choice) {
            case 1: bookTicket(); break;
            case 2: payForTicket(); break;
            case 3: checkIn(); break;
            case 4: viewMyReservations(); break;
            case 5: cancelReservation(); break;
            case 6: searchFlights(); break;
            case 7: viewPassengerProfile(); break;
            case 8: {
                // 启动新进程回到模式选择窗口
                char exePath[MAX_PATH] = {0};
                #ifdef _WIN32
                GetModuleFileNameA(NULL, exePath, MAX_PATH);
                STARTUPINFOA si; ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si);
                PROCESS_INFORMATION pi; ZeroMemory(&pi, sizeof(pi));
                if (CreateProcessA(exePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                }
                #endif
                return; // 直接返回，当前控制台 passenger 菜单结束
            }
            case 0: break;
        }
    } while (choice != 0);
}

// 管理员菜单
void AirlineReservationSystem::showAdminMenu() {
    int choice;
    do {
        std::cout << "\n--- Administrator Menu ---" << std::endl;
        std::cout << "1. Manage aircraft models" << std::endl;
        std::cout << "2. Manage flights" << std::endl;
        std::cout << "3. Manage crew" << std::endl;
        std::cout << "4. View passenger list" << std::endl;
        std::cout << "5. View flight status" << std::endl;
        std::cout << "6. Generate Flight Briefing" << std::endl;
        std::cout << "7. Generate Flight Plan for Crew" << std::endl;
        std::cout << "8. Schedule New Flight (Auto)" << std::endl;
        std::cout << "9. View Waitlist" << std::endl;
        std::cout << "10. Return to Mode Selection" << std::endl;
        std::cout << "0. Back to Main Menu" << std::endl;
        choice = getValidIntInput("Enter your choice: ", 0, 10);
        switch (choice) {
            case 1: manageAircrafts(); break;
            case 2: manageFlights(); break;
            case 3: manageCrew(); break;
            case 4: viewPassengerList(); break;
            case 5: viewFlightStatus(); break;
            case 6: generateFlightBriefing(); break;
            case 7: generateFlightPlan(); break;
            case 8: scheduleNewFlight(); break;
            case 9: viewWaitlist(); break;
            case 10: {
                char exePath[MAX_PATH] = {0};
                #ifdef _WIN32
                GetModuleFileNameA(NULL, exePath, MAX_PATH);
                STARTUPINFOA si; ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si);
                PROCESS_INFORMATION pi; ZeroMemory(&pi, sizeof(pi));
                // 传入参数以使选择窗口置顶显示
                std::string cmd = std::string("\"") + exePath + "\" -chooser-topmost";
                char* cmdBuf = cmd.empty() ? NULL : _strdup(cmd.c_str());
                if (CreateProcessA(exePath, cmdBuf, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                }
                if (cmdBuf) free(cmdBuf);
                // 关闭当前控制台并退出当前进程
                FreeConsole();
                ExitProcess(0);
                #endif
                return; // 非 Win32 情况
            }
            case 0: break;
        }
    } while (choice != 0);
}

// 乘客登录
bool AirlineReservationSystem::passengerLogin() {
    std::cout << "\n========== Passenger Login / Registration ==========" << std::endl;
    std::cout << "1. Login with existing passport number" << std::endl;
    std::cout << "2. Register as new passenger" << std::endl;
    
    int choice = getValidIntInput("Enter your choice: ", 1, 2);
    
    if (choice == 1) {
        // 现有乘客登录
        std::string passportNumber = getValidStringInput("\nEnter your passport number: ");
        
        currentPassenger = findPassenger(passportNumber);
        if (currentPassenger == nullptr) {
            std::cout << "Passenger not found. Please register first." << std::endl;
            return false;
        } else {
            std::cout << "Welcome back, " << currentPassenger->getName() << "!" << std::endl;
            return true;
        }
    } else {
        // 新乘客注册
        std::string name, passportNumber;
        
        std::cout << "\n=== New Passenger Registration ===" << std::endl;
        name = getValidStringInput("Enter your name: ");
        passportNumber = getValidStringInput("Enter your passport number: ");
        
        // 检查护照号是否已存在
        if (findPassenger(passportNumber) != nullptr) {
            std::cout << "This passport number is already registered. Please login instead." << std::endl;
            return false;
        }
        
        // 创建新乘客
        currentPassenger = new Passenger(name, passportNumber);
        addPassenger(currentPassenger);
        std::cout << "\nRegistration successful!" << std::endl;
        std::cout << "Welcome, " << name << "!" << std::endl;
        std::cout << "Your passport number: " << passportNumber << std::endl;
        
        // 保存乘客数据到CSV
        savePassengersData();
        return true;
    }
}

// 管理员登录
bool AirlineReservationSystem::adminLogin() {
    std::string username = getValidStringInput("Enter administrator username: ");
    std::string password = getValidStringInput("Enter administrator password: ");
    if (username == "admin" && password == "admin123") {
        std::cout << "Administrator login successful." << std::endl;
        return true;
    } else {
        std::cout << "Invalid username or password." << std::endl;
        return false;
    }
}

// 预订机票
// 预订机票 (步骤1: 创建订单,状态为待付款)
void AirlineReservationSystem::bookTicket() {
    if (!currentPassenger) {
        std::cout << "Error: Please login first." << std::endl;
        return;
    }

    std::string origin, destination, travelDate;
    std::cout << "\n========== Book Flight ==========\n";
    origin = getValidStringInput("Enter origin airport code (e.g., PEK): ");
    destination = getValidStringInput("Enter destination airport code (e.g., LAX): ");
    travelDate = getValidStringInput("Enter travel date (YYYY-MM-DD): ");

    // 查找直飞航班
    std::vector<Flight*> directFlights;
    for (const auto& flight : flights) {
        if (flight->getOrigin() == origin && 
            flight->getDestination() == destination &&
            flight->getFlightDate() == travelDate) {
            directFlights.push_back(flight);
        }
    }

    if (!directFlights.empty()) {
        // 有直飞航班
        std::cout << "\n--- Available Direct Flights ---" << std::endl;
        for (size_t i = 0; i < directFlights.size(); ++i) {
            Flight* flight = directFlights[i];
            std::cout << (i + 1) << ". Flight " << flight->getFlightNumber() 
                      << " | " << flight->getOrigin() << " -> " << flight->getDestination()
                      << " | Date: " << flight->getFlightDate()
                      << " | Departs: " << flight->getDepartureTime()
                      << " | Arrives: " << flight->getArrivalTime()
                      << " | Aircraft: " << flight->getAircraft()->getType() << std::endl;
        }

        // 选择航班
        int flightChoice = getValidIntInput("\nSelect flight number (1-" + std::to_string(directFlights.size()) + ", 0 to search connecting flights): ", 0, directFlights.size());

        if (flightChoice == 0) {
            // 用户选择查看转机航班
            std::cout << "\nSearching for connecting flights...\n";
        } else {
            Flight* selectedFlight = directFlights[flightChoice - 1];

            // 选择仓位等级
            std::cout << "\n--- Select Cabin Class ---" << std::endl;
            std::cout << "1. First Class" << std::endl;
            std::cout << "2. Business Class" << std::endl;
            std::cout << "3. Premium Economy" << std::endl;
            std::cout << "4. Economy Class" << std::endl;
            
            int cabinChoice = getValidIntInput("Enter your choice (1-4): ", 1, 4);
            
            SeatType selectedSeatType;
            switch (cabinChoice) {
                case 1: selectedSeatType = SeatType::FIRST_CLASS; break;
                case 2: selectedSeatType = SeatType::BUSINESS_CLASS; break;
                case 3: selectedSeatType = SeatType::PREMIUM_ECONOMY; break;
                case 4: selectedSeatType = SeatType::ECONOMY_CLASS; break;
                default:
                    std::cout << "Invalid choice. Defaulting to Economy Class." << std::endl;
                    selectedSeatType = SeatType::ECONOMY_CLASS;
            }

            // 检查是否有空座
            if (!hasAvailableSeats(selectedFlight, selectedSeatType)) {
                std::cout << "\n========== Cabin Full ==========\n";
                std::cout << "Sorry, " << seatTypeToString(selectedSeatType) 
                          << " is fully booked for this flight." << std::endl;
                std::cout << "\nWould you like to:" << std::endl;
                std::cout << "1. Join the waitlist (priority based on loyalty status)" << std::endl;
                std::cout << "2. Select a different cabin class" << std::endl;
                std::cout << "3. Cancel" << std::endl;
                
                int waitlistChoice = getValidIntInput("Enter your choice (1-3): ", 1, 3);
                
                if (waitlistChoice == 1) {
                    // 加入候补队列
                    addToWaitlist(currentPassenger, selectedFlight, selectedSeatType);
                    return;
                } else if (waitlistChoice == 2) {
                    // 回到舱位选择
                    std::cout << "\nPlease try again with a different cabin class." << std::endl;
                    return;
                } else {
                    std::cout << "Booking cancelled." << std::endl;
                    return;
                }
            }

            // 创建订单 (待付款状态)
            Order* order = orderManager.createOrder(
                currentPassenger->getPassportNumber(),
                selectedFlight->getFlightNumber(),
                travelDate,
                selectedSeatType
            );

            // 计算票价
            double basePrice = calculateBasePrice(selectedFlight, selectedSeatType);
            double discount = getLoyaltyDiscount(currentPassenger->getLoyaltyStatus());
            double finalPrice = calculateFinalPrice(selectedFlight, selectedSeatType, currentPassenger->getLoyaltyStatus());
            
            // 设置订单票价
            order->setTicketPrice(finalPrice);

            std::cout << "\n========== Booking Successful! ==========" << std::endl;
            std::cout << "Order Number: " << order->getOrderNumber() << std::endl;
            std::cout << "Flight: " << selectedFlight->getFlightNumber() << std::endl;
            std::cout << "Route: " << selectedFlight->getOrigin() << " -> " << selectedFlight->getDestination() << std::endl;
            std::cout << "Date: " << travelDate << std::endl;
            std::cout << "Cabin: " << Order::seatTypeToString(selectedSeatType) << std::endl;
            
            // 显示价格信息
            std::cout << "\n--- Ticket Price ---" << std::endl;
            std::cout << "Base Price: $" << std::fixed << std::setprecision(2) << basePrice << std::endl;
            if (discount > 0) {
                std::cout << "Member Discount: " << (discount * 100) << "% OFF" << std::endl;
                std::cout << "Discount Amount: $" << (basePrice * discount) << std::endl;
            }
            std::cout << "Final Price: $" << finalPrice << std::endl;
            
            std::cout << "\nStatus: PENDING PAYMENT" << std::endl;
            std::cout << "\nPlease proceed to payment (Option 2 in menu) to complete booking." << std::endl;
            std::cout << "You can cancel this order anytime before payment." << std::endl;

            // 保存订单数据
            orderManager.saveToCSV("data/orders.csv");
            return;
        }
    } else {
        std::cout << "\nNo direct flights found between " << origin << " and " << destination 
                  << " on " << travelDate << std::endl;
        std::cout << "Searching for connecting flights...\n";
    }

    // 搜索转机航班
    std::vector<ConnectingFlightRoute> connectingRoutes = findConnectingFlights(origin, destination, travelDate);
    
    if (connectingRoutes.empty()) {
        std::cout << "No connecting flights found." << std::endl;
        return;
    }

    // 显示转机航班选项
    std::cout << "\n========== Available Connecting Flights ==========\n";
    for (size_t i = 0; i < connectingRoutes.size() && i < 5; ++i) {
        displayConnectingRoute(connectingRoutes[i], i + 1);
    }

    // 让用户选择路线
    int routeChoice = getValidIntInput("\nSelect route number (1-" + std::to_string(std::min<size_t>(connectingRoutes.size(), 5)) + ", 0 to cancel): ", 
                                    0, static_cast<int>(std::min<size_t>(connectingRoutes.size(), 5)));

    if (routeChoice == 0) {
        std::cout << "Booking cancelled." << std::endl;
        return;
    }

    ConnectingFlightRoute selectedRoute = connectingRoutes[routeChoice - 1];

    // 选择仓位等级
    std::cout << "\n--- Select Cabin Class ---" << std::endl;
    std::cout << "1. First Class" << std::endl;
    std::cout << "2. Business Class" << std::endl;
    std::cout << "3. Premium Economy" << std::endl;
    std::cout << "4. Economy Class" << std::endl;
    
    int cabinChoice = getValidIntInput("Enter your choice (1-4): ", 1, 4);
    
    SeatType selectedSeatType;
    switch (cabinChoice) {
        case 1: selectedSeatType = SeatType::FIRST_CLASS; break;
        case 2: selectedSeatType = SeatType::BUSINESS_CLASS; break;
        case 3: selectedSeatType = SeatType::PREMIUM_ECONOMY; break;
        case 4: selectedSeatType = SeatType::ECONOMY_CLASS; break;
        default:
            std::cout << "Invalid choice. Defaulting to Economy Class." << std::endl;
            selectedSeatType = SeatType::ECONOMY_CLASS;
    }

    // 预订联程航班
    bookConnectingFlight(selectedRoute, selectedSeatType);
}

// 付款出票 (步骤2: 输入信用卡付款,状态改为已付款)
void AirlineReservationSystem::payForTicket() {
    if (!currentPassenger) {
        std::cout << "Error: Please login first." << std::endl;
        return;
    }

    // 获取当前乘客的待付款订单
    std::vector<Order*> pendingOrders = orderManager.getOrdersByPassport(currentPassenger->getPassportNumber());
    std::vector<Order*> unpaidOrders;
    
    for (Order* order : pendingOrders) {
        if (order->getStatus() == OrderStatus::PENDING_PAYMENT) {
            unpaidOrders.push_back(order);
        }
    }

    if (unpaidOrders.empty()) {
        std::cout << "\nNo unpaid orders found. Please book a flight first." << std::endl;
        return;
    }

    // 显示待付款订单
    std::cout << "\n========== Your Unpaid Orders ==========\n";
    for (size_t i = 0; i < unpaidOrders.size(); ++i) {
        Order* order = unpaidOrders[i];
        std::cout << (i + 1) << ". Order: " << order->getOrderNumber();
        
        if (order->isConnectingFlight()) {
            std::cout << " | Type: CONNECTING FLIGHT";
            std::vector<std::string> flightNumbers = order->getConnectingFlightNumbers();
            std::cout << " (" << flightNumbers.size() << " segments)";
        } else {
            std::cout << " | Flight: " << order->getFlightNumber();
        }
        
        std::cout << " | Date: " << order->getFlightDate()
                  << " | Price: $" << std::fixed << std::setprecision(2) << order->getTicketPrice()
                  << " | Created: " << order->getCreateTime() << std::endl;
    }

    // 选择订单
    int choice = getValidIntInput("\nSelect order to pay (1-" + std::to_string(unpaidOrders.size()) + "): ", 1, unpaidOrders.size());

    Order* selectedOrder = unpaidOrders[choice - 1];

    // 输入信用卡信息
    std::string creditCard;
    std::cout << "\n========== Payment Information ==========\n";
    creditCard = getValidStringInput("Enter credit card number (16 digits): ");

    if (creditCard.length() < 4) {
        std::cout << "Invalid credit card number." << std::endl;
        return;
    }

    // 模拟支付处理
    std::cout << "\nProcessing payment..." << std::endl;
    std::cout << "Payment successful!" << std::endl;

    // 更新订单状态
    selectedOrder->setCreditCard(creditCard);
    selectedOrder->setStatus(OrderStatus::PAID);
    
    // 获取当前时间
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    selectedOrder->setPaymentTime(oss.str());

    std::cout << "\n========== Ticket Issued ==========\n";
    std::cout << "Order Number: " << selectedOrder->getOrderNumber() << std::endl;
    
    if (selectedOrder->isConnectingFlight()) {
        std::cout << "Type: CONNECTING FLIGHT" << std::endl;
        std::vector<std::string> flightNumbers = selectedOrder->getConnectingFlightNumbers();
        std::cout << "Flight Segments:" << std::endl;
        for (size_t i = 0; i < flightNumbers.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << flightNumbers[i] << std::endl;
        }
    } else {
        std::cout << "Flight: " << selectedOrder->getFlightNumber() << std::endl;
    }
    
    std::cout << "Date: " << selectedOrder->getFlightDate() << std::endl;
    std::cout << "Amount Paid: $" << std::fixed << std::setprecision(2) << selectedOrder->getTicketPrice() << std::endl;
    std::cout << "Status: PAID" << std::endl;
    std::cout << "Card: ****" << selectedOrder->getCreditCardLast4() << std::endl;
    std::cout << "\n*** This ticket is NON-REFUNDABLE ***" << std::endl;
    std::cout << "Please proceed to check-in (Option 3) to select your seat." << std::endl;

    // 保存订单
    orderManager.saveToCSV("data/orders.csv");
}

// 值机选座 (步骤3: 选择座位,状态改为已值机)
void AirlineReservationSystem::checkIn() {
    if (!currentPassenger) {
        std::cout << "Error: Please login first." << std::endl;
        return;
    }

    // 获取当前乘客的已付款订单
    std::vector<Order*> paidOrders = orderManager.getOrdersByPassport(currentPassenger->getPassportNumber());
    std::vector<Order*> readyForCheckIn;
    
    for (Order* order : paidOrders) {
        if (order->getStatus() == OrderStatus::PAID) {
            readyForCheckIn.push_back(order);
        }
    }

    if (readyForCheckIn.empty()) {
        std::cout << "\nNo paid orders ready for check-in. Please pay for your ticket first." << std::endl;
        return;
    }

    // 显示可值机订单
    std::cout << "\n========== Orders Ready for Check-In ==========\n";
    for (size_t i = 0; i < readyForCheckIn.size(); ++i) {
        Order* order = readyForCheckIn[i];
        std::cout << (i + 1) << ". Order: " << order->getOrderNumber();
        
        if (order->isConnectingFlight()) {
            std::cout << " | Type: CONNECTING FLIGHT";
            std::vector<std::string> flightNumbers = order->getConnectingFlightNumbers();
            std::cout << " (" << flightNumbers.size() << " segments)";
        } else {
            std::cout << " | Flight: " << order->getFlightNumber();
        }
        std::cout << " | Date: " << order->getFlightDate() << std::endl;
    }

    // 选择订单
    int choice = getValidIntInput("\nSelect order for check-in (1-" + std::to_string(readyForCheckIn.size()) + "): ", 1, readyForCheckIn.size());

    Order* selectedOrder = readyForCheckIn[choice - 1];
    
    // 处理联程航班值机
    if (selectedOrder->isConnectingFlight()) {
        std::vector<std::string> flightNumbers = selectedOrder->getConnectingFlightNumbers();
        std::vector<std::string> seatNumbers;
        
        std::cout << "\n========== Check-In for Connecting Flights ==========\n";
        std::cout << "You will now check in for all " << flightNumbers.size() << " flight segments.\n";
        
        for (size_t i = 0; i < flightNumbers.size(); ++i) {
            Flight* flight = findFlight(flightNumbers[i]);
            
            if (!flight) {
                std::cout << "Error: Flight " << flightNumbers[i] << " not found." << std::endl;
                return;
            }
            
            std::cout << "\n--- Segment " << (i + 1) << ": Flight " << flightNumbers[i] 
                      << " (" << flight->getOrigin() << " -> " << flight->getDestination() << ") ---\n";
            
            // 检查飞机是否有用户选择的舱位
            Aircraft* aircraft = flight->getAircraft();
            SeatType requestedClass = selectedOrder->getSeatType();
            SeatType actualClass = requestedClass;
            
            if (!aircraft->hasCabinClass(requestedClass)) {
                // 需要降级
                actualClass = getDowngradedCabinClass(requestedClass, aircraft);
                std::cout << "Note: This aircraft (" << aircraft->getType() 
                          << ") does not have " << seatTypeToString(requestedClass) << ".\n";
                std::cout << "      Your seat will be in " << seatTypeToString(actualClass) << " instead.\n";
            }
            
            // 显示座位图
            std::cout << "Cabin class: " << seatTypeToString(actualClass) << std::endl;
            flight->getAircraft()->displaySeatMap();
            
            // 选择座位
            int row = getValidIntInput("\nEnter seat row number: ", 1, 100);
            char col = getValidCharInput("Enter seat column (A-J): ");
            col = toupper(col);
            
            Seat* seat = flight->getAircraft()->getSeat(row, col);
            if (!seat) {
                std::cout << "Invalid seat selection." << std::endl;
                return;
            }
            
            if (seat->isBooked()) {
                std::cout << "This seat is already occupied. Please choose another seat." << std::endl;
                return;
            }
            
            // 验证座位类型 - 使用实际舱位而非订单舱位
            if (seat->getType() != actualClass) {
                std::cout << "Error: This seat does not match your assigned cabin class." << std::endl;
                std::cout << "Your assigned cabin: " << seatTypeToString(actualClass) << std::endl;
                std::cout << "Selected seat type: " << seatTypeToString(seat->getType()) << std::endl;
                return;
            }
            
            // 预订座位
            seat->book();
            
            // 生成座位号字符串
            std::ostringstream seatStr;
            seatStr << row << col;
            seatNumbers.push_back(seatStr.str());
            
            // 创建预订记录
            std::string reservationNumber = selectedOrder->getOrderNumber() + "_SEG" + std::to_string(i + 1);
            Reservation* reservation = new Reservation(reservationNumber, selectedOrder->getOrderNumber(),
                                                      currentPassenger, flight, seat);
            reservation->confirm();
            reservations[reservationNumber] = reservation;
        }
        
        // 更新订单
        selectedOrder->setConnectingFlights(flightNumbers, seatNumbers);
        selectedOrder->setSeatNumber(seatNumbers[0]); // 第一段座位作为主座位号
        selectedOrder->setStatus(OrderStatus::CHECKED_IN);
        
        // 设置值机时间
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        std::ostringstream timeStr;
        timeStr << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        selectedOrder->setCheckInTime(timeStr.str());
        
        std::cout << "\n========== Check-In Successful! ==========\n";
        std::cout << "Order Number: " << selectedOrder->getOrderNumber() << std::endl;
        std::cout << "Type: CONNECTING FLIGHT (" << flightNumbers.size() << " segments)" << std::endl;
        std::cout << "\nSeats Assigned:" << std::endl;
        for (size_t i = 0; i < flightNumbers.size(); ++i) {
            std::cout << "  Segment " << (i + 1) << " (" << flightNumbers[i] << "): " 
                      << seatNumbers[i] << std::endl;
        }
        std::cout << "Status: CHECKED IN" << std::endl;
        std::cout << "\nYou are ready to board all segments. Have a pleasant journey!" << std::endl;
        
    } else {
        // 处理单程航班值机（原有逻辑）
        Flight* flight = findFlight(selectedOrder->getFlightNumber());
        
        if (!flight) {
            std::cout << "Error: Flight not found." << std::endl;
            return;
        }
        
        // 显示座位图
        std::cout << "\n========== Seat Selection ==========\n";
        std::cout << "Your cabin class: " << Order::seatTypeToString(selectedOrder->getSeatType()) << std::endl;
        flight->getAircraft()->displaySeatMap();
        
        // 选择座位
        int row = getValidIntInput("\nEnter seat row number: ", 1, 100);
        char col = getValidCharInput("Enter seat column (A-J): ");
        col = toupper(col);
        
        Seat* seat = flight->getAircraft()->getSeat(row, col);
        if (!seat) {
            std::cout << "Invalid seat selection." << std::endl;
            return;
        }
        
        if (seat->isBooked()) {
            std::cout << "This seat is already occupied. Please choose another seat." << std::endl;
            return;
        }
        
        // 验证座位类型是否匹配订单的仓位等级
        if (seat->getType() != selectedOrder->getSeatType()) {
            std::cout << "Error: This seat does not match your cabin class." << std::endl;
            std::cout << "Your cabin: " << Order::seatTypeToString(selectedOrder->getSeatType()) << std::endl;
            std::cout << "Seat type: " << Order::seatTypeToString(seat->getType()) << std::endl;
            return;
        }
        
        // 预订座位
        seat->book();
        
        // 生成座位号字符串
        std::ostringstream seatStr;
        seatStr << row << col;
        selectedOrder->setSeatNumber(seatStr.str());
        selectedOrder->setStatus(OrderStatus::CHECKED_IN);
        
        // 获取当前时间
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        std::ostringstream timeStr;
        timeStr << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        selectedOrder->setCheckInTime(timeStr.str());
        
        // 创建或更新预订记录
        std::string reservationNumber = selectedOrder->getOrderNumber(); // 使用订单号作为预订号
        Reservation* reservation = new Reservation(reservationNumber, selectedOrder->getOrderNumber(),
                                                  currentPassenger, flight, seat);
        reservation->confirm();
        reservations[reservationNumber] = reservation;
        
        std::cout << "\n========== Check-In Successful! ==========\n";
        std::cout << "Order Number: " << selectedOrder->getOrderNumber() << std::endl;
        std::cout << "Flight: " << selectedOrder->getFlightNumber() << std::endl;
        std::cout << "Seat: " << row << col << std::endl;
        std::cout << "Status: CHECKED IN" << std::endl;
        std::cout << "\nYou are ready to board. Have a pleasant flight!" << std::endl;
    }

    // 保存数据
    orderManager.saveToCSV("data/orders.csv");
    saveReservationsData();
}

// ===== (Removed duplicate UI Public API Wrappers – kept original implementations later in file) =====

// 取消预订
void AirlineReservationSystem::cancelReservation() {
    if (!currentPassenger) {
        std::cout << "Error: Please login first." << std::endl;
        return;
    }

    // 获取当前乘客的订单
    std::vector<Order*> myOrders = orderManager.getOrdersByPassport(currentPassenger->getPassportNumber());
    std::vector<Order*> cancellableOrders;
    
    for (Order* order : myOrders) {
        // 只有待付款的订单可以取消
        if (order->getStatus() == OrderStatus::PENDING_PAYMENT) {
            cancellableOrders.push_back(order);
        }
    }

    if (cancellableOrders.empty()) {
        std::cout << "\nNo cancellable orders found." << std::endl;
        std::cout << "Note: Only unpaid orders can be cancelled. Paid tickets are non-refundable." << std::endl;
        return;
    }

    // 显示可取消订单
    std::cout << "\n========== Cancellable Orders ==========\n";
    for (size_t i = 0; i < cancellableOrders.size(); ++i) {
        Order* order = cancellableOrders[i];
        std::cout << (i + 1) << ". Order: " << order->getOrderNumber();
        
        if (order->isConnectingFlight()) {
            std::cout << " | Type: CONNECTING FLIGHT";
            std::vector<std::string> flightNumbers = order->getConnectingFlightNumbers();
            std::cout << " (" << flightNumbers.size() << " segments)";
        } else {
            std::cout << " | Flight: " << order->getFlightNumber();
        }
        
        std::cout << " | Date: " << order->getFlightDate()
                  << " | Status: PENDING PAYMENT" << std::endl;
    }

    // 选择要取消的订单
    int choice = getValidIntInput("\nSelect order to cancel (1-" + std::to_string(cancellableOrders.size()) + ", 0 to go back): ", 
                                   0, cancellableOrders.size());

    if (choice == 0) return;

    Order* selectedOrder = cancellableOrders[choice - 1];

    // 确认取消
    bool confirm = getValidYesNoInput("Are you sure you want to cancel order " + selectedOrder->getOrderNumber() + "? (Y/N): ");

    if (confirm) {
        // 获取航班和舱位信息，用于处理候补
        std::string flightNumber = selectedOrder->getFlightNumber();
        std::string flightDate = selectedOrder->getFlightDate();
        SeatType seatType = selectedOrder->getSeatType();
        std::string seatNumber = selectedOrder->getSeatNumber();
        
        // 查找对应的航班
        Flight* relatedFlight = nullptr;
        for (auto* flight : flights) {
            if (flight->getFlightNumber() == flightNumber && 
                flight->getFlightDate() == flightDate) {
                relatedFlight = flight;
                break;
            }
        }
        
        // 如果是已值机的订单（有座位号），需要释放座位
        if (selectedOrder->getStatus() == OrderStatus::CHECKED_IN && !seatNumber.empty() && relatedFlight) {
            Aircraft* aircraft = relatedFlight->getAircraft();
            if (aircraft) {
                // 解析座位号 (例如: "1A")
                if (seatNumber.length() >= 2) {
                    int row = std::stoi(seatNumber.substr(0, seatNumber.length() - 1));
                    char col = seatNumber.back();
                    
                    // 释放座位
                    Seat* seat = aircraft->getSeat(row, col);
                    if (seat && seat->isBooked()) {
                        seat->unbook();
                        std::cout << "Seat " << seatNumber << " has been released." << std::endl;
                    }
                }
            }
        }
        
        // 如果是联程航班，需要释放所有航段的座位
        if (selectedOrder->isConnectingFlight()) {
            std::vector<std::string> flightNumbers = selectedOrder->getConnectingFlightNumbers();
            std::vector<std::string> seatNumbers = selectedOrder->getConnectingSeatNumbers();
            
            for (size_t i = 0; i < flightNumbers.size() && i < seatNumbers.size(); ++i) {
                if (seatNumbers[i].empty()) continue;
                
                // 查找对应的航班
                Flight* flight = nullptr;
                for (auto* f : flights) {
                    if (f->getFlightNumber() == flightNumbers[i] && f->getFlightDate() == flightDate) {
                        flight = f;
                        break;
                    }
                }
                
                if (flight) {
                    Aircraft* aircraft = flight->getAircraft();
                    if (aircraft && seatNumbers[i].length() >= 2) {
                        int row = std::stoi(seatNumbers[i].substr(0, seatNumbers[i].length() - 1));
                        char col = seatNumbers[i].back();
                        
                        Seat* seat = aircraft->getSeat(row, col);
                        if (seat && seat->isBooked()) {
                            seat->unbook();
                            std::cout << "Seat " << seatNumbers[i] << " on flight " << flightNumbers[i] 
                                      << " has been released." << std::endl;
                        }
                    }
                }
            }
        }
        
        // 取消订单
        selectedOrder->setStatus(OrderStatus::CANCELLED);
        std::cout << "\nOrder " << selectedOrder->getOrderNumber() << " has been cancelled successfully." << std::endl;
        
        // 保存订单和预订数据
        orderManager.saveToCSV("data/orders.csv");
        saveReservationsData();
        
        // 如果找到了对应的航班，处理候补队列
        if (relatedFlight) {
            std::cout << "\nChecking waitlist for available upgrades..." << std::endl;
            processWaitlist(relatedFlight, seatType);
        }
    } else {
        std::cout << "Cancellation aborted." << std::endl;
    }
}

// 确认预订 (出票)
void AirlineReservationSystem::confirmReservation() {
    std::string reservationNumber;
    std::cout << "Enter your reservation number to confirm: ";
    std::cin >> reservationNumber;

    auto it = reservations.find(reservationNumber);
    if (it != reservations.end()) {
        Reservation* reservation = it->second;
        if (reservation->getStatus() == ReservationStatus::PENDING) {
            reservation->confirm();
            std::cout << "Reservation " << reservationNumber << " has been confirmed." << std::endl;
            
            // 保存更改到CSV
            saveReservationsData();
        } else {
            std::cout << "This reservation cannot be confirmed (current status: " << (int)reservation->getStatus() << ")." << std::endl;
        }
    } else {
        std::cout << "Reservation not found." << std::endl;
    }
}

// 更改预订
void AirlineReservationSystem::changeReservation() {
    std::cout << "Function not implemented yet." << std::endl;
}

// 搜索航班
void AirlineReservationSystem::searchFlights() {
    std::cout << "\n--- All Flights ---" << std::endl;
    for (const auto& flight : flights) {
        std::cout << flight->getFlightNumber() << " | "
                  << flight->getOrigin() << " -> " << flight->getDestination() << " | "
                  << "Date: " << flight->getFlightDate() << " | "
                  << flight->getDepartureTime() << " - " << flight->getArrivalTime() << " | "
                  << flight->getAircraft()->getType() << std::endl;
    }
}

// 查看我的预订/订单
void AirlineReservationSystem::viewMyReservations() {
    if (!currentPassenger) {
        std::cout << "Error: Please login first." << std::endl;
        return;
    }

    std::string passportNumber = currentPassenger->getPassportNumber();
    std::vector<Order*> myOrders = orderManager.getOrdersByPassport(passportNumber);

    if (myOrders.empty()) {
        std::cout << "\nYou don't have any orders yet." << std::endl;
        return;
    }

    std::cout << "\n==================== My Orders ====================\n";
    for (Order* order : myOrders) {
        std::cout << "\nOrder Number: " << order->getOrderNumber() << std::endl;
        
        // 检查是否为联程航班
        if (order->isConnectingFlight()) {
            std::cout << "Type: CONNECTING FLIGHT" << std::endl;
            std::vector<std::string> flightNumbers = order->getConnectingFlightNumbers();
            std::cout << "Flight Segments (" << flightNumbers.size() << " total):" << std::endl;
            for (size_t i = 0; i < flightNumbers.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << flightNumbers[i];
                // 查找航班以显示路线
                for (Flight* f : flights) {
                    if (f->getFlightNumber() == flightNumbers[i]) {
                        std::cout << " (" << f->getOrigin() << " -> " << f->getDestination() << ")";
                        break;
                    }
                }
                std::cout << std::endl;
            }
        } else {
            std::cout << "Flight: " << order->getFlightNumber() << std::endl;
        }
        
        std::cout << "Date: " << order->getFlightDate() << std::endl;
        std::cout << "Cabin: " << Order::seatTypeToString(order->getSeatType()) << std::endl;
        
        if (order->isConnectingFlight()) {
            std::vector<std::string> seats = order->getConnectingSeatNumbers();
            std::cout << "Seats: ";
            bool hasSeats = false;
            for (const auto& seat : seats) {
                if (!seat.empty()) {
                    std::cout << seat << " ";
                    hasSeats = true;
                }
            }
            if (!hasSeats) {
                std::cout << "Not selected";
            }
            std::cout << std::endl;
        } else {
            std::cout << "Seat: " << (order->getSeatNumber().empty() ? "Not selected" : order->getSeatNumber()) << std::endl;
        }
        
        std::cout << "Price: $" << std::fixed << std::setprecision(2) << order->getTicketPrice() << std::endl;
        
        // 显示状态
        std::cout << "Status: ";
        switch (order->getStatus()) {
            case OrderStatus::PENDING_PAYMENT:
                std::cout << "PENDING PAYMENT" << std::endl;
                break;
            case OrderStatus::PAID:
                std::cout << "PAID (Need check-in)" << std::endl;
                if (!order->getPaymentTime().empty()) {
                    std::cout << "Payment Time: " << order->getPaymentTime() << std::endl;
                    std::cout << "Card: ****" << order->getCreditCardLast4() << std::endl;
                }
                break;
            case OrderStatus::CHECKED_IN:
                std::cout << "CHECKED IN" << std::endl;
                if (!order->getCheckInTime().empty()) {
                    std::cout << "Check-In Time: " << order->getCheckInTime() << std::endl;
                }
                break;
            case OrderStatus::CANCELLED:
                std::cout << "CANCELLED" << std::endl;
                break;
        }
        
        std::cout << "Created: " << order->getCreateTime() << std::endl;
        std::cout << "---------------------------------------------------" << std::endl;
    }
}

// 查看乘客个人信息和会员权益
void AirlineReservationSystem::viewPassengerProfile() {
    if (!currentPassenger) {
        std::cout << "Error: Please login first." << std::endl;
        return;
    }

    std::cout << "\n==================== My Profile ====================\n";
    std::cout << "Name: " << currentPassenger->getName() << std::endl;
    std::cout << "Passport Number: " << currentPassenger->getPassportNumber() << std::endl;
    
    // 显示会员等级
    std::cout << "\nMembership Tier: ";
    LoyaltyStatus status = currentPassenger->getLoyaltyStatus();
    std::string tierName;
    switch (status) {
        case LoyaltyStatus::PLATINUM:
            tierName = "PLATINUM";
            break;
        case LoyaltyStatus::GOLD:
            tierName = "GOLD";
            break;
        case LoyaltyStatus::SILVER:
            tierName = "SILVER";
            break;
        case LoyaltyStatus::NONE:
            tierName = "REGULAR";
            break;
    }
    std::cout << tierName << std::endl;

    // 显示会员权益
    std::cout << "\n==================== Member Benefits ====================\n";
    
    if (status == LoyaltyStatus::PLATINUM) {
        std::cout << "- Ticket Discount: 20% OFF" << std::endl;
        std::cout << "- Priority Check-in" << std::endl;
        std::cout << "- Priority Boarding" << std::endl;
        std::cout << "- Lounge Access (Premium)" << std::endl;
        std::cout << "- Extra Baggage Allowance (40kg)" << std::endl;
        std::cout << "- Free Seat Selection" << std::endl;
        std::cout << "- Fast Track Security" << std::endl;
        std::cout << "- Complimentary Upgrades (Subject to availability)" << std::endl;
        std::cout << "- 24/7 Dedicated Customer Service" << std::endl;
        std::cout << "- Flexible Booking Changes" << std::endl;
    } else if (status == LoyaltyStatus::GOLD) {
        std::cout << "- Ticket Discount: 15% OFF" << std::endl;
        std::cout << "- Priority Check-in" << std::endl;
        std::cout << "- Priority Boarding" << std::endl;
        std::cout << "- Lounge Access (Standard)" << std::endl;
        std::cout << "- Extra Baggage Allowance (30kg)" << std::endl;
        std::cout << "- Free Seat Selection" << std::endl;
        std::cout << "- Fast Track Security" << std::endl;
        std::cout << "- Flexible Booking Changes" << std::endl;
    } else if (status == LoyaltyStatus::SILVER) {
        std::cout << "- Ticket Discount: 10% OFF" << std::endl;
        std::cout << "- Priority Check-in" << std::endl;
        std::cout << "- Priority Boarding" << std::endl;
        std::cout << "- Extra Baggage Allowance (25kg)" << std::endl;
        std::cout << "- Free Seat Selection" << std::endl;
    } else {
        std::cout << "- Standard Baggage Allowance (20kg)" << std::endl;
        std::cout << "\nUpgrade your membership to enjoy more benefits!" << std::endl;
    }
    
    std::cout << "\n=====================================================\n";
    
    // 显示订单统计
    std::vector<Order*> myOrders = orderManager.getOrdersByPassport(currentPassenger->getPassportNumber());
    std::cout << "\nTotal Orders: " << myOrders.size() << std::endl;
    
    int completedFlights = 0;
    for (const auto& order : myOrders) {
        if (order->getStatus() == OrderStatus::CHECKED_IN) {
            completedFlights++;
        }
    }
    std::cout << "Completed Flights: " << completedFlights << std::endl;
    std::cout << "=====================================================\n";
}

// 管理飞机型号
void AirlineReservationSystem::manageAircrafts() {
    std::cout << "\n--- Aircraft Model Management ---" << std::endl;
    std::cout << "1. Add new aircraft model" << std::endl;
    std::cout << "2. View all aircraft models" << std::endl;
    std::cout << "0. Back to Admin Menu" << std::endl;
    int choice = getValidIntInput("Enter your choice: ", 0, 2);

    switch (choice) {
        case 1: addAircraftModel(); break;
        case 2: 
            std::cout << "\n--- All Aircraft Models ---" << std::endl;
            for (const auto& aircraft : aircrafts) {
                std::cout << "- " << aircraft->getType() << " (Total Seats: " << aircraft->getTotalSeats() << ")" << std::endl;
            }
            break;
        case 0: break;
    }
}

// 添加飞机型号
void AirlineReservationSystem::addAircraftModel() {
    std::string type = getValidStringInput("Enter new aircraft type name: ");
    int fc = getValidIntInput("Enter number of First Class rows: ", 0, 50);
    int bc = getValidIntInput("Enter number of Business Class rows: ", 0, 100);
    int pe = getValidIntInput("Enter number of Premium Economy rows: ", 0, 100);
    int ec = getValidIntInput("Enter number of Economy Class rows: ", 0, 200);
    int spr = getValidIntInput("Enter number of seats per row: ", 3, 12);

    aircrafts.push_back(new Aircraft(type, fc, bc, pe, ec, spr));
    std::cout << "Aircraft model " << type << " added successfully." << std::endl;
    
    // 保存到CSV文件
    saveAircraftsData();
}


// 管理航班
void AirlineReservationSystem::manageFlights() {
    std::cout << "\n--- Flight Management ---" << std::endl;
    std::cout << "1. Add new flight" << std::endl;
    std::cout << "2. Remove flight" << std::endl;
    std::cout << "3. View all flights" << std::endl;
    std::cout << "0. Back to Admin Menu" << std::endl;
    int choice = getValidIntInput("Enter your choice: ", 0, 3);

    switch (choice) {
        case 1: addFlight(); break;
        case 2: removeFlight(); break;
        case 3: searchFlights(); break;
        case 0: break;
    }
}

// 添加航班
void AirlineReservationSystem::addFlight() {
    std::string flightNumber, originCode, destCode, departureTime, arrivalTime, flightDate;
    
    flightNumber = getValidStringInput("Enter new flight number: ");
    
    // 检查航班号是否已存在
    if (findFlight(flightNumber)) {
        std::cout << "Error: Flight number already exists!" << std::endl;
        return;
    }
    
    originCode = getValidStringInput("Enter origin airport code: ");
    destCode = getValidStringInput("Enter destination airport code: ");
    departureTime = getValidStringInput("Enter departure time (HH:MM): ");
    arrivalTime = getValidStringInput("Enter arrival time (HH:MM or HH:MM+1): ");
    flightDate = getValidStringInput("Enter flight date (YYYY-MM-DD): ");

    // 查找机场
    Airport* originAirport = nullptr;
    Airport* destAirport = nullptr;
    
    for (Airport* airport : airports) {
        if (airport->getIATACode() == originCode) {
            originAirport = airport;
        }
        if (airport->getIATACode() == destCode) {
            destAirport = airport;
        }
    }
    
    if (!originAirport || !destAirport) {
        std::cout << "Error: Invalid airport code(s)!" << std::endl;
        return;
    }
    
    // 计算航程
    double distance = FlightCalculator::calculateDistance(originAirport, destAirport);
    std::cout << "\nFlight distance: " << std::fixed << std::setprecision(1) << distance << " km" << std::endl;
    
    // 自动选择最优机型
    std::cout << "Automatically selecting optimal aircraft..." << std::endl;
    Aircraft* selectedAircraft = selectOptimalAircraft(distance);
    
    if (!selectedAircraft) {
        std::cout << "Error: Cannot select suitable aircraft!" << std::endl;
        return;
    }

    // 创建新航班
    Flight* newFlight = new Flight(flightNumber, originCode, destCode, 
                                   departureTime, arrivalTime, selectedAircraft, flightDate);
    
    // 设置机场信息
    newFlight->setOriginAirport(originAirport);
    newFlight->setDestinationAirport(destAirport);
    
    // 添加到系统
    flights.push_back(newFlight);
    flightGraph[originCode].push_back(newFlight); // 更新图
    
    std::cout << "\nFlight " << flightNumber << " added successfully!" << std::endl;
    std::cout << "Aircraft: " << selectedAircraft->getType() << std::endl;
    
    // 保存到CSV文件
    saveFlightsData();
}

// 删除航班
void AirlineReservationSystem::removeFlight() {
    std::string flightNumber = getValidStringInput("Enter flight number to remove: ");

    // 从主列表删除
    auto it = std::remove_if(flights.begin(), flights.end(), [&](Flight* flight) {
        if (flight->getFlightNumber() == flightNumber) {
            // 从图中删除
            auto& graphNode = flightGraph[flight->getOrigin()];
            graphNode.erase(std::remove(graphNode.begin(), graphNode.end(), flight), graphNode.end());
            delete flight;
            return true;
        }
        return false;
    });

    if (it != flights.end()) {
        flights.erase(it, flights.end());
        std::cout << "Flight " << flightNumber << " removed successfully." << std::endl;
        
        // 保存到CSV文件
        saveFlightsData();
    } else {
        std::cout << "Flight " << flightNumber << " not found." << std::endl;
    }
}

// 管理机组成员
void AirlineReservationSystem::manageCrew() {
    int choice;
    do {
        std::cout << "\n--- Crew Management ---" << std::endl;
        std::cout << "1. Add new crew member" << std::endl;
        std::cout << "2. View all crew members" << std::endl;
        std::cout << "3. Assign crew to a flight" << std::endl;
        std::cout << "0. Back to Admin Menu" << std::endl;
        choice = getValidIntInput("Enter your choice: ", 0, 3);
        switch (choice) {
            case 1: addCrewMember(); break;
            case 2: viewCrewMembers(); break;
            case 3: assignCrewToFlight(); break;
            case 0: break;
        }
    } while (choice != 0);
}

// 查看乘客列表
void AirlineReservationSystem::viewPassengerList() {
    std::cout << "\n--- All Passengers ---" << std::endl;
    displayPassengers(passengerTree);
}

// 查看航班状态
// 查看航班状态
void AirlineReservationSystem::viewFlightStatus() {
    std::string flightNumber = getValidStringInput("Enter flight number: ");

    Flight* flight = findFlight(flightNumber);
    if (!flight) {
        std::cout << "Flight not found." << std::endl;
        return;
    }

    // 显示座位图
    std::cout << "\n========== Flight Status: " << flightNumber << " ==========\n";
    flight->getAircraft()->displaySeatMap();
    
    // 显示机组成员
    std::cout << "\n--- Crew Members ---" << std::endl;
    flight->viewCrew();
    
    // 显示乘客信息
    std::cout << "\n--- Passenger List ---" << std::endl;
    std::vector<Order*> flightOrders = orderManager.getOrdersByFlight(flightNumber);
    
    if (flightOrders.empty()) {
        std::cout << "No passengers booked for this flight." << std::endl;
    } else {
        int checkedInCount = 0;
        int paidCount = 0;
        int pendingCount = 0;
        
        std::cout << std::left << std::setw(20) << "Passenger Name" 
                  << std::setw(15) << "Passport" 
                  << std::setw(10) << "Seat"
                  << std::setw(20) << "Status" << std::endl;
        std::cout << std::string(65, '-') << std::endl;
        
        for (Order* order : flightOrders) {
            Passenger* passenger = findPassenger(order->getPassportNumber());
            if (!passenger) continue;
            
            std::string statusStr;
            switch (order->getStatus()) {
                case OrderStatus::PENDING_PAYMENT:
                    statusStr = "Pending Payment";
                    pendingCount++;
                    break;
                case OrderStatus::PAID:
                    statusStr = "Paid (Not Checked In)";
                    paidCount++;
                    break;
                case OrderStatus::CHECKED_IN:
                    statusStr = "Checked In";
                    checkedInCount++;
                    break;
                case OrderStatus::CANCELLED:
                    statusStr = "Cancelled";
                    break;
            }
            
            std::cout << std::left << std::setw(20) << passenger->getName()
                      << std::setw(15) << passenger->getPassportNumber()
                      << std::setw(10) << (order->getSeatNumber().empty() ? "N/A" : order->getSeatNumber())
                      << std::setw(20) << statusStr << std::endl;
        }
        
        std::cout << std::string(65, '-') << std::endl;
        std::cout << "Total Passengers: " << flightOrders.size() << std::endl;
        std::cout << "  - Checked In: " << checkedInCount << std::endl;
        std::cout << "  - Paid (Awaiting Check-In): " << paidCount << std::endl;
        std::cout << "  - Pending Payment: " << pendingCount << std::endl;
    }
}

// 生成航线飞行简报
// 生成航班简报
void AirlineReservationSystem::generateFlightBriefing() {
    std::string flightNumber = getValidStringInput("Enter flight number for briefing: ");

    Flight* flight = findFlight(flightNumber);
    if (!flight) {
        std::cout << "Flight not found." << std::endl;
        return;
    }

    std::cout << "\n--- Flight Briefing for " << flightNumber << " ---" << std::endl;
    std::cout << "Route: " << flight->getOrigin() << " -> " << flight->getDestination() << std::endl;
    std::cout << "Aircraft: " << flight->getAircraft()->getType() << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // 统计预订
    int fc_booked = 0, bc_booked = 0, ec_booked = 0;
    std::vector<Passenger*> confirmedPassengers;
    for (const auto& pair : reservations) {
        Reservation* res = pair.second;
        if (res->getFlight() == flight && res->getStatus() == ReservationStatus::CONFIRMED) {
            confirmedPassengers.push_back(res->getPassenger());
            switch (res->getSeat()->getType()) {
                case SeatType::FIRST_CLASS: fc_booked++; break;
                case SeatType::BUSINESS_CLASS: bc_booked++; break;
                case SeatType::ECONOMY_CLASS: ec_booked++; break;
            }
        }
    }

    // 统计座位
    int fc_total = flight->getAircraft()->getSeatCountByType(SeatType::FIRST_CLASS);
    int bc_total = flight->getAircraft()->getSeatCountByType(SeatType::BUSINESS_CLASS);
    int ec_total = flight->getAircraft()->getSeatCountByType(SeatType::ECONOMY_CLASS);
    int total_seats = flight->getAircraft()->getTotalSeats();
    int total_booked = fc_booked + bc_booked + ec_booked;

    // 显示预订统计
    std::cout << "Booking Statistics:" << std::endl;
    if (fc_total > 0) std::cout << "  First Class: " << fc_booked << "/" << fc_total << " (" << std::fixed << std::setprecision(1) << (fc_total > 0 ? (double)fc_booked/fc_total*100 : 0.0) << "%)" << std::endl;
    if (bc_total > 0) std::cout << "  Business Class: " << bc_booked << "/" << bc_total << " (" << (bc_total > 0 ? (double)bc_booked/bc_total*100 : 0.0) << "%)" << std::endl;
    if (ec_total > 0) std::cout << "  Economy Class: " << ec_booked << "/" << ec_total << " (" << (ec_total > 0 ? (double)ec_booked/ec_total*100 : 0.0) << "%)" << std::endl;
    std::cout << "  Total: " << total_booked << "/" << total_seats << " (" << (total_seats > 0 ? (double)total_booked/total_seats*100 : 0.0) << "%)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // 显示乘客名单
    std::cout << "Confirmed Passenger Manifest:" << std::endl;
    if (confirmedPassengers.empty()) {
        std::cout << "  No confirmed passengers." << std::endl;
    } else {
        for (const auto& p : confirmedPassengers) {
            std::cout << "  - " << p->getName() << " (Passport: " << p->getPassportNumber() << ")" << std::endl;
        }
    }
    std::cout << "----------------------------------------" << std::endl;

    // 显示机组
    flight->viewCrew();
    std::cout << "--- End of Briefing ---" << std::endl;
}

// 生成飞行计划
// 生成飞行计划
void AirlineReservationSystem::generateFlightPlan() {
    std::string flightNumber = getValidStringInput("Enter flight number for flight plan: ");

    Flight* flight = findFlight(flightNumber);
    if (!flight) {
        std::cout << "Flight not found." << std::endl;
        return;
    }

    // 计算已值机的乘客数量
    std::vector<Order*> flightOrders = orderManager.getOrdersByFlight(flightNumber);
    int checkedInPassengers = 0;
    for (Order* order : flightOrders) {
        if (order->getStatus() == OrderStatus::CHECKED_IN) {
            checkedInPassengers++;
        }
    }

    FlightPlan plan(flight, airports);

    // 传入乘客数量显示飞行计划
    plan.displayWithPassengerCount(checkedInPassengers);
}

// 查找航班
Flight* AirlineReservationSystem::findFlight(const std::string& flightNumber) {
    for (auto& flight : flights) {
        if (flight->getFlightNumber() == flightNumber) {
            return flight;
        }
    }
    return nullptr;
}

// 查找乘客
Passenger* AirlineReservationSystem::findPassenger(const std::string& passportNumber) {
    // 遍历树来查找乘客
    PassengerNode* current = passengerTree;
    while (current != nullptr) {
        if (passportNumber == current->passenger->getPassportNumber()) {
            return current->passenger;
        } else if (passportNumber < current->passenger->getPassportNumber()) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return nullptr;
}

// 添加乘客 (插入到树中和向量中)
void AirlineReservationSystem::addPassenger(Passenger* passenger) {
    // 检查是否已存在
    if (findPassenger(passenger->getPassportNumber()) != nullptr) {
        return; // 已存在,不重复添加
    }
    
    // 添加到树中
    PassengerNode* newNode = new PassengerNode(passenger);
    if (passengerTree == nullptr) {
        passengerTree = newNode;
    } else {
        PassengerNode* current = passengerTree;
        while (true) {
            if (passenger->getPassportNumber() < current->passenger->getPassportNumber()) {
                if (current->left == nullptr) {
                    current->left = newNode;
                    break;
                }
                current = current->left;
            } else {
                if (current->right == nullptr) {
                    current->right = newNode;
                    break;
                }
                current = current->right;
            }
        }
    }
    
    // 添加到向量中
    passengers.push_back(passenger);
}

// 显示乘客 (中序遍历)
void AirlineReservationSystem::displayPassengers(PassengerNode* node) {
    if (node == nullptr) return;
    displayPassengers(node->left);
    std::cout << "Name: " << node->passenger->getName() << ", Passport: " << node->passenger->getPassportNumber() << std::endl;
    displayPassengers(node->right);
}

// 添加机组成员
void AirlineReservationSystem::addCrewMember() {
    std::string id = getValidStringInput("Enter crew ID: ");
    std::string name = getValidStringInput("Enter crew name: ");
    int roleChoice = getValidIntInput("Select role (1: Pilot, 2: Cabin Crew): ", 1, 2);
    CrewRole role = (roleChoice == 1) ? CrewRole::PILOT : CrewRole::CABIN_CREW;

    Crew* newCrew = new Crew(id, name, role);

    if (role == CrewRole::PILOT) {
        int numQualifications = getValidIntInput("Enter number of aircraft qualifications: ", 0, 20);
        for (int i = 0; i < numQualifications; ++i) {
            std::string acType = getValidStringInput("Enter qualified aircraft type " + std::to_string(i + 1) + ": ");
            newCrew->addQualification(acType);
        }
    }

    crewMembers.push_back(newCrew);
    std::cout << "Crew member " << name << " added successfully." << std::endl;
    
    // 保存到CSV文件
    saveCrewData();
}

// 查看所有机组成员
void AirlineReservationSystem::viewCrewMembers() {
    std::cout << "\n--- All Crew Members ---" << std::endl;
    for (const auto& member : crewMembers) {
        std::cout << "ID: " << member->getId() << ", Name: " << member->getName()
                  << ", Role: " << (member->getRole() == CrewRole::PILOT ? "Pilot" : "Cabin Crew") << std::endl;
        if (member->getRole() == CrewRole::PILOT) {
            const auto& qualifications = member->getQualifications();
            if (!qualifications.empty()) {
                std::cout << "  Qualifications: ";
                for (const auto& type : qualifications) {
                    std::cout << type << " ";
                }
                std::cout << std::endl;
            }
        }
    }
}

// 分配机组到航班
void AirlineReservationSystem::assignCrewToFlight() {
    std::string flightNumber = getValidStringInput("Enter flight number to assign crew: ");

    Flight* flight = findFlight(flightNumber);
    if (!flight) {
        std::cout << "Flight not found." << std::endl;
        return;
    }

    std::cout << "Assigning crew for flight " << flight->getFlightNumber()
              << " on aircraft " << flight->getAircraft()->getType() << std::endl;

    std::vector<Crew*> assigned;
    std::string crewId;
    std::cout << "Enter crew IDs to assign (enter 'done' to finish):" << std::endl;

    while (std::cin >> crewId && crewId != "done") {
        Crew* member = nullptr;
        for (auto& m : crewMembers) {
            if (m->getId() == crewId) {
                member = m;
                break;
            }
        }

        if (member) {
            // 检查机长资质
            if (member->getRole() == CrewRole::PILOT) {
                bool qualified = false;
                for (const auto& type : member->getQualifications()) {
                    if (type == flight->getAircraft()->getType()) {
                        qualified = true;
                        break;
                    }
                }
                if (qualified) {
                    assigned.push_back(member);
                    std::cout << "Pilot " << member->getName() << " assigned." << std::endl;
                } else {
                    std::cout << "Error: Pilot " << member->getName() << " is not qualified for " << flight->getAircraft()->getType() << "." << std::endl;
                }
            } else {
                assigned.push_back(member);
                std::cout << "Cabin crew " << member->getName() << " assigned." << std::endl;
            }
        } else {
            std::cout << "Crew member with ID " << crewId << " not found." << std::endl;
        }
    }

    flight->assignCrew(assigned);
    std::cout << "Crew assignment for flight " << flightNumber << " completed." << std::endl;
}

// 查找转机航班 (BFS/DFS)
std::vector<std::vector<Flight*>> AirlineReservationSystem::findConnectingFlights(const std::string& origin, const std::string& destination) {
    std::vector<std::vector<Flight*>> result;
    std::map<std::string, bool> visited;
    std::vector<Flight*> path;

    // 使用BFS来寻找所有可能的路径
    std::queue<std::pair<std::string, std::vector<Flight*>>> q;
    q.push({origin, {}});

    while (!q.empty()) {
        std::string currentCity = q.front().first;
        std::vector<Flight*> currentPath = q.front().second;
        q.pop();

        if (currentCity == destination) {
            result.push_back(currentPath);
            continue;
        }

        if (flightGraph.count(currentCity)) {
            for (Flight* flight : flightGraph[currentCity]) {
                // 避免环路
                bool cityInPath = false;
                for(const auto& p_flight : currentPath){
                    if(p_flight->getDestination() == flight->getDestination()){
                        cityInPath = true;
                        break;
                    }
                }
                if(!cityInPath){
                    std::vector<Flight*> newPath = currentPath;
                    newPath.push_back(flight);
                    q.push({flight->getDestination(), newPath});
                }
            }
        }
    }

    return result;
}


// 获取常旅客优先级
int AirlineReservationSystem::getLoyaltyPriority(LoyaltyStatus status) {
    switch (status) {
        case LoyaltyStatus::PLATINUM: return 4;
        case LoyaltyStatus::GOLD: return 3;
        case LoyaltyStatus::SILVER: return 2;
        case LoyaltyStatus::NONE: return 1;
        default: return 0;
    }
}

// 生成唯一的预订号
std::string AirlineReservationSystem::generateReservationNumber() {
    std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, chars.size() - 1);
    std::stringstream ss;
    for (int i = 0; i < 6; ++i) {
        ss << chars[distribution(generator)];
    }
    return ss.str();
}

// 计算基础票价 (根据航程距离和仓位等级)
double AirlineReservationSystem::calculateBasePrice(Flight* flight, SeatType seatType) {
    if (!flight || !flight->getOriginAirport() || !flight->getDestinationAirport()) {
        return 0.0;
    }

    // 使用FlightCalculator计算航程距离(公里)
    double distanceKm = FlightCalculator::calculateDistance(
        flight->getOriginAirport(), 
        flight->getDestinationAirport()
    );

    // 基础价格系数 (美元/公里)
    double basePricePerKm = 0.15;
    
    // 不同仓位的价格系数
    double cabinMultiplier = 1.0;
    switch (seatType) {
        case SeatType::FIRST_CLASS:
            cabinMultiplier = 4.0;  // 头等舱是基础价格的4倍
            break;
        case SeatType::BUSINESS_CLASS:
            cabinMultiplier = 2.5;  // 商务舱是基础价格的2.5倍
            break;
        case SeatType::PREMIUM_ECONOMY:
            cabinMultiplier = 1.5;  // 超级经济舱是基础价格的1.5倍
            break;
        case SeatType::ECONOMY_CLASS:
            cabinMultiplier = 1.0;  // 经济舱是基础价格
            break;
    }

    // 计算基础价格
    double basePrice = distanceKm * basePricePerKm * cabinMultiplier;
    
    return basePrice;
}

// 获取会员折扣率
double AirlineReservationSystem::getLoyaltyDiscount(LoyaltyStatus status) {
    switch (status) {
        case LoyaltyStatus::PLATINUM:
            return 0.20;  // 白金卡20%折扣
        case LoyaltyStatus::GOLD:
            return 0.15;  // 金卡15%折扣
        case LoyaltyStatus::SILVER:
            return 0.10;  // 银卡10%折扣
        case LoyaltyStatus::NONE:
        default:
            return 0.0;   // 无折扣
    }
}

// 计算最终票价 (应用会员折扣)
double AirlineReservationSystem::calculateFinalPrice(Flight* flight, SeatType seatType, LoyaltyStatus loyaltyStatus) {
    double basePrice = calculateBasePrice(flight, seatType);
    double discount = getLoyaltyDiscount(loyaltyStatus);
    double finalPrice = basePrice * (1.0 - discount);
    
    return finalPrice;
}

// 保存所有数据到CSV文件
void AirlineReservationSystem::saveAllData() {
    saveAircraftsData();
    saveFlightsData();
    saveCrewData();
    savePassengersData();
    saveReservationsData();
    orderManager.saveToCSV("data/orders.csv");
}

// 保存飞机数据
void AirlineReservationSystem::saveAircraftsData() {
    DataLoader::saveAircrafts("data/aircrafts.csv", aircrafts);
}

// 保存航班数据
void AirlineReservationSystem::saveFlightsData() {
    DataLoader::saveFlights("data/flights.csv", flights);
}

// 保存机组成员数据
void AirlineReservationSystem::saveCrewData() {
    DataLoader::saveCrewMembers("data/crew.csv", crewMembers);
}

// 保存乘客数据
void AirlineReservationSystem::savePassengersData() {
    DataLoader::savePassengers("data/passengers.csv", passengers);
}

// 保存预订数据
void AirlineReservationSystem::saveReservationsData() {
    DataLoader::saveReservations("data/reservations.csv", reservations);
}

// ==================== 自动选择最优机型 ====================
// 根据航程自动选择最合适的飞机型号
// 规则: 如果航程超过所有飞机的最大航程,使用A380
//      否则,优先选择航程接近飞行距离的最小飞机
Aircraft* AirlineReservationSystem::selectOptimalAircraft(double distanceKm) {
    if (aircrafts.empty()) {
        std::cerr << "No aircraft available!" << std::endl;
        return nullptr;
    }
    
    // 查找所有能够完成此航程的飞机
    std::vector<Aircraft*> suitableAircrafts;
    Aircraft* a380 = nullptr;
    
    for (Aircraft* aircraft : aircrafts) {
        if (aircraft->getType() == "A380-800") {
            a380 = aircraft;
        }
        if (aircraft->getMaxRange() >= distanceKm) {
            suitableAircrafts.push_back(aircraft);
        }
    }
    
    // 如果没有飞机能完成航程,使用A380
    if (suitableAircrafts.empty()) {
        std::cout << "Distance " << distanceKm << " km exceeds all aircraft ranges. Using A380-800." << std::endl;
        return a380;
    }
    
    // 找出最合适的飞机: 航程最接近飞行距离的最小飞机
    Aircraft* optimalAircraft = suitableAircrafts[0];
    int minSeats = optimalAircraft->getTotalSeats();
    double minRangeDiff = optimalAircraft->getMaxRange() - distanceKm;
    
    for (Aircraft* aircraft : suitableAircrafts) {
        int seats = aircraft->getTotalSeats();
        double rangeDiff = aircraft->getMaxRange() - distanceKm;
        
        // 优先选择航程接近的,其次选择座位数少的
        if (rangeDiff < minRangeDiff || (rangeDiff == minRangeDiff && seats < minSeats)) {
            optimalAircraft = aircraft;
            minSeats = seats;
            minRangeDiff = rangeDiff;
        }
    }
    
    std::cout << "Selected aircraft: " << optimalAircraft->getType() 
              << " (Max Range: " << optimalAircraft->getMaxRange() << " km, "
              << "Total Seats: " << optimalAircraft->getTotalSeats() << ")" << std::endl;
    
    return optimalAircraft;
}

// ==================== 自动分配机组成员 ====================
// 根据飞机型号自动分配有资质的机组人员
std::vector<Crew*> AirlineReservationSystem::assignCrewForAircraft(const std::string& aircraftType) {
    std::vector<Crew*> assignedCrew;
    
    // 1. 查找有资质的机长: 优先使用预处理映射并选择最近空闲的
    Crew* pilot = nullptr;
    auto it = pilotsByAircraftType.find(aircraftType);
    if (it != pilotsByAircraftType.end() && !it->second.empty()) {
        // 选择 lastAssigned 最早（最空闲）的机长
        Crew* best = nullptr;
        for (Crew* c : it->second) {
            if (!best) best = c;
            else {
                std::time_t a = c->getLastAssigned();
                std::time_t b = best->getLastAssigned();
                // lastAssigned == 0 表示未分配过 -> treat as most idle
                if ((a == 0 && b != 0) || (a != 0 && b != 0 && a < b)) {
                    best = c;
                }
            }
        }
        pilot = best;
        if (pilot) {
            assignedCrew.push_back(pilot);
            std::cout << "Assigned pilot: " << pilot->getName() << " (" << pilot->getId() << ")" << std::endl;
        }
    } else {
        // 回退: 线性扫描所有机组成员
        for (Crew* crew : crewMembers) {
            if (crew->getRole() == CrewRole::PILOT) {
                const auto& qualifications = crew->getQualifications();
                if (std::find(qualifications.begin(), qualifications.end(), aircraftType) != qualifications.end()) {
                    pilot = crew;
                    assignedCrew.push_back(pilot);
                    std::cout << "Assigned pilot: " << pilot->getName() << " (" << pilot->getId() << ")" << std::endl;
                    break;
                }
            }
        }
    }
    
    if (!pilot) {
        std::cout << "Warning: No qualified pilot found for " << aircraftType << std::endl;
    }
    
    // 2. 分配乘务员 (根据飞机大小分配2-4名)
    int cabinCrewNeeded = 2; // 默认2名
    // 根据飞机型号判断需要的乘务员数量
    if (aircraftType.find("A380") != std::string::npos || 
        aircraftType.find("777-300ER") != std::string::npos ||
        aircraftType.find("A350") != std::string::npos) {
        cabinCrewNeeded = 4;
    } else if (aircraftType.find("787") != std::string::npos ||
               aircraftType.find("A330") != std::string::npos ||
               aircraftType.find("777-200ER") != std::string::npos) {
        cabinCrewNeeded = 3;
    }
    
    int cabinCrewAssigned = 0;
    // 优先从映射中选择最近空闲的乘务员
    auto cit = cabinCrewByAircraftType.find(aircraftType);
    if (cit != cabinCrewByAircraftType.end() && !cit->second.empty()) {
        std::vector<Crew*> candidates = cit->second;
        std::sort(candidates.begin(), candidates.end(), [](Crew* a, Crew* b){
            auto ta = a->getLastAssigned();
            auto tb = b->getLastAssigned();
            if (ta == 0 && tb != 0) return true;   // 未分配过者优先
            if (tb == 0 && ta != 0) return false;
            return ta < tb; // 较早（更空闲）在前
        });
        for (Crew* c : candidates) {
            if (cabinCrewAssigned >= cabinCrewNeeded) break;
            assignedCrew.push_back(c);
            std::cout << "Assigned cabin crew: " << c->getName() << " (" << c->getId() << ")" << std::endl;
            cabinCrewAssigned++;
        }
    }
    // 若不足，回退到全量扫描
    if (cabinCrewAssigned < cabinCrewNeeded) {
        for (Crew* crew : crewMembers) {
            if (cabinCrewAssigned >= cabinCrewNeeded) break;
            if (crew->getRole() == CrewRole::CABIN_CREW) {
                const auto& qualifications = crew->getQualifications();
                if (std::find(qualifications.begin(), qualifications.end(), aircraftType) != qualifications.end()) {
                    // 避免重复添加同一人
                    if (std::find(assignedCrew.begin(), assignedCrew.end(), crew) == assignedCrew.end()) {
                        assignedCrew.push_back(crew);
                        std::cout << "Assigned cabin crew: " << crew->getName() << " (" << crew->getId() << ")" << std::endl;
                        cabinCrewAssigned++;
                    }
                }
            }
        }
    }
    
    if (cabinCrewAssigned < cabinCrewNeeded) {
        std::cout << "Warning: Only assigned " << cabinCrewAssigned 
                  << " cabin crew members (needed " << cabinCrewNeeded << ")" << std::endl;
    }

    // 更新时间戳: 分配完成的机长与乘务员 lastAssigned = 当前时间
    std::time_t now = std::time(nullptr);
    for (Crew* c : assignedCrew) {
        c->setLastAssigned(now);
    }
    
    return assignedCrew;
}

// ==================== 查找最近的备降机场 ====================
// 使用简单搜索算法找到距离目的地最近的机场作为备降机场
Airport* AirlineReservationSystem::findNearestAlternateAirport(Airport* destination) {
    if (!destination || airports.size() < 2) {
        return nullptr;
    }
    
    Airport* nearestAirport = nullptr;
    double minDistance = 999999.0;
    
    for (Airport* airport : airports) {
        // 跳过目的地机场本身
        if (airport->getIATACode() == destination->getIATACode()) {
            continue;
        }
        
        // 计算距离
        double distance = FlightCalculator::calculateDistance(destination, airport);
        
        if (distance < minDistance) {
            minDistance = distance;
            nearestAirport = airport;
        }
    }
    
    if (nearestAirport) {
        std::cout << "Selected alternate airport: " << nearestAirport->getName() 
                  << " (" << nearestAirport->getIATACode() << "), "
                  << "Distance: " << std::fixed << std::setprecision(1) << minDistance << " km" << std::endl;
    }
    
    return nearestAirport;
}

// ==================== 自动安排新航班 ====================
// 管理员功能: 输入时间地点信息,自动安排机型、机组、生成飞行计划
// 自动调度新航班
void AirlineReservationSystem::scheduleNewFlight() {
    std::cout << "\n========== Schedule New Flight ==========" << std::endl;
    
    std::string flightNumber, originCode, destCode, departureTime, arrivalTime, flightDate;
    
    // 输入航班基本信息
    flightNumber = getValidStringInput("Enter flight number (e.g., CA123): ");
    
    // 检查航班号是否已存在
    if (findFlight(flightNumber)) {
        std::cout << "Error: Flight number already exists!" << std::endl;
        return;
    }
    
    originCode = getValidStringInput("Enter origin airport code (e.g., PEK): ");
    destCode = getValidStringInput("Enter destination airport code (e.g., LAX): ");
    departureTime = getValidStringInput("Enter departure time (HH:MM): ");
    arrivalTime = getValidStringInput("Enter arrival time (HH:MM or HH:MM+1): ");
    flightDate = getValidStringInput("Enter flight date (YYYY-MM-DD): ");
    
    // 查找机场
    Airport* originAirport = nullptr;
    Airport* destAirport = nullptr;
    
    for (Airport* airport : airports) {
        if (airport->getIATACode() == originCode) {
            originAirport = airport;
        }
        if (airport->getIATACode() == destCode) {
            destAirport = airport;
        }
    }
    
    if (!originAirport || !destAirport) {
        std::cout << "Error: Invalid airport code(s)!" << std::endl;
        return;
    }
    
    // 计算航程
    double distance = FlightCalculator::calculateDistance(originAirport, destAirport);
    std::cout << "\nFlight distance: " << std::fixed << std::setprecision(1) << distance << " km" << std::endl;
    
    // 自动选择最优机型
    std::cout << "\n--- Selecting Optimal Aircraft ---" << std::endl;
    Aircraft* selectedAircraft = selectOptimalAircraft(distance);
    
    if (!selectedAircraft) {
        std::cout << "Error: Cannot select suitable aircraft!" << std::endl;
        return;
    }
    
    // 自动分配机组
    std::cout << "\n--- Assigning Crew Members ---" << std::endl;
    std::vector<Crew*> assignedCrew = assignCrewForAircraft(selectedAircraft->getType());
    
    if (assignedCrew.empty()) {
        std::cout << "Error: Cannot assign crew members!" << std::endl;
        return;
    }
    
    // 创建新航班
    Flight* newFlight = new Flight(flightNumber, originCode, destCode, 
                                   departureTime, arrivalTime, selectedAircraft, flightDate);
    
    // 设置机场信息
    newFlight->setOriginAirport(originAirport);
    newFlight->setDestinationAirport(destAirport);
    
    // 分配机组
    for (Crew* crew : assignedCrew) {
        newFlight->addCrewMember(crew);
    }
    
    // 添加到系统
    flights.push_back(newFlight);
    flightGraph[originCode].push_back(newFlight); // 更新图以支持转机搜索
    
    // 保存数据
    saveFlightsData();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Flight " << flightNumber << " has been successfully scheduled!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 显示飞行计划
    std::cout << "\nWould you like to view the flight plan? (y/n): ";
    char choice;
    std::cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        FlightPlan plan(newFlight, airports);
        plan.display();
    }
}

// ==================== 转机航班搜索功能 ====================

// 将时间字符串转换为分钟数
int AirlineReservationSystem::timeToMinutes(const std::string& time) {
    int hours = 0, minutes = 0;
    char colon;
    std::stringstream ss(time);
    ss >> hours >> colon >> minutes;
    return hours * 60 + minutes;
}

// 将分钟数转换为时间字符串
std::string AirlineReservationSystem::minutesToTime(int minutes) {
    int hours = (minutes / 60) % 24;
    int mins = minutes % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":" 
       << std::setfill('0') << std::setw(2) << mins;
    return ss.str();
}

// 给时间加上分钟数
std::string AirlineReservationSystem::addMinutesToTime(const std::string& time, int minutes) {
    int totalMinutes = timeToMinutes(time) + minutes;
    return minutesToTime(totalMinutes);
}

// 计算两个航班之间的转机时间（分钟）
int AirlineReservationSystem::calculateLayoverMinutes(Flight* flight1, Flight* flight2) {
    if (!flight1 || !flight2) return -1;
    
    // 获取第一个航班的到达时间
    std::string arrivalTime = flight1->getArrivalTime();
    std::string departureTime = flight2->getDepartureTime();
    
    // 处理跨天情况（如果到达时间有+1标记）
    int arrivalDay = 0;
    if (arrivalTime.find("+1") != std::string::npos) {
        arrivalDay = 1;
        arrivalTime = arrivalTime.substr(0, arrivalTime.find("+"));
    }
    
    int departureDay = 0;
    if (departureTime.find("+1") != std::string::npos) {
        departureDay = 1;
        departureTime = departureTime.substr(0, departureTime.find("+"));
    }
    
    int arrivalMinutes = timeToMinutes(arrivalTime) + arrivalDay * 24 * 60;
    int departureMinutes = timeToMinutes(departureTime) + departureDay * 24 * 60;
    
    // 如果第二个航班是第二天的，需要调整
    if (departureMinutes < arrivalMinutes) {
        departureMinutes += 24 * 60;
    }
    
    return departureMinutes - arrivalMinutes;
}

// 检查两个航班是否可以衔接（转机时间是否合理）
bool AirlineReservationSystem::isValidLayover(Flight* flight1, Flight* flight2) {
    if (!flight1 || !flight2) return false;
    
    // 第一个航班的目的地必须是第二个航班的出发地
    if (flight1->getDestination() != flight2->getOrigin()) {
        return false;
    }
    
    // 计算转机时间
    int layoverMinutes = calculateLayoverMinutes(flight1, flight2);
    
    // 最少转机时间：国际航班90分钟，国内航班60分钟（这里简化为统一90分钟）
    const int MIN_LAYOVER = 90;
    // 最长转机时间：12小时
    const int MAX_LAYOVER = 12 * 60;
    
    return layoverMinutes >= MIN_LAYOVER && layoverMinutes <= MAX_LAYOVER;
}

// 搜索转机航班（使用Dijkstra算法找最短路径）
std::vector<ConnectingFlightRoute> AirlineReservationSystem::findConnectingFlights(
    const std::string& origin, const std::string& destination, const std::string& travelDate) {
    
    std::vector<ConnectingFlightRoute> validRoutes;
    
    // 使用Dijkstra算法，最多允许2次转机（即最多3段航班）
    const int MAX_SEGMENTS = 5;
    
    // Dijkstra优先队列：<总距离, <当前路径, 已访问城市>>
    using PathInfo = std::pair<std::vector<Flight*>, std::set<std::string>>;
    using DijkstraNode = std::pair<double, PathInfo>;
    std::priority_queue<DijkstraNode, std::vector<DijkstraNode>, std::greater<DijkstraNode>> pq;
    
    // 记录到达每个城市的最短距离（忽略时间）
    std::map<std::string, double> bestDistance;
    
    // 找出所有从起点出发的航班，加入优先队列（忽略日期）
    for (Flight* flight : flights) {
        if (flight->getOrigin() == origin) {
            double distance = 0.0;
            if (flight->getOriginAirport() && flight->getDestinationAirport()) {
                distance = FlightCalculator::calculateDistance(
                    flight->getOriginAirport(), flight->getDestinationAirport());
            }
            
            std::vector<Flight*> path = {flight};
            std::set<std::string> visited = {origin};
            pq.push({distance, {path, visited}});
        }
    }
    
    while (!pq.empty()) {
        auto current = pq.top();
        pq.pop();
        
        double currentDistance = current.first;
        std::vector<Flight*>& currentPath = current.second.first;
        std::set<std::string>& visitedCities = current.second.second;
        
        Flight* lastFlight = currentPath.back();
        std::string currentCity = lastFlight->getDestination();
        
        // 如果到达目的地，保存这条路线
        if (currentCity == destination) {
            ConnectingFlightRoute route;
            route.flights = currentPath;
            route.isValid = true;
            route.totalDistance = currentDistance;
            
            // 计算转机时间
            for (size_t i = 1; i < currentPath.size(); ++i) {
                int layover = calculateLayoverMinutes(currentPath[i-1], currentPath[i]);
                route.layoverMinutes.push_back(layover);
            }
            
            validRoutes.push_back(route);
            continue;
        }
        
        // 状态剪枝：如果已有更好的路径到达此城市，跳过（忽略时间）
        if (bestDistance.find(currentCity) != bestDistance.end() && 
            bestDistance[currentCity] <= currentDistance) {
            continue;
        }
        bestDistance[currentCity] = currentDistance;
        
        // 如果还没到达最大段数，继续搜索
        if (currentPath.size() < MAX_SEGMENTS) {
            // 找出所有从当前城市出发的航班
            for (Flight* nextFlight : flights) {
                if (nextFlight->getOrigin() == currentCity) {
                    // 避免访问已经去过的城市（避免环路）
                    if (visitedCities.find(nextFlight->getDestination()) != visitedCities.end()) {
                        continue;
                    }
                    
                    // 计算新路径的总距离
                    double segmentDistance = 0.0;
                    if (nextFlight->getOriginAirport() && nextFlight->getDestinationAirport()) {
                        segmentDistance = FlightCalculator::calculateDistance(
                            nextFlight->getOriginAirport(), nextFlight->getDestinationAirport());
                    }
                    double newDistance = currentDistance + segmentDistance;
                    
                    // 创建新的路径
                    std::vector<Flight*> newPath = currentPath;
                    newPath.push_back(nextFlight);
                    
                    std::set<std::string> newVisited = visitedCities;
                    newVisited.insert(nextFlight->getDestination());
                    
                    pq.push({newDistance, {newPath, newVisited}});
                }
            }
        }
    }
    
    // 结果已经基本按距离排序（优先队列机制），但为确保完全排序再处理一次
    std::sort(validRoutes.begin(), validRoutes.end(), 
        [](const ConnectingFlightRoute& a, const ConnectingFlightRoute& b) {
            return a.totalDistance < b.totalDistance;
        });
    
    return validRoutes;
}

// 显示转机航班路线
void AirlineReservationSystem::displayConnectingRoute(const ConnectingFlightRoute& route, int index) {
    std::cout << "\n--- Route " << index << " ---" << std::endl;
    std::cout << "Total Segments: " << route.flights.size() << std::endl;
    std::cout << "Total Distance: " << std::fixed << std::setprecision(1) 
              << route.totalDistance << " km" << std::endl;
    
    std::cout << "\nFlight Details:" << std::endl;
    for (size_t i = 0; i < route.flights.size(); ++i) {
        Flight* flight = route.flights[i];
        std::cout << "  Segment " << (i + 1) << ": " << flight->getFlightNumber() 
                  << " | " << flight->getOrigin() << " -> " << flight->getDestination()
                  << " | Aircraft: " << flight->getAircraft()->getType() << std::endl;
    }
}

// 预订联程航班
void AirlineReservationSystem::bookConnectingFlight(const ConnectingFlightRoute& route, SeatType seatType) {
    if (route.flights.empty()) {
        std::cout << "Error: Invalid route." << std::endl;
        return;
    }
    
    // 检查每段航班的舱位是否匹配，收集需要降级的航段
    struct CabinAdjustment {
        size_t segmentIndex;
        Flight* flight;
        SeatType originalClass;
        SeatType adjustedClass;
    };
    std::vector<CabinAdjustment> adjustments;
    
    for (size_t i = 0; i < route.flights.size(); ++i) {
        Flight* flight = route.flights[i];
        Aircraft* aircraft = flight->getAircraft();
        
        if (!aircraft->hasCabinClass(seatType)) {
            // 飞机没有用户选择的舱位，需要降级
            SeatType adjustedClass = getDowngradedCabinClass(seatType, aircraft);
            
            if (adjustedClass == seatType) {
                // 无法降级（连经济舱都没有，理论上不应该发生）
                std::cout << "Error: Flight " << flight->getFlightNumber() 
                          << " does not have any suitable cabin class." << std::endl;
                return;
            }
            
            CabinAdjustment adj;
            adj.segmentIndex = i;
            adj.flight = flight;
            adj.originalClass = seatType;
            adj.adjustedClass = adjustedClass;
            adjustments.push_back(adj);
        }
    }
    
    // 如果有舱位需要调整，显示详情并让用户确认
    if (!adjustments.empty()) {
        std::cout << "\n========== CABIN CLASS ADJUSTMENT NOTICE ==========\n";
        std::cout << "Some flight segments do not offer " << seatTypeToString(seatType) << ".\n";
        std::cout << "The following adjustments will be applied:\n\n";
        
        for (const auto& adj : adjustments) {
            std::cout << "Segment " << (adj.segmentIndex + 1) << ": Flight " 
                      << adj.flight->getFlightNumber() 
                      << " (" << adj.flight->getOrigin() << " -> " << adj.flight->getDestination() << ")\n";
            std::cout << "  Aircraft: " << adj.flight->getAircraft()->getType() << "\n";
            std::cout << "  Original: " << seatTypeToString(adj.originalClass) << "\n";
            std::cout << "  Adjusted to: " << seatTypeToString(adj.adjustedClass) << "\n";
            
            // 检查降级后的舱位是否还有空座
            if (!hasAvailableSeats(adj.flight, adj.adjustedClass)) {
                std::cout << "  >>> WARNING: Adjusted cabin class is FULLY BOOKED! <<<\n";
            }
            std::cout << "\n";
        }
        
        // 再次检查是否所有降级舱位都有空座
        bool allAvailable = true;
        for (const auto& adj : adjustments) {
            if (!hasAvailableSeats(adj.flight, adj.adjustedClass)) {
                allAvailable = false;
                break;
            }
        }
        
        if (!allAvailable) {
            std::cout << "\nERROR: Some adjusted cabin classes are fully booked." << std::endl;
            std::cout << "This connecting flight route is not available at this time." << std::endl;
            std::cout << "Please try a different route or contact customer service." << std::endl;
            return;
        }
        
        std::cout << "Note: You will be charged for " << seatTypeToString(seatType) 
                  << " for segments that offer it,\n";
        std::cout << "      and adjusted pricing for segments with cabin downgrades.\n\n";
        
        bool confirm = getValidYesNoInput("Do you want to proceed with this booking? (Y/N): ");
        
        if (!confirm) {
            std::cout << "Booking cancelled." << std::endl;
            return;
        }
    }
    
    // 额外检查：即使没有降级，也要检查原始舱位是否有空座
    for (size_t i = 0; i < route.flights.size(); ++i) {
        Flight* flight = route.flights[i];
        SeatType checkSeatType = seatType;
        
        // 如果该航段有降级，检查降级后的舱位
        for (const auto& adj : adjustments) {
            if (adj.segmentIndex == i) {
                checkSeatType = adj.adjustedClass;
                break;
            }
        }
        
        if (!hasAvailableSeats(flight, checkSeatType)) {
            std::cout << "\nERROR: Flight " << flight->getFlightNumber() 
                      << " (" << flight->getOrigin() << " -> " << flight->getDestination() 
                      << ") has no available seats in " << seatTypeToString(checkSeatType) << "." << std::endl;
            std::cout << "Booking cancelled." << std::endl;
            return;
        }
    }
    
    // 计算总票价（考虑舱位调整）
    double totalPrice = 0.0;
    for (size_t i = 0; i < route.flights.size(); ++i) {
        Flight* flight = route.flights[i];
        SeatType actualSeatType = seatType;
        
        // 检查这段航班是否需要降级
        for (const auto& adj : adjustments) {
            if (adj.segmentIndex == i) {
                actualSeatType = adj.adjustedClass;
                break;
            }
        }
        
        totalPrice += calculateBasePrice(flight, actualSeatType);
    }
    
    // 联程票优惠：10% off
    totalPrice *= 0.9;
    
    // 应用会员折扣
    double discount = getLoyaltyDiscount(currentPassenger->getLoyaltyStatus());
    double finalPrice = totalPrice * (1.0 - discount);
    
    // 创建订单（使用第一段航班的信息作为主要信息）
    Flight* firstFlight = route.flights[0];
    Order* order = orderManager.createOrder(
        currentPassenger->getPassportNumber(),
        firstFlight->getFlightNumber(),
        firstFlight->getFlightDate(),
        seatType  // 保持原始用户选择的舱位作为订单主舱位
    );
    
    // 设置联程航班信息（包含实际舱位信息）
    std::vector<std::string> flightNumbers;
    std::vector<std::string> seatNumbers;
    for (size_t i = 0; i < route.flights.size(); ++i) {
        Flight* flight = route.flights[i];
        flightNumbers.push_back(flight->getFlightNumber());
        
        // 座位号稍后在值机时分配，但标记实际舱位
        SeatType actualSeatType = seatType;
        for (const auto& adj : adjustments) {
            if (adj.segmentIndex == i) {
                actualSeatType = adj.adjustedClass;
                break;
            }
        }
        // 先留空，值机时会填充实际座位号
        seatNumbers.push_back("");
    }
    order->setConnectingFlights(flightNumbers, seatNumbers);
    
    // 设置票价
    order->setTicketPrice(finalPrice);
    
    std::cout << "\n========== Connecting Flight Booking Successful! ==========" << std::endl;
    std::cout << "Order Number: " << order->getOrderNumber() << std::endl;
    std::cout << "Type: CONNECTING FLIGHT (" << route.flights.size() << " segments)" << std::endl;
    std::cout << "Route: " << firstFlight->getOrigin() << " -> " << route.flights.back()->getDestination() << std::endl;
    
    std::cout << "\nFlight Segments:" << std::endl;
    for (size_t i = 0; i < route.flights.size(); ++i) {
        Flight* flight = route.flights[i];
        SeatType actualSeatType = seatType;
        
        for (const auto& adj : adjustments) {
            if (adj.segmentIndex == i) {
                actualSeatType = adj.adjustedClass;
                break;
            }
        }
        
        std::cout << "  " << (i + 1) << ". " << flight->getFlightNumber() 
                  << " (" << flight->getOrigin() << " -> " << flight->getDestination() 
                  << ") - " << flight->getFlightDate();
        
        if (actualSeatType != seatType) {
            std::cout << " [" << seatTypeToString(actualSeatType) << "]";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nPrimary Cabin Class: " << seatTypeToString(seatType);
    if (!adjustments.empty()) {
        std::cout << " (with adjustments as noted above)";
    }
    std::cout << std::endl;
    
    // 显示价格信息
    std::cout << "\n--- Ticket Price ---" << std::endl;
    std::cout << "Base Price (all segments): $" << std::fixed << std::setprecision(2) 
              << (totalPrice / 0.9) << std::endl;
    std::cout << "Connecting Flight Discount: 10% OFF" << std::endl;
    std::cout << "Price after discount: $" << totalPrice << std::endl;
    
    if (discount > 0) {
        std::cout << "Member Discount: " << (discount * 100) << "% OFF" << std::endl;
        std::cout << "Additional discount: $" << (totalPrice * discount) << std::endl;
    }
    std::cout << "Final Price: $" << finalPrice << std::endl;
    
    std::cout << "\nStatus: PENDING PAYMENT" << std::endl;
    std::cout << "\nPlease proceed to payment (Option 2 in menu) to complete booking." << std::endl;
    std::cout << "Note: For connecting flights, you will check in for all segments together." << std::endl;
    
    // 保存订单数据
    orderManager.saveToCSV("data/orders.csv");
}

// 获取降级后的舱位
SeatType AirlineReservationSystem::getDowngradedCabinClass(SeatType originalClass, Aircraft* aircraft) {
    // 舱位优先级：头等舱 > 商务舱 > 超级经济舱 > 经济舱
    // 从用户选择的舱位开始往下找第一个可用的舱位
    
    switch (originalClass) {
        case SeatType::FIRST_CLASS:
            // 头等舱不可用，尝试商务舱
            if (aircraft->hasCabinClass(SeatType::BUSINESS_CLASS)) {
                return SeatType::BUSINESS_CLASS;
            }
            // 商务舱也不可用，尝试超级经济舱
            if (aircraft->hasCabinClass(SeatType::PREMIUM_ECONOMY)) {
                return SeatType::PREMIUM_ECONOMY;
            }
            // 最后尝试经济舱
            if (aircraft->hasCabinClass(SeatType::ECONOMY_CLASS)) {
                return SeatType::ECONOMY_CLASS;
            }
            break;
            
        case SeatType::BUSINESS_CLASS:
            // 商务舱不可用，尝试超级经济舱
            if (aircraft->hasCabinClass(SeatType::PREMIUM_ECONOMY)) {
                return SeatType::PREMIUM_ECONOMY;
            }
            // 最后尝试经济舱
            if (aircraft->hasCabinClass(SeatType::ECONOMY_CLASS)) {
                return SeatType::ECONOMY_CLASS;
            }
            break;
            
        case SeatType::PREMIUM_ECONOMY:
            // 超级经济舱不可用，尝试经济舱
            if (aircraft->hasCabinClass(SeatType::ECONOMY_CLASS)) {
                return SeatType::ECONOMY_CLASS;
            }
            break;
            
        case SeatType::ECONOMY_CLASS:
            // 经济舱是最低级，如果不可用则无法降级
            break;
    }
    
    // 如果没有找到可用的舱位，返回原舱位（理论上不应该发生）
    return originalClass;
}

// 将SeatType转换为字符串
std::string AirlineReservationSystem::seatTypeToString(SeatType seatType) {
    switch (seatType) {
        case SeatType::FIRST_CLASS:
            return "First Class";
        case SeatType::BUSINESS_CLASS:
            return "Business Class";
        case SeatType::PREMIUM_ECONOMY:
            return "Premium Economy";
        case SeatType::ECONOMY_CLASS:
            return "Economy Class";
        default:
            return "Unknown";
    }
}

// ==================== 候补队列相关函数 ====================

// 检查航班特定舱位是否还有空座
bool AirlineReservationSystem::hasAvailableSeats(Flight* flight, SeatType seatType) {
    if (!flight) return false;
    
    Aircraft* aircraft = flight->getAircraft();
    if (!aircraft) return false;
    
    // 检查该舱位是否存在
    if (!aircraft->hasCabinClass(seatType)) {
        return false;
    }
    
    // 统计该舱位已预订的座位数和总座位数
    int bookedSeats = 0;
    int totalSeats = 0;
    
    // 根据舱位类型确定行范围
    int startRow = 1;
    int endRow = 0;
    
    switch (seatType) {
        case SeatType::FIRST_CLASS:
            startRow = 1;
            endRow = aircraft->getFirstClassRows();
            break;
        case SeatType::BUSINESS_CLASS:
            startRow = aircraft->getFirstClassRows() + 1;
            endRow = startRow + aircraft->getBusinessClassRows() - 1;
            break;
        case SeatType::PREMIUM_ECONOMY:
            startRow = aircraft->getFirstClassRows() + aircraft->getBusinessClassRows() + 1;
            endRow = startRow + aircraft->getPremiumEconomyRows() - 1;
            break;
        case SeatType::ECONOMY_CLASS:
            startRow = aircraft->getFirstClassRows() + aircraft->getBusinessClassRows() + aircraft->getPremiumEconomyRows() + 1;
            endRow = startRow + aircraft->getEconomyClassRows() - 1;
            break;
    }
    
    // 遍历该舱位的所有座位
    for (int row = startRow; row <= endRow; ++row) {
        for (char col = 'A'; col < 'A' + aircraft->getSeatsPerRow(); ++col) {
            Seat* seat = aircraft->getSeat(row, col);
            if (seat) {
                totalSeats++;
                if (seat->isBooked()) {
                    bookedSeats++;
                }
            }
        }
    }
    
    // 如果还有空座，返回true
    return bookedSeats < totalSeats;
}

// 将乘客加入候补队列
void AirlineReservationSystem::addToWaitlist(Passenger* passenger, Flight* flight, SeatType seatType) {
    if (!passenger || !flight) return;
    
    // 根据会员等级计算优先级
    int priority = getLoyaltyPriority(passenger->getLoyaltyStatus());
    
    // 创建候补条目
    WaitlistEntry entry;
    entry.passenger = passenger;
    entry.flight = flight;
    entry.seatType = seatType;
    entry.priority = priority;
    
    // 加入优先队列
    waitlist.push(entry);
    
    std::cout << "\n========== Added to Waitlist ==========\n";
    std::cout << "Passenger: " << passenger->getName() << std::endl;
    std::cout << "Flight: " << flight->getFlightNumber() << std::endl;
    std::cout << "Date: " << flight->getFlightDate() << std::endl;
    std::cout << "Cabin: " << seatTypeToString(seatType) << std::endl;
    std::cout << "Loyalty Status: ";
    switch (passenger->getLoyaltyStatus()) {
        case LoyaltyStatus::PLATINUM: std::cout << "PLATINUM"; break;
        case LoyaltyStatus::GOLD: std::cout << "GOLD"; break;
        case LoyaltyStatus::SILVER: std::cout << "SILVER"; break;
        case LoyaltyStatus::NONE: std::cout << "NONE"; break;
    }
    std::cout << std::endl;
    std::cout << "Priority: " << priority << std::endl;
    std::cout << "\nYou will be automatically upgraded when a seat becomes available." << std::endl;
    std::cout << "We will notify you when your booking is confirmed." << std::endl;
    std::cout << "========================================\n";
    
    // 保存候补队列到文件
    saveWaitlistToCSV("data/waitlist.csv");
}

// 处理候补队列 - 当有座位空出时自动处理
void AirlineReservationSystem::processWaitlist(Flight* flight, SeatType seatType) {
    if (!flight || waitlist.empty()) return;
    
    // 临时存储不匹配的条目
    std::vector<WaitlistEntry> tempList;
    bool processed = false;
    
    while (!waitlist.empty()) {
        WaitlistEntry entry = waitlist.top();
        waitlist.pop();
        
        // 检查是否匹配当前航班和舱位
        if (entry.flight->getFlightNumber() == flight->getFlightNumber() &&
            entry.flight->getFlightDate() == flight->getFlightDate() &&
            entry.seatType == seatType &&
            hasAvailableSeats(flight, seatType)) {
            
            // 找到匹配的候补条目，自动创建订单
            std::cout << "\n========== Processing Waitlist ==========\n";
            std::cout << "Passenger: " << entry.passenger->getName() << std::endl;
            std::cout << "Passport: " << entry.passenger->getPassportNumber() << std::endl;
            std::cout << "Flight: " << flight->getFlightNumber() << std::endl;
            std::cout << "Date: " << flight->getFlightDate() << std::endl;
            std::cout << "Cabin: " << seatTypeToString(seatType) << std::endl;
            std::cout << "\nCreating order automatically..." << std::endl;
            
            // 创建订单
            Order* order = orderManager.createOrder(
                entry.passenger->getPassportNumber(),
                flight->getFlightNumber(),
                flight->getFlightDate(),
                seatType
            );
            
            // 计算票价
            double finalPrice = calculateFinalPrice(flight, seatType, entry.passenger->getLoyaltyStatus());
            order->setTicketPrice(finalPrice);
            
            std::cout << "\nOrder Number: " << order->getOrderNumber() << std::endl;
            std::cout << "Ticket Price: $" << std::fixed << std::setprecision(2) << finalPrice << std::endl;
            std::cout << "Status: PENDING PAYMENT" << std::endl;
            std::cout << "\nYour waitlist booking has been confirmed!" << std::endl;
            std::cout << "Please proceed to payment within 24 hours." << std::endl;
            std::cout << "========================================\n";
            
            // 保存订单
            orderManager.saveToCSV("data/orders.csv");
            
            processed = true;
            break; // 只处理一个候补（一个座位空出）
        } else {
            // 不匹配，暂存
            tempList.push_back(entry);
        }
    }
    
    // 将不匹配的条目放回队列
    for (const auto& entry : tempList) {
        waitlist.push(entry);
    }
    
    if (!processed && !tempList.empty()) {
        // 没有处理任何候补条目
        std::cout << "\nNo matching waitlist entries found for this flight and cabin." << std::endl;
    }
    
    // 保存候补队列到文件
    saveWaitlistToCSV("data/waitlist.csv");
}

// 查看候补队列（管理员功能）
void AirlineReservationSystem::viewWaitlist() {
    if (waitlist.empty()) {
        std::cout << "\nWaitlist is empty." << std::endl;
        return;
    }
    
    std::cout << "\n========== Current Waitlist ==========\n";
    std::cout << "Total entries: " << waitlist.size() << std::endl;
    std::cout << "\nNote: The queue is sorted by priority (higher priority listed first)\n";
    std::cout << "Priority levels: PLATINUM=4, GOLD=3, SILVER=2, NONE=1\n";
    std::cout << "======================================\n";
    
    // 由于priority_queue不支持遍历，我们需要复制一份来显示
    std::priority_queue<WaitlistEntry, std::vector<WaitlistEntry>, std::greater<WaitlistEntry>> tempQueue = waitlist;
    
    int index = 1;
    while (!tempQueue.empty()) {
        WaitlistEntry entry = tempQueue.top();
        tempQueue.pop();
        
        std::cout << "\n" << index++ << ". Passenger: " << entry.passenger->getName() 
                  << " (" << entry.passenger->getPassportNumber() << ")" << std::endl;
        std::cout << "   Flight: " << entry.flight->getFlightNumber() 
                  << " on " << entry.flight->getFlightDate() << std::endl;
        std::cout << "   Cabin: " << seatTypeToString(entry.seatType) << std::endl;
        std::cout << "   Loyalty: ";
        switch (entry.passenger->getLoyaltyStatus()) {
            case LoyaltyStatus::PLATINUM: std::cout << "PLATINUM"; break;
            case LoyaltyStatus::GOLD: std::cout << "GOLD"; break;
            case LoyaltyStatus::SILVER: std::cout << "SILVER"; break;
            case LoyaltyStatus::NONE: std::cout << "NONE"; break;
        }
        std::cout << " (Priority: " << entry.priority << ")" << std::endl;
    }
    
    std::cout << "\n======================================\n";
}

// 保存候补队列到CSV文件
void AirlineReservationSystem::saveWaitlistToCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    // 写入表头
    file << "passportNumber,flightNumber,flightDate,seatType,priority\n";
    
    // 由于priority_queue不支持遍历，需要临时复制
    std::priority_queue<WaitlistEntry, std::vector<WaitlistEntry>, std::greater<WaitlistEntry>> tempQueue = waitlist;
    
    while (!tempQueue.empty()) {
        WaitlistEntry entry = tempQueue.top();
        tempQueue.pop();
        
       file << entry.passenger->getPassportNumber() << ","
           << entry.flight->getFlightNumber() << ","
           << entry.flight->getFlightDate() << ","
           << Order::seatTypeToString(entry.seatType) << ","
           << entry.priority << "\n";
    }
    
    file.close();
}

// 从CSV文件加载候补队列
void AirlineReservationSystem::loadWaitlistFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // 文件不存在是正常的（首次运行）
        return;
    }
    
    std::string line;
    // 跳过表头
    std::getline(file, line);
    
    int loadedCount = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string passportNumber, flightNumber, flightDate, seatTypeStr, priorityStr;
        
        std::getline(ss, passportNumber, ',');
        std::getline(ss, flightNumber, ',');
        std::getline(ss, flightDate, ',');
        std::getline(ss, seatTypeStr, ',');
        std::getline(ss, priorityStr, ',');
        
        // 查找乘客
        Passenger* passenger = findPassenger(passportNumber);
        if (!passenger) continue;
        
        // 查找航班
        Flight* flight = nullptr;
        for (auto* f : flights) {
            if (f->getFlightNumber() == flightNumber && f->getFlightDate() == flightDate) {
                flight = f;
                break;
            }
        }
        if (!flight) continue;
        
    // 解析舱位类型（兼容旧格式 "First Class" 与新格式 "FIRST_CLASS"）
    SeatType seatType = SeatType::ECONOMY_CLASS;
    if (seatTypeStr == "FIRST_CLASS" || seatTypeStr == "First Class") seatType = SeatType::FIRST_CLASS;
    else if (seatTypeStr == "BUSINESS_CLASS" || seatTypeStr == "Business Class") seatType = SeatType::BUSINESS_CLASS;
    else if (seatTypeStr == "PREMIUM_ECONOMY" || seatTypeStr == "Premium Economy") seatType = SeatType::PREMIUM_ECONOMY;
        
        // 解析优先级
        int priority = std::stoi(priorityStr);
        
        // 创建候补条目
        WaitlistEntry entry;
        entry.passenger = passenger;
        entry.flight = flight;
        entry.seatType = seatType;
        entry.priority = priority;
        
        // 加入队列
        waitlist.push(entry);
        loadedCount++;
    }
    
    file.close();
    
    if (loadedCount > 0) {
        std::cout << "Successfully loaded " << loadedCount << " waitlist entries from " << filename << std::endl;
    }
}

// ========== 公共API实现 ==========

// 乘客登录
bool AirlineReservationSystem::loginPassenger(const std::string& passportNumber) {
    currentPassenger = findPassenger(passportNumber);
    return currentPassenger != nullptr;
}

// 乘客注册
bool AirlineReservationSystem::registerPassenger(const std::string& name, const std::string& passportNumber) {
    // 检查护照号是否已存在
    if (findPassenger(passportNumber) != nullptr) {
        return false;
    }
    
    // 创建新乘客
    currentPassenger = new Passenger(name, passportNumber);
    addPassenger(currentPassenger);
    
    // 保存乘客数据到CSV
    savePassengersData();
    return true;
}

// 搜索航班
std::vector<Flight*> AirlineReservationSystem::searchFlights(const std::string& origin, 
                                                              const std::string& destination, 
                                                              const std::string& date) {
    std::vector<Flight*> results;
    
    for (const auto& flight : flights) {
        if (flight->getOrigin() == origin && 
            flight->getDestination() == destination &&
            flight->getFlightDate() == date) {
            results.push_back(flight);
        }
    }
    
    return results;
}

// 创建预订
Order* AirlineReservationSystem::createBooking(const std::string& flightNumber, 
                                                const std::string& flightDate, 
                                                SeatType seatType) {
    if (!currentPassenger) {
        return nullptr;
    }
    // 查找航班
    Flight* flight = findFlight(flightNumber);
    if (!flight || flight->getFlightDate() != flightDate) {
        return nullptr;
    }

    // 舱位可用性检查，满则加入候补并返回空指针
    if (!hasAvailableSeats(flight, seatType)) {
        addToWaitlist(currentPassenger, flight, seatType);
        return nullptr;
    }

    // 创建订单
    Order* order = orderManager.createOrder(
        currentPassenger->getPassportNumber(),
        flight->getFlightNumber(),
        flightDate,
        seatType
    );

    if (!order) return nullptr;

    double price = calculateFinalPrice(flight, seatType, currentPassenger->getLoyaltyStatus());
    order->setTicketPrice(price);
    orderManager.saveToCSV("data/orders.csv");
    return order;
}

// 支付订单
bool AirlineReservationSystem::payOrder(const std::string& orderNumber, const std::string& creditCard) {
    Order* order = orderManager.getOrder(orderNumber);
    if (!order) {
        return false;
    }
    
    if (order->getStatus() != OrderStatus::PENDING_PAYMENT) {
        return false;
    }
    
    // 获取信用卡后4位
    std::string last4 = creditCard.length() >= 4 ? creditCard.substr(creditCard.length() - 4) : creditCard;
    
    order->processPayment(last4);
    orderManager.saveToCSV("data/orders.csv");
    
    return true;
}

// 值机选座
bool AirlineReservationSystem::checkInOrder(const std::string& orderNumber, const std::string& seatNumber) {
    Order* order = orderManager.getOrder(orderNumber);
    if (!order) {
        return false;
    }
    
    if (order->getStatus() != OrderStatus::PAID) {
        return false;
    }
    
    // 查找航班
    Flight* flight = findFlight(order->getFlightNumber());
    if (!flight) {
        return false;
    }
    
    // 检查座位是否可用
    if (seatNumber.length() < 2) {
        return false;
    }
    
    int row = std::stoi(seatNumber.substr(0, seatNumber.length() - 1));
    char col = seatNumber[seatNumber.length() - 1];
    
    Seat* seat = flight->getAircraft()->getSeat(row, col);
    if (!seat || seat->isBooked()) {
        return false;
    }
    
    // 预订座位
    if (flight->getAircraft()->bookSeat(row, col)) {
        order->setSeatNumber(seatNumber);
        order->checkIn();
        
        // 保存数据
        orderManager.saveToCSV("data/orders.csv");
        saveReservationsData();
        
        return true;
    }
    
    return false;
}

// 联程航班值机 (UI 批量提交座位号)
bool AirlineReservationSystem::checkInConnectingOrder(const std::string& orderNumber, const std::vector<std::string>& seatNumbers) {
    Order* order = orderManager.getOrder(orderNumber);
    if (!order) return false;
    if (!order->isConnectingFlight()) return false; // 必须是联程订单
    if (order->getStatus() != OrderStatus::PAID) return false; // 必须已付款

    std::vector<std::string> flightNums = order->getConnectingFlightNumbers();
    if (flightNums.empty()) return false;
    if (seatNumbers.size() != flightNums.size()) return false; // 座位数与航段数必须匹配

    std::vector<std::string> finalSeats;
    finalSeats.reserve(seatNumbers.size());

    // 遍历每一段进行座位校验与预订
    for (size_t i = 0; i < flightNums.size(); ++i) {
        const std::string& seatStr = seatNumbers[i];
        if (seatStr.length() < 2) return false;
        // 解析行列
        std::string rowPart = seatStr.substr(0, seatStr.length() - 1);
        char col = toupper(seatStr.back());
        int row = 0;
        try { row = std::stoi(rowPart); } catch (...) { return false; }
        Flight* flight = findFlight(flightNums[i]);
        if (!flight) return false;
        Seat* seat = flight->getAircraft()->getSeat(row, col);
        if (!seat || seat->isBooked()) return false;
        // 舱位匹配或降级舱位匹配处理
        SeatType requested = order->getSeatType();
        SeatType cabinToUse = requested;
        Aircraft* aircraft = flight->getAircraft();
        if (!aircraft->hasCabinClass(requested)) {
            cabinToUse = getDowngradedCabinClass(requested, aircraft); // 已有逻辑用于降级
        }
        if (seat->getType() != cabinToUse) return false; // 座位类型必须匹配调整后的舱位
        // 预订座位
        seat->book();
        finalSeats.push_back(seatStr);
        // 创建或更新预订记录 (每段单独一个 Reservation)
        std::string reservationNumber = order->getOrderNumber() + "_SEG" + std::to_string(i + 1);
        Reservation* reservation = new Reservation(reservationNumber, order->getOrderNumber(), currentPassenger, flight, seat);
        reservation->confirm();
        reservations[reservationNumber] = reservation;
    }

    // 更新订单：第一段座位设为主座位号
    order->setConnectingFlights(flightNums, finalSeats);
    if (!finalSeats.empty()) order->setSeatNumber(finalSeats[0]);
    order->setStatus(OrderStatus::CHECKED_IN);
    // 设置值机时间
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream timeStr;
    timeStr << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    order->setCheckInTime(timeStr.str());

    // 保存
    orderManager.saveToCSV("data/orders.csv");
    saveReservationsData();
    return true;
}

// 获取指定乘客的候补条目（复制 priority_queue 内容进行过滤）
std::vector<WaitlistEntry> AirlineReservationSystem::getPassengerWaitlist(const std::string& passportNumber) {
    std::vector<WaitlistEntry> result;
    auto temp = waitlist; // 复制
    while (!temp.empty()) {
        auto entry = temp.top(); temp.pop();
        if (entry.passenger && entry.passenger->getPassportNumber() == passportNumber) {
            result.push_back(entry);
        }
    }
    // 优先级高的排前（复制结构已经保证堆顶为最高优先），这里保持顺序即可
    return result;
}

// 按乘客+航班+日期+舱位移除候补条目
bool AirlineReservationSystem::removeFromWaitlist(const std::string& passportNumber, const std::string& flightNumber, const std::string& flightDate, SeatType seatType) {
    if (waitlist.empty()) return false;
    bool removed = false;
    std::vector<WaitlistEntry> temp;
    while (!waitlist.empty()) {
        auto entry = waitlist.top(); waitlist.pop();
        bool match = (entry.passenger && entry.passenger->getPassportNumber() == passportNumber &&
                      entry.flight && entry.flight->getFlightNumber() == flightNumber &&
                      entry.flight->getFlightDate() == flightDate &&
                      entry.seatType == seatType);
        if (match && !removed) {
            removed = true; // 丢弃该条
        } else {
            temp.push_back(entry);
        }
    }
    // 重新装回
    for (auto &e : temp) waitlist.push(e);
    if (removed) saveWaitlistToCSV("data/waitlist.csv");
    return removed;
}

// 取消订单
bool AirlineReservationSystem::cancelOrder(const std::string& orderNumber) {
    Order* order = orderManager.getOrder(orderNumber);
    if (!order) {
        return false;
    }
    
    // 如果已经值机,需要释放座位
    if (order->getStatus() == OrderStatus::CHECKED_IN && !order->getSeatNumber().empty()) {
        Flight* flight = findFlight(order->getFlightNumber());
        if (flight) {
            std::string seatNum = order->getSeatNumber();
            int row = std::stoi(seatNum.substr(0, seatNum.length() - 1));
            char col = seatNum[seatNum.length() - 1];
            flight->getAircraft()->unbookSeat(row, col);
        }
    }
    
    order->cancel();
    orderManager.saveToCSV("data/orders.csv");
    saveReservationsData();
    
    return true;
}

// 获取乘客订单
std::vector<Order*> AirlineReservationSystem::getPassengerOrders(const std::string& passportNumber) {
    return orderManager.getOrdersByPassport(passportNumber);
}

// 根据航班号获取航班
Flight* AirlineReservationSystem::getFlightByNumber(const std::string& flightNumber) {
    return findFlight(flightNumber);
}

// 价格预估：返回最终价格（已应用会员折扣）；若航班或乘客不可用返回 -1
double AirlineReservationSystem::quotePrice(const std::string& flightNumber, const std::string& flightDate, SeatType seatType) {
    if (!currentPassenger) return -1.0;
    Flight* flight = findFlight(flightNumber);
    if (!flight || flight->getFlightDate() != flightDate) return -1.0;
    return calculateFinalPrice(flight, seatType, currentPassenger->getLoyaltyStatus());
}

AirlineReservationSystem::PriceBreakdown AirlineReservationSystem::getPriceBreakdown(const std::string& flightNumber, const std::string& flightDate, SeatType seatType) {
    PriceBreakdown pb;
    Flight* target = nullptr;
    for (auto* f : flights) {
        if (f->getFlightNumber() == flightNumber && f->getFlightDate() == flightDate) { target = f; break; }
    }
    if (!target || !currentPassenger) return pb; // base/final 仍为默认-1
    double base = calculateBasePrice(target, seatType);
    double rate = getLoyaltyDiscount(currentPassenger->getLoyaltyStatus());
    double finalP = calculateFinalPrice(target, seatType, currentPassenger->getLoyaltyStatus());
    pb.basePrice = base;
    pb.discountRate = rate;
    pb.discountAmount = base * rate;
    pb.finalPrice = finalP;
    return pb;
}

// 验证管理员
bool AirlineReservationSystem::verifyAdmin(const std::string& username, const std::string& password) {
    return (username == "admin" && password == "admin123");
}

// 供 UI 演示：为当前登录乘客添加候补条目
bool AirlineReservationSystem::addToWaitlistForCurrent(const std::string& flightNumber, const std::string& flightDate, SeatType seatType) {
    if (!currentPassenger) return false;
    Flight* f = findFlight(flightNumber);
    if (!f || f->getFlightDate() != flightDate) return false;
    addToWaitlist(currentPassenger, f, seatType);
    return true;
}

// 运行控制台管理员系统
void AirlineReservationSystem::runAdminConsole() {
    std::cout << "\n=== Administrator Console ===" << std::endl;
    std::cout << "1. Login" << std::endl;
    std::cout << "2. Return to Mode Selection" << std::endl;
    int first = getValidIntInput("Enter your choice: ", 1, 2);
    if (first == 2) {
        // 启动新实例回到模式选择
        char exePath[MAX_PATH] = {0};
        #ifdef _WIN32
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        STARTUPINFOA si; ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si);
        PROCESS_INFORMATION pi; ZeroMemory(&pi, sizeof(pi));
        std::string cmd = std::string("\"") + exePath + "\" -chooser-topmost";
        char* cmdBuf = _strdup(cmd.c_str());
        if (CreateProcessA(exePath, cmdBuf, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
        if (cmdBuf) free(cmdBuf);
        // 关闭当前控制台并退出当前进程
        FreeConsole();
        ExitProcess(0);
        #endif
        return; // 非 Win32 情况
    }
    // 选择登录
    if (!adminLogin()) {
        std::cout << "Login failed. Returning to selection..." << std::endl;
        char exePath[MAX_PATH] = {0};
        #ifdef _WIN32
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        STARTUPINFOA si; ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si);
        PROCESS_INFORMATION pi; ZeroMemory(&pi, sizeof(pi));
        std::string cmd = std::string("\"") + exePath + "\" -chooser-topmost";
        char* cmdBuf = _strdup(cmd.c_str());
        if (CreateProcessA(exePath, cmdBuf, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
        if (cmdBuf) free(cmdBuf);
        // 关闭当前控制台并退出当前进程
        FreeConsole();
        ExitProcess(0);
        #endif
        return; // 非 Win32 情况
    }
    showAdminMenu();
}

// ========== UI 辅助公共接口（联程）实现 ==========
std::vector<ConnectingFlightRoute> AirlineReservationSystem::searchConnectingFlightsUI(
    const std::string& origin, const std::string& destination, const std::string& travelDate) {
    // 直接复用内部算法，UI 自行选择显示前几条
    return findConnectingFlights(origin, destination, travelDate);
}

Order* AirlineReservationSystem::createConnectingBookingFromRoute(const ConnectingFlightRoute& route, SeatType seatType) {
    if (!currentPassenger) return nullptr;
    if (route.flights.empty()) return nullptr;

    // 参考 console 版本的 bookConnectingFlight 实现，移除交互打印，返回 Order*
    struct CabinAdjustment { size_t segmentIndex; Flight* flight; SeatType originalClass; SeatType adjustedClass; };
    std::vector<CabinAdjustment> adjustments;

    // 收集需要降级的航段
    for (size_t i = 0; i < route.flights.size(); ++i) {
        Flight* flight = route.flights[i];
        if (!flight || !flight->getAircraft()) return nullptr;
        Aircraft* aircraft = flight->getAircraft();
        if (!aircraft->hasCabinClass(seatType)) {
            SeatType adjusted = getDowngradedCabinClass(seatType, aircraft);
            CabinAdjustment adj{ i, flight, seatType, adjusted };
            adjustments.push_back(adj);
        }
    }

    // 校验可用座位（按调整后舱位）
    for (size_t i = 0; i < route.flights.size(); ++i) {
        Flight* flight = route.flights[i];
        SeatType checkType = seatType;
        for (const auto& adj : adjustments) if (adj.segmentIndex == i) { checkType = adj.adjustedClass; break; }
        if (!hasAvailableSeats(flight, checkType)) {
            return nullptr; // 有段不可用
        }
    }

    // 计算总价（含联程优惠与会员折扣）
    double totalBase = 0.0;
    for (size_t i = 0; i < route.flights.size(); ++i) {
        Flight* flight = route.flights[i];
        SeatType actualType = seatType;
        for (const auto& adj : adjustments) if (adj.segmentIndex == i) { actualType = adj.adjustedClass; break; }
        totalBase += calculateBasePrice(flight, actualType);
    }
    double afterConnectingDiscount = totalBase * 0.9; // 10% off 联程
    double finalPrice = afterConnectingDiscount * (1.0 - getLoyaltyDiscount(currentPassenger->getLoyaltyStatus()));

    // 创建订单：以第一段为主
    Flight* firstFlight = route.flights.front();
    Order* order = orderManager.createOrder(
        currentPassenger->getPassportNumber(),
        firstFlight->getFlightNumber(),
        firstFlight->getFlightDate(),
        seatType
    );
    if (!order) return nullptr;

    // 保存联程信息（座位在值机时填写）
    std::vector<std::string> flightNumbers; flightNumbers.reserve(route.flights.size());
    std::vector<std::string> seatNumbers; seatNumbers.reserve(route.flights.size());
    for (auto* f : route.flights) {
        if (!f) { return nullptr; }
        flightNumbers.push_back(f->getFlightNumber());
        seatNumbers.push_back("");
    }
    order->setConnectingFlights(flightNumbers, seatNumbers);
    order->setTicketPrice(finalPrice);

    // 持久化
    orderManager.saveToCSV("data/orders.csv");
    return order;
}

