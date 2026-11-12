#include "Passenger.h"

// 构造函数
Passenger::Passenger(const std::string& name, const std::string& passportNumber, LoyaltyStatus loyaltyStatus)
    : name(name), passportNumber(passportNumber), loyaltyStatus(loyaltyStatus) {}

// 获取乘客姓名
std::string Passenger::getName() const {
    return name;
}

// 获取护照号
std::string Passenger::getPassportNumber() const {
    return passportNumber;
}

// 获取常旅客状态
LoyaltyStatus Passenger::getLoyaltyStatus() const {
    return loyaltyStatus;
}

// 设置常旅客状态
void Passenger::setLoyaltyStatus(LoyaltyStatus status) {
    loyaltyStatus = status;
}
