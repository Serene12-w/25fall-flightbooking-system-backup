#include "DataLoader.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <unordered_map>

// 去除字符串首尾空格和引号
std::string DataLoader::trim(const std::string& str) {
    if (str.empty()) return "";
    
    size_t start = str.find_first_not_of(" \t\r\n\"");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n\"");
    return str.substr(start, end - start + 1);
}

// 解析CSV行,支持带引号的字段
std::vector<std::string> DataLoader::parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (c == '"') {
            inQuotes = !inQuotes;
        }
        else if (c == ',' && !inQuotes) {
            fields.push_back(trim(field));
            field.clear();
        }
        else {
            field += c;
        }
    }
    
    // 添加最后一个字段
    fields.push_back(trim(field));
    
    return fields;
}

// 查找飞机类型对应的索引
int DataLoader::findAircraftIndex(const std::string& aircraftType, const std::vector<Aircraft*>& aircrafts) {
    for (size_t i = 0; i < aircrafts.size(); ++i) {
        if (aircrafts[i]->getType() == aircraftType) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// 从CSV文件加载机场数据
bool DataLoader::loadAirports(const std::string& filename, std::vector<Airport*>& airports) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    // 跳过标题行
    std::getline(file, line);
    
    int count = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        // CSV格式: iataCode,icaoCode,name,city,country,latitudeDeg,latitudeMin,latitudeDir,
        //          longitudeDeg,longitudeMin,longitudeDir,elevationFt,primaryRunway,secondaryRunway,
        //          towerFreq,groundFreq,atisFreq,utcOffset
        if (fields.size() >= 18) {
            std::string iataCode = trim(fields[0]);
            std::string icaoCode = trim(fields[1]);
            std::string name = trim(fields[2]);
            std::string city = trim(fields[3]);
            std::string country = trim(fields[4]);
            double latDeg = std::stod(fields[5]);
            double latMin = std::stod(fields[6]);
            char latDir = fields[7][0];
            double lonDeg = std::stod(fields[8]);
            double lonMin = std::stod(fields[9]);
            char lonDir = fields[10][0];
            int elevationFt = std::stoi(fields[11]);
            std::string primaryRunway = trim(fields[12]);
            std::string secondaryRunway = trim(fields[13]);
            std::string towerFreq = trim(fields[14]);
            std::string groundFreq = trim(fields[15]);
            std::string atisFreq = trim(fields[16]);
            double utcOffset = std::stod(fields[17]);
            
            airports.push_back(new Airport(iataCode, icaoCode, name, city, country,
                                          latDeg, latMin, latDir, lonDeg, lonMin, lonDir,
                                          elevationFt, primaryRunway, secondaryRunway,
                                          towerFreq, groundFreq, atisFreq, utcOffset));
            count++;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << count << " airports" << std::endl;
    return true;
}

// 从CSV文件加载飞机数据
bool DataLoader::loadAircrafts(const std::string& filename, std::vector<Aircraft*>& aircrafts) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    // 跳过标题行
    std::getline(file, line);
    
    int count = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        // CSV格式: type,firstClassRows,businessClassRows,premiumEconomyRows,economyClassRows,seatsPerRow,maxRangeKm
        if (fields.size() >= 6) {
            std::string type = fields[0];
            int firstClassRows = std::stoi(fields[1]);
            int businessClassRows = std::stoi(fields[2]);
            int premiumEconomyRows = std::stoi(fields[3]);
            int economyClassRows = std::stoi(fields[4]);
            int seatsPerRow = std::stoi(fields[5]);
            
            Aircraft* aircraft = new Aircraft(type, firstClassRows, businessClassRows, 
                                            premiumEconomyRows, economyClassRows, seatsPerRow);
            
            // 如果有maxRangeKm字段,读取并设置
            if (fields.size() >= 7) {
                double maxRangeKm = std::stod(fields[6]);
                aircraft->setMaxRange(maxRangeKm);
            }
            
            aircrafts.push_back(aircraft);
            count++;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << count << " aircraft models" << std::endl;
    return true;
}

// 从CSV文件加载航班数据
bool DataLoader::loadFlights(const std::string& filename, std::vector<Flight*>& flights, 
                             const std::vector<Aircraft*>& aircrafts, const std::vector<Crew*>& crewMembers,
                             const std::vector<Airport*>& airports) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    // 创建机组成员ID到指针的映射
    std::map<std::string, Crew*> crewMap;
    for (auto* crew : crewMembers) {
        crewMap[crew->getId()] = crew;
    }
    
    // 创建机场IATA代码到指针的映射
    std::map<std::string, Airport*> airportMap;
    for (auto* airport : airports) {
        airportMap[airport->getIATACode()] = airport;
    }
    
    std::string line;
    // 跳过标题行
    std::getline(file, line);
    
    int count = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        // 新的CSV格式: flightNumber,origin,destination,departureTime,arrivalTime,flightDate,aircraftType,crewIds
        if (fields.size() >= 7) {
            std::string flightNumber = fields[0];
            std::string origin = fields[1];
            std::string destination = fields[2];
            std::string departureTime = fields[3];
            std::string arrivalTime = fields[4];
            std::string flightDate = fields[5];
            std::string aircraftType = fields[6];
            
            // 查找对应的飞机
            int aircraftIndex = findAircraftIndex(aircraftType, aircrafts);
            if (aircraftIndex != -1) {
                Flight* flight = new Flight(flightNumber, origin, destination, 
                                           departureTime, arrivalTime, aircrafts[aircraftIndex], flightDate);
                
                // 关联机场
                auto originIt = airportMap.find(origin);
                auto destIt = airportMap.find(destination);
                if (originIt != airportMap.end()) {
                    flight->setOriginAirport(originIt->second);
                }
                if (destIt != airportMap.end()) {
                    flight->setDestinationAirport(destIt->second);
                }
                
                // 加载机组成员(如果有)
                if (fields.size() >= 8 && !fields[7].empty()) {
                    std::vector<Crew*> assignedCrew;
                    std::stringstream ss(fields[7]);
                    std::string crewId;
                    while (std::getline(ss, crewId, ',')) {
                        crewId = trim(crewId);
                        auto it = crewMap.find(crewId);
                        if (it != crewMap.end()) {
                            assignedCrew.push_back(it->second);
                        } else {
                            std::cerr << "Warning: Crew member " << crewId << " not found for flight " << flightNumber << std::endl;
                        }
                    }
                    if (!assignedCrew.empty()) {
                        flight->assignCrew(assignedCrew);
                    }
                }
                
                flights.push_back(flight);
                count++;
            } else {
                std::cerr << "Warning: Flight " << flightNumber << " uses aircraft type " 
                         << aircraftType << " which was not found" << std::endl;
            }
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << count << " flights" << std::endl;
    return true;
}

// 从CSV文件加载机组成员数据
bool DataLoader::loadCrewMembers(const std::string& filename, std::vector<Crew*>& crewMembers) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    // 跳过标题行
    std::getline(file, line);
    
    int count = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        // 支持两种格式:
        // 旧格式: id,name,role,qualifications
        // 新格式: id,name,role,lastAssigned,qualifications
        if (fields.size() >= 4) {
            std::string id = fields[0];
            std::string name = fields[1];
            std::string roleStr = fields[2];
            long long lastAssignedVal = 0;
            std::string qualificationsStr;
            if (fields.size() >= 5) {
                // 新格式
                try { lastAssignedVal = std::stoll(fields[3]); } catch(...) { lastAssignedVal = 0; }
                qualificationsStr = fields[4];
            } else {
                // 旧格式
                qualificationsStr = fields[3];
            }
            
            // 解析角色
            CrewRole role = (roleStr == "PILOT") ? CrewRole::PILOT : CrewRole::CABIN_CREW;
            
            // 创建机组成员
            Crew* crew = new Crew(id, name, role);
            if (lastAssignedVal > 0) {
                crew->setLastAssigned(static_cast<std::time_t>(lastAssignedVal));
            }
            
            // 解析并添加资质(用逗号分隔)
            std::stringstream ss(qualificationsStr);
            std::string qualification;
            while (std::getline(ss, qualification, ',')) {
                qualification = trim(qualification);
                if (!qualification.empty()) {
                    crew->addQualification(qualification);
                }
            }
            
            crewMembers.push_back(crew);
            count++;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << count << " crew members" << std::endl;
    return true;
}

// 辅助函数:转义CSV字段(处理包含逗号的字段)
std::string DataLoader::escapeCSVField(const std::string& field) {
    // 如果字段包含逗号或引号,需要用引号包围
    if (field.find(',') != std::string::npos || field.find('"') != std::string::npos) {
        std::string escaped = "\"";
        for (char c : field) {
            if (c == '"') {
                escaped += "\"\""; // 双引号转义
            } else {
                escaped += c;
            }
        }
        escaped += "\"";
        return escaped;
    }
    return field;
}

// 保存飞机数据到CSV文件
bool DataLoader::saveAircrafts(const std::string& filename, const std::vector<Aircraft*>& aircrafts) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }
    
    // 写入标题行
    file << "type,firstClassRows,businessClassRows,premiumEconomyRows,economyClassRows,seatsPerRow\n";
    
    // 写入每个飞机型号
    for (const auto& aircraft : aircrafts) {
        file << aircraft->getType() << ","
             << aircraft->getFirstClassRows() << ","
             << aircraft->getBusinessClassRows() << ","
             << aircraft->getPremiumEconomyRows() << ","
             << aircraft->getEconomyClassRows() << ","
             << aircraft->getSeatsPerRow() << "\n";
    }
    
    file.close();
    std::cout << "Successfully saved " << aircrafts.size() << " aircraft models to " << filename << std::endl;
    return true;
}

// 保存航班数据到CSV文件
bool DataLoader::saveFlights(const std::string& filename, const std::vector<Flight*>& flights) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }
    
    // 写入标题行 - 只保存基本字段
    file << "flightNumber,origin,destination,departureTime,arrivalTime,flightDate,aircraftType,crewIds\n";
    
    // 写入每个航班
    for (const auto& flight : flights) {
        file << flight->getFlightNumber() << ","
             << flight->getOrigin() << ","
             << flight->getDestination() << ","
             << flight->getDepartureTime() << ","
             << flight->getArrivalTime() << ","
             << flight->getFlightDate() << ","
             << flight->getAircraft()->getType() << ",";
        
        // 机组成员IDs(逗号分隔)
        file << "\"";
        const auto& crew = flight->getAssignedCrew();
        for (size_t i = 0; i < crew.size(); ++i) {
            file << crew[i]->getId();
            if (i < crew.size() - 1) file << ",";
        }
        file << "\"\n";
    }
    
    file.close();
    std::cout << "Successfully saved " << flights.size() << " flights to " << filename << std::endl;
    return true;
}

// 保存机组成员数据到CSV文件
bool DataLoader::saveCrewMembers(const std::string& filename, const std::vector<Crew*>& crewMembers) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }
    
    // 写入标题行（新格式，包含 lastAssigned）
    file << "id,name,role,lastAssigned,qualifications\n";
    
    // 写入每个机组成员
    for (const auto& crew : crewMembers) {
       file << crew->getId() << ","
           << crew->getName() << ","
           << (crew->getRole() == CrewRole::PILOT ? "PILOT" : "CABIN_CREW") << ","
           << static_cast<long long>(crew->getLastAssigned()) << ",";
        
        // 写入资质列表
        const auto& qualifications = crew->getQualifications();
        if (qualifications.empty()) {
            file << "\n";
        } else {
            // 如果有多个资质,用引号包围并用逗号分隔
            file << "\"";
            for (size_t i = 0; i < qualifications.size(); ++i) {
                file << qualifications[i];
                if (i < qualifications.size() - 1) {
                    file << ",";
                }
            }
            file << "\"\n";
        }
    }
    
    file.close();
    std::cout << "Successfully saved " << crewMembers.size() << " crew members to " << filename << std::endl;
    return true;
}

// 从CSV文件加载乘客数据
bool DataLoader::loadPassengers(const std::string& filename, std::vector<Passenger*>& passengers) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    // 跳过标题行
    std::getline(file, line);
    
    int count = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        // CSV格式: passportNumber,name,loyaltyStatus
        if (fields.size() >= 3) {
            std::string passportNumber = fields[0];
            std::string name = fields[1];
            std::string loyaltyStr = fields[2];
            
            LoyaltyStatus loyaltyStatus = LoyaltyStatus::NONE;
            if (loyaltyStr == "SILVER") loyaltyStatus = LoyaltyStatus::SILVER;
            else if (loyaltyStr == "GOLD") loyaltyStatus = LoyaltyStatus::GOLD;
            else if (loyaltyStr == "PLATINUM") loyaltyStatus = LoyaltyStatus::PLATINUM;
            
            passengers.push_back(new Passenger(name, passportNumber, loyaltyStatus));
            count++;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << count << " passengers" << std::endl;
    return true;
}

// 从CSV文件加载预订数据 (新格式: 每个航班一行)
bool DataLoader::loadReservations(const std::string& filename, std::unordered_map<std::string, Reservation*>& reservations,
                                 const std::vector<Passenger*>& passengers, const std::vector<Flight*>& flights) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    // 创建查找映射
    std::map<std::string, Passenger*> passengerMap;
    for (auto* p : passengers) {
        passengerMap[p->getPassportNumber()] = p;
    }
    
    // 创建航班映射: "航班号:日期" -> Flight*
    std::map<std::string, Flight*> flightMap;
    for (auto* f : flights) {
        std::string flightKey = f->getFlightNumber() + ":" + f->getFlightDate();
        flightMap[flightKey] = f;
    }
    
    std::string line;
    // 跳过标题行
    std::getline(file, line);
    
    int count = 0;
    int resNumber = 1; // 自动生成预订号
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<std::string> fields = parseCSVLine(line);
        
        // 新CSV格式: flightNumber,flightDate,seatReservations
        // seatReservations格式: "1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED,2A:P87654321:CONFIRMED"
        if (fields.size() >= 3) {
            std::string flightNumber = fields[0];
            std::string flightDate = fields[1];
            std::string seatReservations = fields[2];
            
            // 查找航班(使用航班号+日期)
            std::string flightKey = flightNumber + ":" + flightDate;
            auto flightIt = flightMap.find(flightKey);
            if (flightIt == flightMap.end()) {
                std::cerr << "Warning: Flight " << flightNumber << " on " << flightDate << " not found" << std::endl;
                continue;
            }
            
            Flight* flight = flightIt->second;
            
            // 解析座位预订信息: "1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED,..."
            std::stringstream ss(seatReservations);
            std::string seatInfo;
            
            while (std::getline(ss, seatInfo, ',')) {
                seatInfo = trim(seatInfo);
                if (seatInfo.empty()) continue;
                
                // 解析单个座位信息: "1A:G12345678:CONFIRMED"
                std::vector<std::string> parts;
                std::stringstream seatSS(seatInfo);
                std::string part;
                while (std::getline(seatSS, part, ':')) {
                    parts.push_back(trim(part));
                }
                
                if (parts.size() >= 3) {
                    std::string seatPos = parts[0];      // "1A"
                    std::string passportNumber = parts[1]; // "G12345678"
                    std::string statusStr = parts[2];     // "CONFIRMED"
                    
                    // 解析座位位置 (1A -> row=1, col='A')
                    int seatRow = 0;
                    char seatCol = 0;
                    
                    // 提取行号和列号
                    size_t i = 0;
                    while (i < seatPos.length() && std::isdigit(seatPos[i])) {
                        seatRow = seatRow * 10 + (seatPos[i] - '0');
                        i++;
                    }
                    if (i < seatPos.length()) {
                        seatCol = seatPos[i];
                    }
                    
                    // 查找乘客
                    auto passengerIt = passengerMap.find(passportNumber);
                    if (passengerIt == passengerMap.end()) {
                        std::cerr << "Warning: Passenger " << passportNumber << " not found" << std::endl;
                        continue;
                    }
                    
                    Passenger* passenger = passengerIt->second;
                    Seat* seat = flight->getAircraft()->getSeat(seatRow, seatCol);
                    
                    if (seat) {
                        // 生成预订号
                        std::string reservationNumber = "RES" + std::string(3 - std::to_string(resNumber).length(), '0') + std::to_string(resNumber);
                        resNumber++;
                        
                        Reservation* reservation = new Reservation(reservationNumber, passenger, flight, seat);
                        
                        // 设置状态
                        if (statusStr == "CONFIRMED") reservation->confirm();
                        else if (statusStr == "CANCELLED") reservation->cancel();
                        else if (statusStr == "WAITLISTED") reservation->waitlist();
                        
                        // 预订座位
                        if (statusStr == "CONFIRMED" || statusStr == "PENDING") {
                            flight->getAircraft()->bookSeat(seatRow, seatCol);
                        }
                        
                        reservations[reservationNumber] = reservation;
                        count++;
                    }
                }
            }
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << count << " reservations from " << flightMap.size() << " flights" << std::endl;
    return true;
}

// 保存乘客数据到CSV文件
bool DataLoader::savePassengers(const std::string& filename, const std::vector<Passenger*>& passengers) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }
    
    // 写入标题行
    file << "passportNumber,name,loyaltyStatus\n";
    
    // 写入每个乘客
    for (const auto& passenger : passengers) {
        std::string loyaltyStr = "NONE";
        switch (passenger->getLoyaltyStatus()) {
            case LoyaltyStatus::SILVER: loyaltyStr = "SILVER"; break;
            case LoyaltyStatus::GOLD: loyaltyStr = "GOLD"; break;
            case LoyaltyStatus::PLATINUM: loyaltyStr = "PLATINUM"; break;
            default: break;
        }
        
        file << passenger->getPassportNumber() << ","
             << passenger->getName() << ","
             << loyaltyStr << "\n";
    }
    
    file.close();
    std::cout << "Successfully saved " << passengers.size() << " passengers to " << filename << std::endl;
    return true;
}

// 保存预订数据到CSV文件 (新格式: 每个航班一行,包含日期)
bool DataLoader::saveReservations(const std::string& filename, const std::unordered_map<std::string, Reservation*>& reservations) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }
    
    // 按航班+日期分组预订
    std::map<std::string, std::vector<Reservation*>> flightReservations;
    for (const auto& pair : reservations) {
        Reservation* reservation = pair.second;
        // 使用"航班号:日期"作为key
        std::string flightKey = reservation->getFlight()->getFlightNumber() + ":" + reservation->getFlight()->getFlightDate();
        flightReservations[flightKey].push_back(reservation);
    }
    
    // 写入标题行
    file << "flightNumber,flightDate,seatReservations\n";
    
    // 写入每个航班的预订信息
    for (const auto& flightPair : flightReservations) {
        const std::string& flightKey = flightPair.first;
        const auto& reservationList = flightPair.second;
        
        // 分离航班号和日期
        size_t colonPos = flightKey.find(':');
        std::string flightNumber = flightKey.substr(0, colonPos);
        std::string flightDate = flightKey.substr(colonPos + 1);
        
        file << flightNumber << "," << flightDate << ",\"";
        
        // 写入该航班的所有座位预订: "1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED,..."
        bool first = true;
        for (const auto& reservation : reservationList) {
            if (!first) file << ",";
            first = false;
            
            std::string statusStr = "PENDING";
            switch (reservation->getStatus()) {
                case ReservationStatus::CONFIRMED: statusStr = "CONFIRMED"; break;
                case ReservationStatus::CANCELLED: statusStr = "CANCELLED"; break;
                case ReservationStatus::WAITLISTED: statusStr = "WAITLISTED"; break;
                default: break;
            }
            
            // 格式: "行列:护照号:状态" 例如 "1A:G12345678:CONFIRMED"
            file << reservation->getSeat()->getRow()
                 << reservation->getSeat()->getCol()
                 << ":"
                 << reservation->getPassenger()->getPassportNumber()
                 << ":"
                 << statusStr;
        }
        
        file << "\"\n";
    }
    
    file.close();
    std::cout << "Successfully saved " << reservations.size() << " reservations (" 
              << flightReservations.size() << " flight instances) to " << filename << std::endl;
    return true;
}

