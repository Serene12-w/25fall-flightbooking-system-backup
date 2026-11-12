#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <ctime>
#include <vector>
#include "Utils.h"

// 订单状态枚举
enum class OrderStatus {
    PENDING_PAYMENT,    // 待付款
    PAID,              // 已付款(已出票)
    CHECKED_IN,        // 已值机
    CANCELLED          // 已取消
};

// 订单类
class Order {
public:
    // 构造函数
    Order(const std::string& orderNumber, const std::string& passportNumber, 
          const std::string& flightNumber, const std::string& flightDate,
          const std::string& seatNumber = "", OrderStatus status = OrderStatus::PENDING_PAYMENT,
          SeatType seatType = SeatType::ECONOMY_CLASS);

    // 获取订单号
    std::string getOrderNumber() const;

    // 获取乘客护照号
    std::string getPassportNumber() const;

    // 获取航班号
    std::string getFlightNumber() const;

    // 获取航班日期
    std::string getFlightDate() const;

    // 获取座位号
    std::string getSeatNumber() const;

    // 设置座位号
    void setSeatNumber(const std::string& seat);

    // 获取/设置仓位等级
    SeatType getSeatType() const;
    void setSeatType(SeatType type);

    // 获取订单状态
    OrderStatus getStatus() const;

    // 设置订单状态
    void setStatus(OrderStatus status);

    // 获取创建时间
    std::string getCreateTime() const;

    // 获取支付时间
    std::string getPaymentTime() const;

    // 设置支付时间
    void setPaymentTime(const std::string& time);

    // 获取值机时间
    std::string getCheckInTime() const;

    // 设置值机时间
    void setCheckInTime(const std::string& time);

    // 获取信用卡号(仅显示后4位)
    std::string getCreditCardLast4() const;

    // 设置信用卡号
    void setCreditCard(const std::string& cardNumber);

    // 获取票价
    double getTicketPrice() const;

    // 设置票价
    void setTicketPrice(double price);
    
    // 联程航班相关方法
    bool isConnectingFlight() const;
    void setConnectingFlights(const std::vector<std::string>& flightNumbers, const std::vector<std::string>& seatNumbers);
    std::vector<std::string> getConnectingFlightNumbers() const;
    std::vector<std::string> getConnectingSeatNumbers() const;

    // 将订单状态转换为字符串
    static std::string statusToString(OrderStatus status);

    // 将字符串转换为订单状态
    static OrderStatus stringToStatus(const std::string& str);

    // 将仓位等级转换为字符串
    static std::string seatTypeToString(SeatType type);

    // 将字符串转换为仓位等级
    static SeatType stringToSeatType(const std::string& str);

    // 处理支付
    void processPayment(const std::string& creditCard);

    // 办理值机
    void checkIn();

    // 取消订单
    void cancel();

private:
    std::string orderNumber;     // 订单号
    std::string passportNumber;  // 乘客护照号
    std::string flightNumber;    // 航班号（主航班或第一段）
    std::string flightDate;      // 航班日期
    std::string seatNumber;      // 座位号（主航班或第一段）
    SeatType seatType;           // 仓位等级
    OrderStatus status;          // 订单状态
    std::string createTime;      // 创建时间
    std::string paymentTime;     // 支付时间
    std::string checkInTime;     // 值机时间
    std::string creditCardLast4; // 信用卡后4位
    double ticketPrice;          // 票价(美元)
    
    // 联程航班支持
    std::vector<std::string> connectingFlightNumbers;  // 联程航班号列表（包含所有航班）
    std::vector<std::string> connectingSeatNumbers;    // 对应的座位号列表
};

#endif // ORDER_H
