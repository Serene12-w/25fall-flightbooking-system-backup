#include "Flight.h"
#include <iostream>

// 构造函数 - 为每个航班创建独立的Aircraft副本
Flight::Flight(const std::string& flightNumber, const std::string& origin, const std::string& destination, const std::string& departureTime, const std::string& arrivalTime, Aircraft* aircraftTemplate, const std::string& flightDate)
    : flightNumber(flightNumber), origin(origin), destination(destination), departureTime(departureTime), arrivalTime(arrivalTime), flightDate(flightDate),
      originAirport(nullptr), destinationAirport(nullptr), ownsAircraft(true) {
    // 创建Aircraft的独立副本,这样每个航班有自己的座位状态
    if (aircraftTemplate) {
        aircraft = new Aircraft(*aircraftTemplate);
    } else {
        aircraft = nullptr;
    }
}

// 析构函数
Flight::~Flight() {
    // 如果拥有Aircraft对象,释放它
    if (ownsAircraft && aircraft) {
        delete aircraft;
        aircraft = nullptr;
    }
}

// 获取航班号
std::string Flight::getFlightNumber() const {
    return flightNumber;
}

// 获取出发地
std::string Flight::getOrigin() const {
    return origin;
}

// 获取目的地
std::string Flight::getDestination() const {
    return destination;
}

// 获取出发时间
std::string Flight::getDepartureTime() const {
    return departureTime;
}

// 获取到达时间
std::string Flight::getArrivalTime() const {
    return arrivalTime;
}

// 获取航班日期
std::string Flight::getFlightDate() const {
    return flightDate;
}

// 设置航班日期
void Flight::setFlightDate(const std::string& date) {
    this->flightDate = date;
}

// 获取飞机信息
Aircraft* Flight::getAircraft() const {
    return aircraft;
}

// 分配机组
void Flight::assignCrew(const std::vector<Crew*>& crew) {
    assignedCrew = crew;
}

// 添加单个机组成员
void Flight::addCrewMember(Crew* crew) {
    if (crew) {
        assignedCrew.push_back(crew);
    }
}

// 获取机组成员列表
const std::vector<Crew*>& Flight::getAssignedCrew() const {
    return assignedCrew;
}

// 查看机组
void Flight::viewCrew() const {
    std::cout << "Assigned crew for flight " << flightNumber << ":" << std::endl;
    if (assignedCrew.empty()) {
        std::cout << "  No crew assigned." << std::endl;
        return;
    }
    for (const auto& member : assignedCrew) {
        std::cout << "  - " << member->getName() << " (" << member->getId() << "), Role: "
                  << (member->getRole() == CrewRole::PILOT ? "Pilot" : "Cabin Crew") << std::endl;
    }
}
