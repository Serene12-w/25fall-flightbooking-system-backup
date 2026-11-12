#include "FlightCalculator.h"
#include <cmath>

float const M_PI=3.14159265358979323846;
// 将角度转换为弧度
double FlightCalculator::toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

// 使用Haversine公式计算两个机场之间的大圆距离(公里)
double FlightCalculator::calculateDistance(const Airport* origin, const Airport* destination) {
    if (!origin || !destination) {
        return 0.0;
    }

    double lat1 = toRadians(origin->getLatitude());
    double lon1 = toRadians(origin->getLongitude());
    double lat2 = toRadians(destination->getLatitude());
    double lon2 = toRadians(destination->getLongitude());

    // Haversine公式
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dLon / 2) * std::sin(dLon / 2);

    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    double distance = EARTH_RADIUS_KM * c;

    return distance;
}

// 计算航班所需燃油(公斤)
double FlightCalculator::calculateFuelRequired(double distanceKm, double fuelCoefficient, int passengerCount) {
    // 基础燃油 = 距离 × 燃油系数
    double baseFuel = distanceKm * fuelCoefficient;

    // 考虑乘客负载的额外燃油消耗(每个乘客增加0.5%的燃油消耗)
    double loadFactor = 1.0 + (passengerCount * 0.005);

    // 增加10%的应急和备降燃油
    double totalFuel = baseFuel * loadFactor * 1.10;

    return totalFuel;
}

// 计算起飞重量(公斤)
double FlightCalculator::calculateTakeoffWeight(double emptyWeight, double fuelWeight, int passengerCount, double cargoWeight) {
    // 起飞重量 = 空重 + 燃油重量 + (乘客数 × 平均乘客重量) + 货物重量
    double totalWeight = emptyWeight + fuelWeight + (passengerCount * AVG_PASSENGER_WEIGHT_KG) + cargoWeight;

    return totalWeight;
}

// 计算预计飞行时间(小时)
double FlightCalculator::calculateFlightTime(double distanceKm, double cruiseSpeedKmh) {
    if (cruiseSpeedKmh <= 0) {
        return 0.0;
    }

    // 飞行时间 = 距离 / 巡航速度
    // 增加15%用于爬升、下降和空中交通管制
    double flightTime = (distanceKm / cruiseSpeedKmh) * 1.15;

    return flightTime;
}
