#ifndef AIRLINE_DATA_H
#define AIRLINE_DATA_H

#include <vector>
#include "Aircraft.h"
#include "Flight.h"
#include "Crew.h"
#include "Airport.h"

// 航空数据类 - 用于集中管理所有航空信息
class AirlineData {
public:
    // 初始化所有飞机型号
    static void initializeAircrafts(std::vector<Aircraft*>& aircrafts);
    
    // 初始化所有航班
    static void initializeFlights(std::vector<Flight*>& flights, const std::vector<Aircraft*>& aircrafts, 
                                  const std::vector<Crew*>& crewMembers, const std::vector<Airport*>& airports);
    
    // 初始化所有机组成员
    static void initializeCrewMembers(std::vector<Crew*>& crewMembers);
};

#endif // AIRLINE_DATA_H
