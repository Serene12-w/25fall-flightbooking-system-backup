#include "Order.h"
#include <ctime>
#include <iomanip>
#include <sstream>

// 获取当前时间字符串
static std::string getCurrentTimeString() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// 构造函数
Order::Order(const std::string& orderNumber, const std::string& passportNumber, 
             const std::string& flightNumber, const std::string& flightDate,
             const std::string& seatNumber, OrderStatus status, SeatType seatType)
    : orderNumber(orderNumber), passportNumber(passportNumber), 
      flightNumber(flightNumber), flightDate(flightDate),
      seatNumber(seatNumber), status(status), seatType(seatType),
      createTime(getCurrentTimeString()), paymentTime(""), checkInTime(""), creditCardLast4(""), ticketPrice(0.0) {}

// 获取订单号
std::string Order::getOrderNumber() const {
    return orderNumber;
}

// 获取乘客护照号
std::string Order::getPassportNumber() const {
    return passportNumber;
}

// 获取航班号
std::string Order::getFlightNumber() const {
    return flightNumber;
}

// 获取航班日期
std::string Order::getFlightDate() const {
    return flightDate;
}

// 获取座位号
std::string Order::getSeatNumber() const {
    return seatNumber;
}

// 设置座位号
void Order::setSeatNumber(const std::string& seat) {
    seatNumber = seat;
}

// 获取仓位等级
SeatType Order::getSeatType() const {
    return seatType;
}

// 设置仓位等级
void Order::setSeatType(SeatType type) {
    seatType = type;
}

// 获取订单状态
OrderStatus Order::getStatus() const {
    return status;
}

// 设置订单状态
void Order::setStatus(OrderStatus newStatus) {
    status = newStatus;
}

// 获取创建时间
std::string Order::getCreateTime() const {
    return createTime;
}

// 获取支付时间
std::string Order::getPaymentTime() const {
    return paymentTime;
}

// 设置支付时间
void Order::setPaymentTime(const std::string& time) {
    paymentTime = time;
}

// 获取值机时间
std::string Order::getCheckInTime() const {
    return checkInTime;
}

// 设置值机时间
void Order::setCheckInTime(const std::string& time) {
    checkInTime = time;
}

// 获取信用卡号(仅显示后4位)
std::string Order::getCreditCardLast4() const {
    return creditCardLast4;
}

// 设置信用卡号
void Order::setCreditCard(const std::string& cardNumber) {
    if (cardNumber.length() >= 4) {
        creditCardLast4 = cardNumber.substr(cardNumber.length() - 4);
    } else {
        creditCardLast4 = cardNumber;
    }
}

// 获取票价
double Order::getTicketPrice() const {
    return ticketPrice;
}

// 设置票价
void Order::setTicketPrice(double price) {
    ticketPrice = price;
}

// 检查是否为联程航班
bool Order::isConnectingFlight() const {
    return !connectingFlightNumbers.empty();
}

// 设置联程航班信息
void Order::setConnectingFlights(const std::vector<std::string>& flightNumbers, const std::vector<std::string>& seatNumbers) {
    connectingFlightNumbers = flightNumbers;
    connectingSeatNumbers = seatNumbers;
}

// 获取联程航班号列表
std::vector<std::string> Order::getConnectingFlightNumbers() const {
    return connectingFlightNumbers;
}

// 获取联程座位号列表
std::vector<std::string> Order::getConnectingSeatNumbers() const {
    return connectingSeatNumbers;
}

// 将订单状态转换为字符串
std::string Order::statusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::PENDING_PAYMENT:
            return "PENDING_PAYMENT";
        case OrderStatus::PAID:
            return "PAID";
        case OrderStatus::CHECKED_IN:
            return "CHECKED_IN";
        case OrderStatus::CANCELLED:
            return "CANCELLED";
        default:
            return "UNKNOWN";
    }
}

// 将字符串转换为订单状态
OrderStatus Order::stringToStatus(const std::string& str) {
    if (str == "PENDING_PAYMENT") return OrderStatus::PENDING_PAYMENT;
    if (str == "PAID") return OrderStatus::PAID;
    if (str == "CHECKED_IN") return OrderStatus::CHECKED_IN;
    if (str == "CANCELLED") return OrderStatus::CANCELLED;
    return OrderStatus::PENDING_PAYMENT;
}

// 将仓位等级转换为字符串
std::string Order::seatTypeToString(SeatType type) {
    switch (type) {
        case SeatType::FIRST_CLASS:
            return "FIRST_CLASS";
        case SeatType::BUSINESS_CLASS:
            return "BUSINESS_CLASS";
        case SeatType::PREMIUM_ECONOMY:
            return "PREMIUM_ECONOMY";
        case SeatType::ECONOMY_CLASS:
            return "ECONOMY_CLASS";
        default:
            return "ECONOMY_CLASS";
    }
}

// 将字符串转换为仓位等级
SeatType Order::stringToSeatType(const std::string& str) {
    if (str == "FIRST_CLASS") return SeatType::FIRST_CLASS;
    if (str == "BUSINESS_CLASS") return SeatType::BUSINESS_CLASS;
    if (str == "PREMIUM_ECONOMY") return SeatType::PREMIUM_ECONOMY;
    if (str == "ECONOMY_CLASS") return SeatType::ECONOMY_CLASS;
    return SeatType::ECONOMY_CLASS;
}

// 处理支付
void Order::processPayment(const std::string& creditCard) {
    this->creditCardLast4 = creditCard;
    this->status = OrderStatus::PAID;
    this->paymentTime = getCurrentTimeString();
}

// 办理值机
void Order::checkIn() {
    if (this->status == OrderStatus::PAID) {
        this->status = OrderStatus::CHECKED_IN;
        this->checkInTime = getCurrentTimeString();
    }
}

// 取消订单
void Order::cancel() {
    this->status = OrderStatus::CANCELLED;
}

