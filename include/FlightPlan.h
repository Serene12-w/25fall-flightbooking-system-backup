#ifndef FLIGHT_PLAN_H
#define FLIGHT_PLAN_H

#include "Flight.h"
#include <string>
#include <vector>

// 飞行计划类
class FlightPlan {
public:
    // 构造函数 - 需要传入所有机场列表用于查找备降机场
    FlightPlan(Flight* flight, const std::vector<Airport*>& airports);

    // 显示飞行计划
    void display() const;

    // 显示飞行计划(带乘客数量)
    void displayWithPassengerCount(int passengerCount) const;

private:
    Flight* flight; // 关联的航班
    std::vector<Airport*> airports; // 所有机场列表
    
    // 辅助方法: 查找最近的备降机场
    Airport* findNearestAlternateAirport(Airport* destination) const;
};

#endif // FLIGHT_PLAN_H
