#ifndef PASSENGER_H
#define PASSENGER_H

#include <string>
#include "Utils.h"

// 乘客类
class Passenger {
public:
    // 构造函数
    Passenger(const std::string& name, const std::string& passportNumber, LoyaltyStatus loyaltyStatus = LoyaltyStatus::NONE);

    // 获取乘客姓名
    std::string getName() const;

    // 获取护照号
    std::string getPassportNumber() const;

    // 获取常旅客状态
    LoyaltyStatus getLoyaltyStatus() const;

    // 设置常旅客状态
    void setLoyaltyStatus(LoyaltyStatus status);

private:
    std::string name;           // 乘客姓名
    std::string passportNumber; // 护照号
    LoyaltyStatus loyaltyStatus; // 常旅客状态
};

#endif // PASSENGER_H
