#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <vector>
#include <string>
#include "Seat.h"

// 飞机类
class Aircraft {
public:
    // 构造函数
    Aircraft(const std::string& type, int firstClassRows, int businessClassRows, int premiumEconomyRows, int economyClassRows, int seatsPerRow);

    // 拷贝构造函数 - 创建独立的座位副本
    Aircraft(const Aircraft& other);

    // 获取飞机型号
    std::string getType() const;

    // 显示座位图
    void displaySeatMap() const;

    // 预订座位
    bool bookSeat(int row, char col);

    // 取消预订座位
    bool unbookSeat(int row, char col);

    // 获取座位
    Seat* getSeat(int row, char col);

    // 获取总座位数
    int getTotalSeats() const;

    // 按类型获取座位数
    int getSeatCountByType(SeatType type) const;

    // 获取舱位配置信息 (用于保存到CSV)
    int getFirstClassRows() const { return firstClassRows; }
    int getBusinessClassRows() const { return businessClassRows; }
    int getPremiumEconomyRows() const { return premiumEconomyRows; }
    int getEconomyClassRows() const { return economyClassRows; }
    int getSeatsPerRow() const { return seatsPerRow; }

    // 设置和获取飞机性能参数
    void setEmptyWeight(double weight) { emptyWeightKg = weight; }
    double getEmptyWeight() const { return emptyWeightKg; }
    
    void setFuelCoefficient(double coeff) { fuelCoefficientKgPerKm = coeff; }
    double getFuelCoefficient() const { return fuelCoefficientKgPerKm; }
    
    void setCruiseSpeed(double speed) { cruiseSpeedKmh = speed; }
    double getCruiseSpeed() const { return cruiseSpeedKmh; }

    void setMaxRange(double range) { maxRangeKm = range; }
    double getMaxRange() const { return maxRangeKm; }

    // 检查飞机是否有特定舱位
    bool hasCabinClass(SeatType seatType) const;

private:
    std::string type; // 飞机型号
    std::vector<std::vector<Seat>> seats; // 座位图 (二维数组)
    int firstClassRows;
    int businessClassRows;
    int premiumEconomyRows;
    int economyClassRows;
    int seatsPerRow;
    
    // 飞机性能参数
    double emptyWeightKg;           // 飞机空重(kg)
    double fuelCoefficientKgPerKm;  // 燃油系数(kg/km)
    double cruiseSpeedKmh;          // 巡航速度(km/h)
    double maxRangeKm;              // 最大航程(km)
};

#endif // AIRCRAFT_H
