#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <vector>

// 定义座位类型
enum class SeatType {
    FIRST_CLASS,        // 头等舱
    BUSINESS_CLASS,     // 商务舱
    PREMIUM_ECONOMY,    // 超级经济舱
    ECONOMY_CLASS       // 经济舱
};

// 定义预订状态
enum class ReservationStatus {
    PENDING,    // 待确认
    CONFIRMED,  // 已确认
    CANCELLED,  // 已取消
    WAITLISTED  // 候补
};

// 定义常旅客等级
enum class LoyaltyStatus {
    NONE,
    SILVER,
    GOLD,
    PLATINUM
};

#endif // UTILS_H
