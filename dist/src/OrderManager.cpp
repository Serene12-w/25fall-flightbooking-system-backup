#include "OrderManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

// 构造函数
OrderManager::OrderManager() : orderCounter(1) {
    // 尝试从文件加载订单计数器
    loadCounterFromFile();
}

// 析构函数
OrderManager::~OrderManager() {
    // 保存订单计数器到文件
    saveCounterToFile();
    clear();
}

// 加载订单计数器
void OrderManager::loadCounterFromFile() {
    std::ifstream file("data/order_counter.txt");
    if (file.is_open()) {
        file >> orderCounter;
        file.close();
        std::cout << "Loaded order counter: " << orderCounter << std::endl;
    }
}

// 保存订单计数器
void OrderManager::saveCounterToFile() {
    std::ofstream file("data/order_counter.txt");
    if (file.is_open()) {
        file << orderCounter;
        file.close();
    }
}

// 生成唯一订单号
std::string OrderManager::generateOrderNumber() {
    // 格式: ORD + YYYYMMDD + 流水号(5位)
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    
    std::ostringstream oss;
    oss << "ORD" 
        << std::put_time(localTime, "%Y%m%d")
        << std::setw(5) << std::setfill('0') << orderCounter++;
    
    // 保存更新后的计数器
    saveCounterToFile();
    
    return oss.str();
}

// 创建新订单
Order* OrderManager::createOrder(const std::string& passportNumber, 
                                const std::string& flightNumber, 
                                const std::string& flightDate, SeatType seatType) {
    std::string orderNumber = generateOrderNumber();
    Order* order = new Order(orderNumber, passportNumber, flightNumber, flightDate, "", OrderStatus::PENDING_PAYMENT, seatType);
    orders[orderNumber] = order;
    return order;
}

// 根据订单号获取订单
Order* OrderManager::getOrder(const std::string& orderNumber) {
    auto it = orders.find(orderNumber);
    if (it != orders.end()) {
        return it->second;
    }
    return nullptr;
}

// 获取某个乘客的所有订单
std::vector<Order*> OrderManager::getOrdersByPassport(const std::string& passportNumber) {
    std::vector<Order*> result;
    for (auto& pair : orders) {
        if (pair.second->getPassportNumber() == passportNumber) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// 获取某个航班的所有订单
std::vector<Order*> OrderManager::getOrdersByFlight(const std::string& flightNumber) {
    std::vector<Order*> result;
    for (auto& pair : orders) {
        if (pair.second->getFlightNumber() == flightNumber && 
            pair.second->getStatus() != OrderStatus::CANCELLED) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// 更新订单
bool OrderManager::updateOrder(Order* order) {
    if (!order) return false;
    
    std::string orderNumber = order->getOrderNumber();
    auto it = orders.find(orderNumber);
    if (it != orders.end()) {
        // 订单已存在，更新成功
        return true;
    }
    return false;
}

// 取消订单
bool OrderManager::cancelOrder(const std::string& orderNumber) {
    Order* order = getOrder(orderNumber);
    if (order) {
        order->setStatus(OrderStatus::CANCELLED);
        return true;
    }
    return false;
}

// 从CSV文件加载订单
bool OrderManager::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Warning: Could not open orders file: " << filename << std::endl;
        return false;
    }

    std::string line;
    // 跳过表头
    std::getline(file, line);

    int maxCounter = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string orderNumber, passportNumber, flightNumber, flightDate;
        std::string seatNumber, seatTypeStr, statusStr, createTime, paymentTime, checkInTime, cardLast4, priceStr;
        std::string connectingFlightsStr, connectingSeatsStr;

        std::getline(ss, orderNumber, ',');
        std::getline(ss, passportNumber, ',');
        std::getline(ss, flightNumber, ',');
        std::getline(ss, flightDate, ',');
        std::getline(ss, seatNumber, ',');
        std::getline(ss, seatTypeStr, ',');
        std::getline(ss, statusStr, ',');
        std::getline(ss, createTime, ',');
        std::getline(ss, paymentTime, ',');
        std::getline(ss, checkInTime, ',');
        std::getline(ss, cardLast4, ',');
        std::getline(ss, priceStr, ',');
        
        // 读取联程航班信息（可能包含引号）
        std::getline(ss, connectingFlightsStr, ',');
        std::getline(ss, connectingSeatsStr, ',');
        
        // 移除引号
        auto removeQuotes = [](std::string& str) {
            if (!str.empty() && str.front() == '"') str.erase(0, 1);
            if (!str.empty() && str.back() == '"') str.pop_back();
        };
        removeQuotes(connectingFlightsStr);
        removeQuotes(connectingSeatsStr);

        // 创建订单对象
        OrderStatus status = Order::stringToStatus(statusStr);
        SeatType seatType = seatTypeStr.empty() ? SeatType::ECONOMY_CLASS : Order::stringToSeatType(seatTypeStr);
        Order* order = new Order(orderNumber, passportNumber, flightNumber, flightDate, 
                                seatNumber, status, seatType);
        
        // 设置时间信息
        if (!paymentTime.empty()) order->setPaymentTime(paymentTime);
        if (!checkInTime.empty()) order->setCheckInTime(checkInTime);
        if (!cardLast4.empty()) order->setCreditCard("************" + cardLast4);
        
        // 设置票价
        if (!priceStr.empty()) {
            try {
                order->setTicketPrice(std::stod(priceStr));
            } catch (...) {
                order->setTicketPrice(0.0);
            }
        }
        
        // 解析联程航班信息
        if (!connectingFlightsStr.empty()) {
            std::vector<std::string> flightNumbers;
            std::vector<std::string> seatNumbers;
            
            // 解析航班号（用分号分隔）
            std::stringstream flightStream(connectingFlightsStr);
            std::string flight;
            while (std::getline(flightStream, flight, ';')) {
                if (!flight.empty()) {
                    flightNumbers.push_back(flight);
                }
            }
            
            // 解析座位号（用分号分隔）
            std::stringstream seatStream(connectingSeatsStr);
            std::string seat;
            while (std::getline(seatStream, seat, ';')) {
                seatNumbers.push_back(seat); // 允许空座位号
            }
            
            // 确保两个向量大小一致
            while (seatNumbers.size() < flightNumbers.size()) {
                seatNumbers.push_back("");
            }
            
            if (!flightNumbers.empty()) {
                order->setConnectingFlights(flightNumbers, seatNumbers);
            }
        }

        orders[orderNumber] = order;

        // 更新计数器
        if (orderNumber.length() >= 8 && orderNumber.substr(0, 3) == "ORD") {
            try {
                int counter = std::stoi(orderNumber.substr(11)); // 提取流水号
                if (counter > maxCounter) maxCounter = counter;
            } catch (...) {
                // 忽略解析错误
            }
        }
    }

    orderCounter = maxCounter + 1;
    file.close();
    std::cout << "Successfully loaded " << orders.size() << " orders from " << filename << std::endl;
    return true;
}

// 保存订单到CSV文件
bool OrderManager::saveToCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    // 写入表头（添加联程航班字段）
    file << "orderNumber,passportNumber,flightNumber,flightDate,seatNumber,seatType,status,createTime,paymentTime,checkInTime,creditCardLast4,ticketPrice,connectingFlights,connectingSeats\n";

    // 写入订单数据
    for (const auto& pair : orders) {
        Order* order = pair.second;
        file << order->getOrderNumber() << ","
             << order->getPassportNumber() << ","
             << order->getFlightNumber() << ","
             << order->getFlightDate() << ","
             << order->getSeatNumber() << ","
             << Order::seatTypeToString(order->getSeatType()) << ","
             << Order::statusToString(order->getStatus()) << ","
             << order->getCreateTime() << ","
             << order->getPaymentTime() << ","
             << order->getCheckInTime() << ","
             << order->getCreditCardLast4() << ","
             << std::fixed << std::setprecision(2) << order->getTicketPrice() << ",";
        
        // 保存联程航班信息
        if (order->isConnectingFlight()) {
            std::vector<std::string> flightNumbers = order->getConnectingFlightNumbers();
            std::vector<std::string> seatNumbers = order->getConnectingSeatNumbers();
            
            // 用分号分隔多个航班号
            file << "\"";
            for (size_t i = 0; i < flightNumbers.size(); ++i) {
                if (i > 0) file << ";";
                file << flightNumbers[i];
            }
            file << "\",\"";
            
            // 用分号分隔多个座位号
            for (size_t i = 0; i < seatNumbers.size(); ++i) {
                if (i > 0) file << ";";
                file << seatNumbers[i];
            }
            file << "\"";
        } else {
            file << ",";
        }
        
        file << "\n";
    }

    file.close();
    std::cout << "Successfully saved " << orders.size() << " orders to " << filename << std::endl;
    return true;
}

// 获取所有订单
const std::unordered_map<std::string, Order*>& OrderManager::getAllOrders() const {
    return orders;
}

// 清空所有订单
void OrderManager::clear() {
    for (auto& pair : orders) {
        delete pair.second;
    }
    orders.clear();
    orderCounter = 1;
}
