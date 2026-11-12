#ifndef FLIGHT_CALCULATOR_H
#define FLIGHT_CALCULATOR_H

#include "Airport.h"
#include "Aircraft.h"
#include <cmath>

// 飞行计算器类 - 用于计算距离、燃油等
class FlightCalculator {
public:
    // 使用Haversine公式计算两个机场之间的大圆距离(公里)
    static double calculateDistance(const Airport* origin, const Airport* destination);

    // 计算航班所需燃油(公斤)
    // 参数: 距离(km), 飞机燃油系数(kg/km), 乘客数量
    static double calculateFuelRequired(double distanceKm, double fuelCoefficient, int passengerCount);

    // 计算起飞重量(公斤)
    // 参数: 飞机空重(kg), 燃油重量(kg), 乘客数量, 货物重量(kg)
    static double calculateTakeoffWeight(double emptyWeight, double fuelWeight, int passengerCount, double cargoWeight = 0.0);

    // 计算预计飞行时间(小时)
    // 参数: 距离(km), 巡航速度(km/h)
    static double calculateFlightTime(double distanceKm, double cruiseSpeedKmh);

private:
    // 地球平均半径(公里)
    static constexpr double EARTH_RADIUS_KM = 6371.0;

    // 平均乘客重量(包括行李) (公斤)
    static constexpr double AVG_PASSENGER_WEIGHT_KG = 100.0;

    // 将角度转换为弧度
    static double toRadians(double degrees);
};

#endif // FLIGHT_CALCULATOR_H
