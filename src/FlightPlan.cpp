#include "FlightPlan.h"
#include "FlightCalculator.h"
#include <iostream>
#include <iomanip>

// 构造函数
FlightPlan::FlightPlan(Flight* flight, const std::vector<Airport*>& airports) 
    : flight(flight), airports(airports) {
}

// 查找最近的备降机场
Airport* FlightPlan::findNearestAlternateAirport(Airport* destination) const {
    if (!destination || airports.size() < 2) {
        return nullptr;
    }
    
    Airport* nearestAirport = nullptr;
    double minDistance = 999999.0;
    
    for (Airport* airport : airports) {
        // 跳过目的地机场本身
        if (airport->getIATACode() == destination->getIATACode()) {
            continue;
        }
        
        // 计算距离
        double distance = FlightCalculator::calculateDistance(destination, airport);
        
        if (distance < minDistance) {
            minDistance = distance;
            nearestAirport = airport;
        }
    }
    
    return nearestAirport;
}

// 显示飞行计划
void FlightPlan::display() const {
    if (!flight) return;

    Airport* originAirport = flight->getOriginAirport();
    Airport* destAirport = flight->getDestinationAirport();
    Aircraft* aircraft = flight->getAircraft();
    
    std::cout << "\n========================================================" << std::endl;
    std::cout << "                 OPERATIONAL FLIGHT PLAN" << std::endl;
    std::cout << "========================================================" << std::endl;
    std::cout << std::left;

    // 航班基本信息
    std::cout << std::setw(20) << "FLIGHT" << std::setw(20) << "DATE" << std::setw(20) << "AIRCRAFT" << std::endl;
    std::cout << std::setw(20) << flight->getFlightNumber() 
              << std::setw(20) << flight->getFlightDate() 
              << std::setw(20) << aircraft->getType() << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    // 航线信息
    std::cout << std::setw(20) << "DEPARTURE" << std::setw(20) << "DESTINATION" << std::setw(20) << "SCHEDULED TIME" << std::endl;
    if (originAirport && destAirport) {
        std::cout << std::setw(20) << (flight->getOrigin() + " - " + originAirport->getCity()) 
                  << std::setw(20) << (flight->getDestination() + " - " + destAirport->getCity())
                  << std::setw(20) << (flight->getDepartureTime() + " - " + flight->getArrivalTime()) << std::endl;
    } else {
        std::cout << std::setw(20) << flight->getOrigin() 
                  << std::setw(20) << flight->getDestination()
                  << std::setw(20) << (flight->getDepartureTime() + " - " + flight->getArrivalTime()) << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;

    // 实时计算距离和飞行时间
    double distance = 0.0;
    double flightTime = 0.0;
    double cruiseSpeed = aircraft->getCruiseSpeed();
    if (cruiseSpeed == 0) cruiseSpeed = 850.0; // 默认巡航速度
    
    if (originAirport && destAirport) {
        distance = FlightCalculator::calculateDistance(originAirport, destAirport);
        flightTime = FlightCalculator::calculateFlightTime(distance, cruiseSpeed);
        
        std::cout << "DISTANCE: " << std::fixed << std::setprecision(1) << distance << " km" << std::endl;
        std::cout << "ESTIMATED FLIGHT TIME: " << std::fixed << std::setprecision(1) << flightTime 
                  << " hours (" << static_cast<int>(flightTime * 60) << " minutes)" << std::endl;
        std::cout << "--------------------------------------------------------" << std::endl;
    }

    // 飞行参数 (实时计算)
    std::string route = flight->getOrigin() + " DCT " + flight->getDestination(); // Direct route
    std::string cruisingAltitude = "FL390"; // 标准巡航高度
    std::string cruisingSpeedStr = std::to_string((int)cruiseSpeed) + " km/h";
    
    std::cout << "CRUISING ALTITUDE: " << cruisingAltitude << std::endl;
    std::cout << "CRUISING SPEED: " << cruisingSpeedStr << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    // 实时计算燃油需求
    int passengerCount = 0; // 默认乘客数
    double actualFuelNeeded = 0.0;
    if (distance > 0) {
        actualFuelNeeded = FlightCalculator::calculateFuelRequired(distance, aircraft->getFuelCoefficient(), passengerCount);
    }
    
    // 燃油计划
    std::cout << "FUEL PLAN:" << std::endl;
    if (actualFuelNeeded > 0) {
        std::cout << "  - TRIP FUEL: " << std::fixed << std::setprecision(1) << actualFuelNeeded << " KG" << std::endl;
        std::cout << "  - CONTINGENCY (5%): " << actualFuelNeeded * 0.05 << " KG" << std::endl;
        std::cout << "  - ALTERNATE FUEL: " << 1200.0 << " KG" << std::endl;
        std::cout << "  - FINAL RESERVE: " << 800.0 << " KG" << std::endl;
        std::cout << "  - TOTAL FUEL ON BOARD: " << actualFuelNeeded + 2000.0 << " KG" << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;

    // 重量信息
    std::cout << "WEIGHT & BALANCE:" << std::endl;
    std::cout << "  - AIRCRAFT EMPTY WEIGHT: " << std::fixed << std::setprecision(0) << aircraft->getEmptyWeight() << " KG" << std::endl;
    double fuelWeight = actualFuelNeeded;
    double takeoffWeight = FlightCalculator::calculateTakeoffWeight(aircraft->getEmptyWeight(), fuelWeight, passengerCount, 0);
    std::cout << "  - FUEL WEIGHT: " << fuelWeight << " KG" << std::endl;
    std::cout << "  - PASSENGER/CARGO WEIGHT: " << passengerCount * 100 << " KG" << std::endl;
    std::cout << "  - ESTIMATED TAKEOFF WEIGHT: " << takeoffWeight << " KG" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    // 实时查找备降机场
    std::cout << "ALTERNATE AIRPORTS:" << std::endl;
    if (destAirport) {
        Airport* alternateAirport = findNearestAlternateAirport(destAirport);
        if (alternateAirport) {
            double altDistance = FlightCalculator::calculateDistance(destAirport, alternateAirport);
            std::cout << "  PRIMARY ALTERNATE: " << alternateAirport->getIATACode() 
                      << " - " << alternateAirport->getName() << std::endl;
            std::cout << "    City: " << alternateAirport->getCity() << ", " << alternateAirport->getCountry() << std::endl;
            std::cout << "    Distance from destination: " << std::fixed << std::setprecision(1) << altDistance << " km" << std::endl;
            std::cout << "    Elevation: " << alternateAirport->getElevation() << " ft" << std::endl;
            std::cout << "    Primary Runway: " << alternateAirport->getPrimaryRunway() << std::endl;
            std::cout << "    Tower Frequency: " << alternateAirport->getTowerFreq() << " MHz" << std::endl;
        } else {
            std::cout << "  - NONE AVAILABLE" << std::endl;
        }
    } else {
        std::cout << "  - DESTINATION AIRPORT NOT SET" << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;

    // 机场信息
    if (originAirport) {
        std::cout << "\nDEPARTURE AIRPORT INFORMATION:" << std::endl;
        std::cout << "  Airport: " << originAirport->getName() << std::endl;
        std::cout << "  ICAO: " << originAirport->getICAOCode() << std::endl;
        std::cout << "  Elevation: " << originAirport->getElevation() << " ft" << std::endl;
        std::cout << "  Primary Runway: " << originAirport->getPrimaryRunway() << std::endl;
        std::cout << "  Tower Frequency: " << originAirport->getTowerFreq() << " MHz" << std::endl;
        std::cout << "  Ground Frequency: " << originAirport->getGroundFreq() << " MHz" << std::endl;
        std::cout << "  ATIS Frequency: " << originAirport->getATISFreq() << " MHz" << std::endl;
    }
    
    if (destAirport) {
        std::cout << "\nARRIVAL AIRPORT INFORMATION:" << std::endl;
        std::cout << "  Airport: " << destAirport->getName() << std::endl;
        std::cout << "  ICAO: " << destAirport->getICAOCode() << std::endl;
        std::cout << "  Elevation: " << destAirport->getElevation() << " ft" << std::endl;
        std::cout << "  Primary Runway: " << destAirport->getPrimaryRunway() << std::endl;
        std::cout << "  Tower Frequency: " << destAirport->getTowerFreq() << " MHz" << std::endl;
        std::cout << "  Ground Frequency: " << destAirport->getGroundFreq() << " MHz" << std::endl;
        std::cout << "  ATIS Frequency: " << destAirport->getATISFreq() << " MHz" << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;

    // 机组信息
    std::cout << "\nCREW ASSIGNMENT:" << std::endl;
    flight->viewCrew();
    std::cout << "--------------------------------------------------------" << std::endl;

    std::cout << "                 END OF FLIGHT PLAN" << std::endl;
    std::cout << "========================================================" << std::endl;
}

// 显示飞行计划(带乘客数量)
void FlightPlan::displayWithPassengerCount(int actualPassengerCount) const {
    if (!flight) return;

    Airport* originAirport = flight->getOriginAirport();
    Airport* destAirport = flight->getDestinationAirport();
    Aircraft* aircraft = flight->getAircraft();
    
    std::cout << "\n========================================================" << std::endl;
    std::cout << "                 OPERATIONAL FLIGHT PLAN" << std::endl;
    std::cout << "========================================================" << std::endl;
    std::cout << std::left;

    // 航班基本信息
    std::cout << std::setw(20) << "FLIGHT" << std::setw(20) << "DATE" << std::setw(20) << "AIRCRAFT" << std::endl;
    std::cout << std::setw(20) << flight->getFlightNumber() 
              << std::setw(20) << flight->getFlightDate() 
              << std::setw(20) << aircraft->getType() << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    // 航路信息
    std::cout << "ROUTE INFORMATION:" << std::endl;
    std::cout << "  FROM: " << flight->getOrigin();
    if (originAirport) {
        std::cout << " (" << originAirport->getName() << ")";
    }
    std::cout << std::endl;
    std::cout << "  TO: " << flight->getDestination();
    if (destAirport) {
        std::cout << " (" << destAirport->getName() << ")";
    }
    std::cout << std::endl;
    
    // 实时计算距离
    double distance = 0;
    double cruiseSpeed = aircraft->getCruiseSpeed();
    if (cruiseSpeed == 0) cruiseSpeed = 850.0;
    
    if (originAirport && destAirport) {
        distance = FlightCalculator::calculateDistance(originAirport, destAirport);
        std::cout << "  DISTANCE: " << std::fixed << std::setprecision(0) << distance << " KM" << std::endl;
    }

    std::cout << "  DEPARTURE: " << flight->getDepartureTime() << std::endl;
    std::cout << "  ARRIVAL: " << flight->getArrivalTime() << std::endl;
    
    // 实时计算飞行时间
    if (distance > 0 && cruiseSpeed > 0) {
        double flightTime = FlightCalculator::calculateFlightTime(distance, cruiseSpeed);
        int hours = (int)flightTime;
        int minutes = (int)((flightTime - hours) * 60);
        std::cout << "  ESTIMATED FLIGHT TIME: " << hours << "H " << minutes << "M" << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;

    // 巡航信息 (实时生成)
    std::cout << "CRUISE INFORMATION:" << std::endl;
    std::cout << "  ALTITUDE: FL390" << std::endl;
    std::cout << "  SPEED: " << std::fixed << std::setprecision(0) << cruiseSpeed << " km/h" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    // 重量和平衡 (使用实际乘客数量,实时计算)
    std::cout << "WEIGHT & BALANCE:" << std::endl;
    std::cout << "  - AIRCRAFT EMPTY WEIGHT: " << aircraft->getEmptyWeight() << " KG" << std::endl;
    
    // 实时计算燃油重量
    double fuelWeight = 0;
    if (distance > 0 && aircraft->getFuelCoefficient() > 0) {
        fuelWeight = FlightCalculator::calculateFuelRequired(distance, aircraft->getFuelCoefficient(), actualPassengerCount);
    }
    
    // 计算乘客/货物重量 (平均每位乘客100公斤,包括行李)
    double passengerCargoWeight = actualPassengerCount * 100.0;
    
    // 实时计算起飞重量
    double takeoffWeight = FlightCalculator::calculateTakeoffWeight(aircraft->getEmptyWeight(), fuelWeight, actualPassengerCount, 0);
    
    std::cout << "  - FUEL WEIGHT: " << std::fixed << std::setprecision(0) << fuelWeight << " KG" << std::endl;
    std::cout << "  - PASSENGER/CARGO WEIGHT: " << passengerCargoWeight << " KG (" << actualPassengerCount << " passengers)" << std::endl;
    std::cout << "  - ESTIMATED TAKEOFF WEIGHT: " << takeoffWeight << " KG" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    // 实时查找备降机场
    std::cout << "ALTERNATE AIRPORTS:" << std::endl;
    if (destAirport) {
        Airport* alternateAirport = findNearestAlternateAirport(destAirport);
        if (alternateAirport) {
            double altDistance = FlightCalculator::calculateDistance(destAirport, alternateAirport);
            std::cout << "  PRIMARY ALTERNATE: " << alternateAirport->getIATACode() 
                      << " - " << alternateAirport->getName() << std::endl;
            std::cout << "    City: " << alternateAirport->getCity() << ", " << alternateAirport->getCountry() << std::endl;
            std::cout << "    Distance from destination: " << std::fixed << std::setprecision(1) << altDistance << " km" << std::endl;
            std::cout << "    Elevation: " << alternateAirport->getElevation() << " ft" << std::endl;
            std::cout << "    Primary Runway: " << alternateAirport->getPrimaryRunway() << std::endl;
            std::cout << "    Tower Frequency: " << alternateAirport->getTowerFreq() << " MHz" << std::endl;
        } else {
            std::cout << "  - NONE AVAILABLE" << std::endl;
        }
    } else {
        std::cout << "  - DESTINATION AIRPORT NOT SET" << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;

    // 机场信息
    if (originAirport) {
        std::cout << "\nDEPARTURE AIRPORT INFORMATION:" << std::endl;
        std::cout << "  Airport: " << originAirport->getName() << std::endl;
        std::cout << "  ICAO: " << originAirport->getICAOCode() << std::endl;
        std::cout << "  Elevation: " << originAirport->getElevation() << " ft" << std::endl;
        std::cout << "  Primary Runway: " << originAirport->getPrimaryRunway() << std::endl;
        std::cout << "  Tower Frequency: " << originAirport->getTowerFreq() << " MHz" << std::endl;
        std::cout << "  Ground Frequency: " << originAirport->getGroundFreq() << " MHz" << std::endl;
        std::cout << "  ATIS Frequency: " << originAirport->getATISFreq() << " MHz" << std::endl;
    }
    
    if (destAirport) {
        std::cout << "\nARRIVAL AIRPORT INFORMATION:" << std::endl;
        std::cout << "  Airport: " << destAirport->getName() << std::endl;
        std::cout << "  ICAO: " << destAirport->getICAOCode() << std::endl;
        std::cout << "  Elevation: " << destAirport->getElevation() << " ft" << std::endl;
        std::cout << "  Primary Runway: " << destAirport->getPrimaryRunway() << std::endl;
        std::cout << "  Tower Frequency: " << destAirport->getTowerFreq() << " MHz" << std::endl;
        std::cout << "  Ground Frequency: " << destAirport->getGroundFreq() << " MHz" << std::endl;
        std::cout << "  ATIS Frequency: " << destAirport->getATISFreq() << " MHz" << std::endl;
    }
    std::cout << "--------------------------------------------------------" << std::endl;

    // 机组信息
    std::cout << "\nCREW ASSIGNMENT:" << std::endl;
    flight->viewCrew();
    std::cout << "--------------------------------------------------------" << std::endl;

    std::cout << "                 END OF FLIGHT PLAN" << std::endl;
    std::cout << "========================================================" << std::endl;
}
