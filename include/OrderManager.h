#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include "Order.h"
#include <unordered_map>
#include <vector>
#include <string>

// 订单管理器类 - 使用哈希表管理所有订单
class OrderManager {
public:
    // 构造函数
    OrderManager();

    // 析构函数
    ~OrderManager();

    // 生成唯一订单号
    std::string generateOrderNumber();

    // 创建新订单
    Order* createOrder(const std::string& passportNumber, const std::string& flightNumber, 
                      const std::string& flightDate, SeatType seatType = SeatType::ECONOMY_CLASS);

    // 根据订单号获取订单
    Order* getOrder(const std::string& orderNumber);

    // 获取某个乘客的所有订单
    std::vector<Order*> getOrdersByPassport(const std::string& passportNumber);

    // 获取某个航班的所有订单
    std::vector<Order*> getOrdersByFlight(const std::string& flightNumber);

    // 更新订单
    bool updateOrder(Order* order);

    // 取消订单
    bool cancelOrder(const std::string& orderNumber);

    // 从CSV文件加载订单
    bool loadFromCSV(const std::string& filename);

    // 保存订单到CSV文件
    bool saveToCSV(const std::string& filename);

    // 获取所有订单
    const std::unordered_map<std::string, Order*>& getAllOrders() const;

    // 清空所有订单
    void clear();

private:
    std::unordered_map<std::string, Order*> orders; // 订单哈希表，key为订单号
    int orderCounter; // 订单计数器，用于生成订单号
    
    // 加载/保存订单计数器
    void loadCounterFromFile();
    void saveCounterToFile();
};

#endif // ORDER_MANAGER_H
