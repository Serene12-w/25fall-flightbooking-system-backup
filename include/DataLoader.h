#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "Aircraft.h"
#include "Flight.h"
#include "Crew.h"
#include "Passenger.h"
#include "Reservation.h"
#include "Airport.h"

// CSV数据加载器类
class DataLoader {
public:
    // 从CSV文件加载机场数据
    static bool loadAirports(const std::string& filename, std::vector<Airport*>& airports);
    
    // 从CSV文件加载飞机数据
    static bool loadAircrafts(const std::string& filename, std::vector<Aircraft*>& aircrafts);
    
    // 从CSV文件加载航班数据
    static bool loadFlights(const std::string& filename, std::vector<Flight*>& flights, 
                           const std::vector<Aircraft*>& aircrafts, const std::vector<Crew*>& crewMembers,
                           const std::vector<Airport*>& airports);
    
    // 从CSV文件加载机组成员数据
    static bool loadCrewMembers(const std::string& filename, std::vector<Crew*>& crewMembers);

    // 从CSV文件加载乘客数据
    static bool loadPassengers(const std::string& filename, std::vector<Passenger*>& passengers);
    
    // 从CSV文件加载预订数据
    static bool loadReservations(const std::string& filename, std::unordered_map<std::string, Reservation*>& reservations,
                                 const std::vector<Passenger*>& passengers, const std::vector<Flight*>& flights);

    // 保存飞机数据到CSV文件
    static bool saveAircrafts(const std::string& filename, const std::vector<Aircraft*>& aircrafts);
    
    // 保存航班数据到CSV文件
    static bool saveFlights(const std::string& filename, const std::vector<Flight*>& flights);
    
    // 保存机组成员数据到CSV文件
    static bool saveCrewMembers(const std::string& filename, const std::vector<Crew*>& crewMembers);

    // 保存乘客数据到CSV文件
    static bool savePassengers(const std::string& filename, const std::vector<Passenger*>& passengers);
    
    // 保存预订数据到CSV文件
    static bool saveReservations(const std::string& filename, const std::unordered_map<std::string, Reservation*>& reservations);

private:
    // 辅助函数:去除字符串首尾空格和引号
    static std::string trim(const std::string& str);
    
    // 辅助函数:解析CSV行
    static std::vector<std::string> parseCSVLine(const std::string& line);
    
    // 辅助函数:查找飞机索引
    static int findAircraftIndex(const std::string& aircraftType, const std::vector<Aircraft*>& aircrafts);
    
    // 辅助函数:转义CSV字段(处理包含逗号的字段)
    static std::string escapeCSVField(const std::string& field);
};

#endif // DATA_LOADER_H
