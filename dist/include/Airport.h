#ifndef AIRPORT_H
#define AIRPORT_H

#include <string>

// 机场类
class Airport {
public:
    // 构造函数
    Airport(const std::string& iataCode, const std::string& icaoCode, const std::string& name,
            const std::string& city, const std::string& country,
            double latitudeDeg, double latitudeMin, char latitudeDir,
            double longitudeDeg, double longitudeMin, char longitudeDir,
            int elevationFt, const std::string& primaryRunway, const std::string& secondaryRunway,
            const std::string& towerFreq, const std::string& groundFreq, const std::string& atisFreq,
            double utcOffset);

    // 获取IATA代码 (三字代码)
    std::string getIATACode() const;

    // 获取ICAO代码 (四字代码)
    std::string getICAOCode() const;

    // 获取机场名称
    std::string getName() const;

    // 获取城市
    std::string getCity() const;

    // 获取国家
    std::string getCountry() const;

    // 获取纬度(十进制度数)
    double getLatitude() const;

    // 获取经度(十进制度数)
    double getLongitude() const;

    // 获取海拔(英尺)
    int getElevation() const;

    // 获取主跑道信息
    std::string getPrimaryRunway() const;

    // 获取备用跑道信息
    std::string getSecondaryRunway() const;

    // 获取塔台频率
    std::string getTowerFreq() const;

    // 获取地面频率
    std::string getGroundFreq() const;

    // 获取ATIS频率
    std::string getATISFreq() const;

    // 获取UTC偏移
    double getUTCOffset() const;

    // 显示机场信息
    void displayInfo() const;

private:
    std::string iataCode;       // IATA三字代码
    std::string icaoCode;       // ICAO四字代码
    std::string name;           // 机场名称
    std::string city;           // 城市
    std::string country;        // 国家
    double latitude;            // 纬度(十进制度数)
    double longitude;           // 经度(十进制度数)
    int elevationFt;            // 海拔(英尺)
    std::string primaryRunway;  // 主跑道
    std::string secondaryRunway; // 备用跑道
    std::string towerFreq;      // 塔台频率
    std::string groundFreq;     // 地面频率
    std::string atisFreq;       // ATIS频率
    double utcOffset;           // UTC时区偏移

    // 将度分格式转换为十进制度数
    static double convertToDecimalDegrees(double degrees, double minutes, char direction);
};

#endif // AIRPORT_H
