#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>
#include <vector>
#include "Aircraft.h"
#include "Passenger.h"
#include "Crew.h"
#include "Airport.h"

// 航班类
class Flight {
public:
    // 构造函数
    Flight(const std::string& flightNumber, const std::string& origin, const std::string& destination, const std::string& departureTime, const std::string& arrivalTime, Aircraft* aircraft, const std::string& flightDate = "");

    // 析构函数
    ~Flight();

    // 获取航班号
    std::string getFlightNumber() const;

    // 获取出发地IATA代码
    std::string getOrigin() const;

    // 获取目的地IATA代码
    std::string getDestination() const;
    
    // 设置/获取出发机场
    void setOriginAirport(Airport* airport) { originAirport = airport; }
    Airport* getOriginAirport() const { return originAirport; }
    
    // 设置/获取目的地机场
    void setDestinationAirport(Airport* airport) { destinationAirport = airport; }
    Airport* getDestinationAirport() const { return destinationAirport; }

    // 获取出发时间
    std::string getDepartureTime() const;

    // 获取到达时间
    std::string getArrivalTime() const;

    // 获取/设置航班日期
    std::string getFlightDate() const;
    void setFlightDate(const std::string& date);

    // 获取飞机信息
    Aircraft* getAircraft() const;

    // 分配机组
    void assignCrew(const std::vector<Crew*>& crew);

    // 添加单个机组成员
    void addCrewMember(Crew* crew);

    // 获取机组成员列表
    const std::vector<Crew*>& getAssignedCrew() const;

    // 查看机组
    void viewCrew() const;

private:
    std::string flightNumber;   // 航班号
    std::string origin;         // 出发地IATA代码
    std::string destination;    // 目的地IATA代码
    std::string departureTime;  // 出发时间
    std::string arrivalTime;    // 到达时间
    std::string flightDate;     // 航班日期 (格式: YYYY-MM-DD)
    Aircraft* aircraft;         // 执飞飞机 (每个航班拥有独立的Aircraft实例)
    bool ownsAircraft;          // 标记是否拥有Aircraft对象的所有权
    Airport* originAirport;     // 出发机场
    Airport* destinationAirport; // 目的地机场
    std::vector<Crew*> assignedCrew; // 分配的机组成员
};

#endif // FLIGHT_H
