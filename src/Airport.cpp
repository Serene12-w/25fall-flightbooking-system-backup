#include "Airport.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// 构造函数
Airport::Airport(const std::string& iataCode, const std::string& icaoCode, const std::string& name,
                 const std::string& city, const std::string& country,
                 double latitudeDeg, double latitudeMin, char latitudeDir,
                 double longitudeDeg, double longitudeMin, char longitudeDir,
                 int elevationFt, const std::string& primaryRunway, const std::string& secondaryRunway,
                 const std::string& towerFreq, const std::string& groundFreq, const std::string& atisFreq,
                 double utcOffset)
    : iataCode(iataCode), icaoCode(icaoCode), name(name), city(city), country(country),
      elevationFt(elevationFt), primaryRunway(primaryRunway), secondaryRunway(secondaryRunway),
      towerFreq(towerFreq), groundFreq(groundFreq), atisFreq(atisFreq), utcOffset(utcOffset) {
    
    // 将度分格式转换为十进制度数
    latitude = convertToDecimalDegrees(latitudeDeg, latitudeMin, latitudeDir);
    longitude = convertToDecimalDegrees(longitudeDeg, longitudeMin, longitudeDir);
}

// 将度分格式转换为十进制度数
double Airport::convertToDecimalDegrees(double degrees, double minutes, char direction) {
    double decimal = degrees + (minutes / 60.0);
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    return decimal;
}

// 获取IATA代码
std::string Airport::getIATACode() const {
    return iataCode;
}

// 获取ICAO代码
std::string Airport::getICAOCode() const {
    return icaoCode;
}

// 获取机场名称
std::string Airport::getName() const {
    return name;
}

// 获取城市
std::string Airport::getCity() const {
    return city;
}

// 获取国家
std::string Airport::getCountry() const {
    return country;
}

// 获取纬度
double Airport::getLatitude() const {
    return latitude;
}

// 获取经度
double Airport::getLongitude() const {
    return longitude;
}

// 获取海拔
int Airport::getElevation() const {
    return elevationFt;
}

// 获取主跑道信息
std::string Airport::getPrimaryRunway() const {
    return primaryRunway;
}

// 获取备用跑道信息
std::string Airport::getSecondaryRunway() const {
    return secondaryRunway;
}

// 获取塔台频率
std::string Airport::getTowerFreq() const {
    return towerFreq;
}

// 获取地面频率
std::string Airport::getGroundFreq() const {
    return groundFreq;
}

// 获取ATIS频率
std::string Airport::getATISFreq() const {
    return atisFreq;
}

// 获取UTC偏移
double Airport::getUTCOffset() const {
    return utcOffset;
}

// 显示机场信息
void Airport::displayInfo() const {
    std::cout << "\n========== Airport Information ==========" << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "IATA/ICAO: " << iataCode << " / " << icaoCode << std::endl;
    std::cout << "Location: " << city << ", " << country << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Coordinates: " << latitude << "°, " << longitude << "°" << std::endl;
    std::cout << "Elevation: " << elevationFt << " ft" << std::endl;
    std::cout << "Primary Runway: " << primaryRunway << std::endl;
    std::cout << "Secondary Runway: " << secondaryRunway << std::endl;
    std::cout << "Tower: " << towerFreq << " MHz" << std::endl;
    std::cout << "Ground: " << groundFreq << " MHz" << std::endl;
    std::cout << "ATIS: " << atisFreq << " MHz" << std::endl;
    std::cout << "UTC Offset: " << (utcOffset >= 0 ? "+" : "") << utcOffset << std::endl;
    std::cout << "==========================================" << std::endl;
}
