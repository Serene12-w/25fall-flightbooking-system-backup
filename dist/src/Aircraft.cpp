#include "Aircraft.h"
#include <iostream>

// 构造函数
Aircraft::Aircraft(const std::string& type, int firstClassRows, int businessClassRows, int premiumEconomyRows, int economyClassRows, int seatsPerRow)
    : type(type), firstClassRows(firstClassRows), businessClassRows(businessClassRows), premiumEconomyRows(premiumEconomyRows), economyClassRows(economyClassRows), seatsPerRow(seatsPerRow),
      emptyWeightKg(0.0), fuelCoefficientKgPerKm(0.0), cruiseSpeedKmh(0.0), maxRangeKm(0.0) {
    // 初始化座位
    int totalRows = firstClassRows + businessClassRows + premiumEconomyRows + economyClassRows;
    for (int i = 0; i < totalRows; ++i) {
        std::vector<Seat> rowSeats;
        SeatType seatType;
        if (i < firstClassRows) {
            seatType = SeatType::FIRST_CLASS;
        } else if (i < firstClassRows + businessClassRows) {
            seatType = SeatType::BUSINESS_CLASS;
        } else if (i < firstClassRows + businessClassRows + premiumEconomyRows) {
            seatType = SeatType::PREMIUM_ECONOMY;
        } else {
            seatType = SeatType::ECONOMY_CLASS;
        }
        for (int j = 0; j < seatsPerRow; ++j) {
            rowSeats.emplace_back(i + 1, 'A' + j, seatType);
        }
        seats.push_back(rowSeats);
    }
}

// 拷贝构造函数 - 创建独立的座位副本
Aircraft::Aircraft(const Aircraft& other)
    : type(other.type), 
      firstClassRows(other.firstClassRows), 
      businessClassRows(other.businessClassRows), 
      premiumEconomyRows(other.premiumEconomyRows), 
      economyClassRows(other.economyClassRows), 
      seatsPerRow(other.seatsPerRow),
      emptyWeightKg(other.emptyWeightKg),
      fuelCoefficientKgPerKm(other.fuelCoefficientKgPerKm),
      cruiseSpeedKmh(other.cruiseSpeedKmh),
      maxRangeKm(other.maxRangeKm) {
    // 深拷贝座位 - 重新创建所有座位,但不复制预订状态
    int totalRows = firstClassRows + businessClassRows + premiumEconomyRows + economyClassRows;
    for (int i = 0; i < totalRows; ++i) {
        std::vector<Seat> rowSeats;
        SeatType seatType;
        if (i < firstClassRows) {
            seatType = SeatType::FIRST_CLASS;
        } else if (i < firstClassRows + businessClassRows) {
            seatType = SeatType::BUSINESS_CLASS;
        } else if (i < firstClassRows + businessClassRows + premiumEconomyRows) {
            seatType = SeatType::PREMIUM_ECONOMY;
        } else {
            seatType = SeatType::ECONOMY_CLASS;
        }
        for (int j = 0; j < seatsPerRow; ++j) {
            rowSeats.emplace_back(i + 1, 'A' + j, seatType);
        }
        seats.push_back(rowSeats);
    }
}

// 获取飞机型号
std::string Aircraft::getType() const {
    return type;
}

// 显示座位图
void Aircraft::displaySeatMap() const {
    std::cout << "Seat Map for " << type << std::endl;
    std::cout << "Legend: 4=First Class, 3=Business, 2=Premium Economy, 1=Economy, 0=Occupied" << std::endl;
    
    // 打印列号
    std::cout << "   ";
    for (int j = 0; j < seatsPerRow; ++j) {
        std::cout << (char)('A' + j) << " ";
    }
    std::cout << std::endl;

    for (int i = 0; i < seats.size(); ++i) {
        // 打印行号
        if (i + 1 < 10) {
            std::cout << " " << i + 1 << " ";
        } else {
            std::cout << i + 1 << " ";
        }
        
        for (const auto& seat : seats[i]) {
            if (seat.isBooked()) {
                std::cout << "0 ";  // 已被占用
            } else {
                // 根据座位类型显示不同数字
                switch (seat.getType()) {
                    case SeatType::FIRST_CLASS:
                        std::cout << "4 ";
                        break;
                    case SeatType::BUSINESS_CLASS:
                        std::cout << "3 ";
                        break;
                    case SeatType::PREMIUM_ECONOMY:
                        std::cout << "2 ";
                        break;
                    case SeatType::ECONOMY_CLASS:
                        std::cout << "1 ";
                        break;
                }
            }
        }
        std::cout << std::endl;
    }
}

// 预订座位
bool Aircraft::bookSeat(int row, char col) {
    if (row > 0 && row <= seats.size() && col >= 'A' && col < 'A' + seatsPerRow) {
        Seat* seat = getSeat(row, col);
        if (seat && !seat->isBooked()) {
            seat->book();
            return true;
        }
    }
    return false;
}

// 取消预订座位
bool Aircraft::unbookSeat(int row, char col) {
    if (row > 0 && row <= seats.size() && col >= 'A' && col < 'A' + seatsPerRow) {
        Seat* seat = getSeat(row, col);
        if (seat && seat->isBooked()) {
            seat->unbook();
            return true;
        }
    }
    return false;
}

// 获取座位
Seat* Aircraft::getSeat(int row, char col) {
    if (row > 0 && row <= seats.size() && col >= 'A' && col < 'A' + seatsPerRow) {
        return &seats[row - 1][col - 'A'];
    }
    return nullptr;
}

// 获取总座位数
int Aircraft::getTotalSeats() const {
    return (firstClassRows + businessClassRows + premiumEconomyRows + economyClassRows) * seatsPerRow;
}

// 按类型获取座位数
int Aircraft::getSeatCountByType(SeatType type) const {
    switch (type) {
        case SeatType::FIRST_CLASS:
            return firstClassRows * seatsPerRow;
        case SeatType::BUSINESS_CLASS:
            return businessClassRows * seatsPerRow;
        case SeatType::PREMIUM_ECONOMY:
            return premiumEconomyRows * seatsPerRow;
        case SeatType::ECONOMY_CLASS:
            return economyClassRows * seatsPerRow;
        default:
            return 0;
    }
}

// 检查飞机是否有特定舱位
bool Aircraft::hasCabinClass(SeatType seatType) const {
    switch (seatType) {
        case SeatType::FIRST_CLASS:
            return firstClassRows > 0;
        case SeatType::BUSINESS_CLASS:
            return businessClassRows > 0;
        case SeatType::PREMIUM_ECONOMY:
            return premiumEconomyRows > 0;
        case SeatType::ECONOMY_CLASS:
            return economyClassRows > 0;
        default:
            return false;
    }
}
