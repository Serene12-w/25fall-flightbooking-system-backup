#include "AirlineData.h"
#include "DataLoader.h"
#include <iostream>

// ==================== 初始化飞机型号 ====================
void AirlineData::initializeAircrafts(std::vector<Aircraft*>& aircrafts) {
    // 尝试从CSV文件加载
    if (DataLoader::loadAircrafts("data/aircrafts.csv", aircrafts)) {
        return; // 成功从文件加载
    }
    
    // 如果文件加载失败,使用默认数据
    std::cout << "Failed to load from CSV file, using default aircraft data..." << std::endl;
    // 空客机型 - Airbus Family
    // A320系列 (窄体客机 - Narrow Body)
    // 参数: (机型, 头等舱行数, 商务舱行数, 超级经济舱行数, 经济舱行数, 每行座位数)
    aircrafts.push_back(new Aircraft("A319-100", 0, 8, 0, 16, 6));           // 索引0: 144座标准配置
    aircrafts.push_back(new Aircraft("A320-200", 0, 12, 0, 24, 6));          // 索引1: 216座标准配置
    aircrafts.push_back(new Aircraft("A320neo", 0, 12, 4, 22, 6));           // 索引2: 228座配置 (新发动机)
    aircrafts.push_back(new Aircraft("A321-200", 0, 16, 0, 28, 6));          // 索引3: 264座标准配置
    aircrafts.push_back(new Aircraft("A321neo", 0, 16, 4, 26, 6));           // 索引4: 276座配置
    aircrafts.push_back(new Aircraft("A321LR", 4, 20, 4, 22, 6));            // 索引5: 300座远程配置
    aircrafts.push_back(new Aircraft("A321XLR", 4, 24, 4, 22, 6));           // 索引6: 324座超远程配置
    
    // A330系列 (宽体客机 - Wide Body)
    aircrafts.push_back(new Aircraft("A330-200", 8, 28, 6, 24, 8));          // 索引7: 528座三舱配置
    aircrafts.push_back(new Aircraft("A330-300", 8, 32, 8, 28, 8));          // 索引8: 608座三舱配置
    aircrafts.push_back(new Aircraft("A330-900neo", 8, 28, 8, 24, 8));       // 索引9: 544座三舱配置
    
    // A350系列 (宽体客机 - Wide Body)
    aircrafts.push_back(new Aircraft("A350-900", 12, 36, 8, 32, 9));         // 索引10: 792座三舱配置
    aircrafts.push_back(new Aircraft("A350-1000", 16, 40, 10, 35, 9));       // 索引11: 909座三舱配置
    
    // A380系列 (超大型宽体客机 - Very Large Aircraft)
    aircrafts.push_back(new Aircraft("A380-800", 14, 76, 10, 70, 10));       // 索引12: 1700座三舱配置 (双层)

    // 波音机型 - Boeing Family
    // B737系列 (窄体客机 - Narrow Body)
    aircrafts.push_back(new Aircraft("B737-700", 0, 8, 0, 16, 6));           // 索引13: 144座双舱配置
    aircrafts.push_back(new Aircraft("B737-800", 0, 12, 0, 24, 6));          // 索引14: 216座双舱配置
    aircrafts.push_back(new Aircraft("B737-900ER", 0, 16, 0, 26, 6));        // 索引15: 252座双舱配置
    aircrafts.push_back(new Aircraft("B737 MAX 8", 0, 12, 4, 22, 6));        // 索引16: 228座配置
    aircrafts.push_back(new Aircraft("B737 MAX 9", 0, 16, 4, 24, 6));        // 索引17: 264座配置
    
    // B747系列 (宽体客机 - Wide Body "Queen of the Skies")
    aircrafts.push_back(new Aircraft("B747-400", 14, 52, 10, 50, 10));       // 索引18: 1260座三舱配置 (双层)
    aircrafts.push_back(new Aircraft("B747-8", 16, 56, 12, 52, 10));         // 索引19: 1360座三舱配置 (双层)
    
    // B767系列 (宽体客机 - Wide Body)
    aircrafts.push_back(new Aircraft("B767-300ER", 8, 28, 6, 24, 7));        // 索引20: 462座三舱配置
    
    // B777系列 (宽体客机 - Wide Body "Triple Seven")
    aircrafts.push_back(new Aircraft("B777-200ER", 8, 32, 8, 28, 9));        // 索引21: 684座三舱配置
    aircrafts.push_back(new Aircraft("B777-300ER", 12, 40, 10, 34, 9));      // 索引22: 864座三舱配置
    
    // B787系列 (宽体客机 - Wide Body "Dreamliner")
    aircrafts.push_back(new Aircraft("B787-8", 8, 28, 8, 24, 9));            // 索引23: 612座三舱配置
    aircrafts.push_back(new Aircraft("B787-9", 8, 32, 8, 28, 9));            // 索引24: 684座三舱配置
    aircrafts.push_back(new Aircraft("B787-10", 8, 36, 8, 32, 9));           // 索引25: 756座三舱配置
}

// ==================== 初始化航班网络 ====================
void AirlineData::initializeFlights(std::vector<Flight*>& flights, const std::vector<Aircraft*>& aircrafts, 
                                   const std::vector<Crew*>& crewMembers, const std::vector<Airport*>& airports) {
    // 尝试从CSV文件加载
    if (DataLoader::loadFlights("data/flights.csv", flights, aircrafts, crewMembers, airports)) {
        return; // 成功从文件加载
    }
    
    // 如果文件加载失败,使用默认数据
    std::cout << "Failed to load from CSV file, using default flight data..." << std::endl;
    // 航班命名规则: 航空公司代码 + 航班号
    // CA - Air China (中国国际航空)
    // MU - China Eastern (中国东方航空)
    // CZ - China Southern (中国南方航空)
    // BA - British Airways
    // AF - Air France
    // LH - Lufthansa
    // EK - Emirates
    // QR - Qatar Airways
    // SQ - Singapore Airlines
    // QF - Qantas Airways
    // UA - United Airlines
    // AA - American Airlines
    // TK - Turkish Airlines
    // AC - Air Canada
    
    // 亚洲-太平洋航线 (Asia-Pacific Routes)
    flights.push_back(new Flight("CA101", "PEK", "HKG", "08:30", "12:00", aircrafts[1]));     // Beijing-Hong Kong
    flights.push_back(new Flight("CA981", "PEK", "HND", "09:00", "13:30", aircrafts[10]));    // Beijing-Tokyo
    flights.push_back(new Flight("CA183", "PEK", "ICN", "10:00", "13:00", aircrafts[2]));     // Beijing-Seoul
    flights.push_back(new Flight("CA925", "PEK", "SIN", "14:00", "20:30", aircrafts[23]));    // Beijing-Singapore
    flights.push_back(new Flight("CA177", "PEK", "SYD", "23:00", "13:30+1", aircrafts[24]));  // Beijing-Sydney
    
    // 跨太平洋航线 (Trans-Pacific Routes)
    flights.push_back(new Flight("CA989", "PEK", "LAX", "15:30", "11:00", aircrafts[24]));    // Beijing-Los Angeles (13hrs)
    flights.push_back(new Flight("UA888", "LAX", "PEK", "13:00", "17:30+1", aircrafts[24]));  // Los Angeles-Beijing
    flights.push_back(new Flight("AA127", "LAX", "HND", "11:30", "15:00+1", aircrafts[23]));  // Los Angeles-Tokyo
    flights.push_back(new Flight("UA79", "ORD", "PEK", "14:00", "16:30+1", aircrafts[19]));   // Chicago-Beijing
    flights.push_back(new Flight("AA126", "DFW", "HND", "12:00", "16:30+1", aircrafts[24]));  // Dallas-Tokyo
    
    // 跨大西洋航线 (Trans-Atlantic Routes)
    flights.push_back(new Flight("BA112", "LHR", "ATL", "10:30", "14:30", aircrafts[24]));    // London-Atlanta
    flights.push_back(new Flight("BA115", "LHR", "ORD", "11:00", "14:00", aircrafts[23]));    // London-Chicago
    flights.push_back(new Flight("AF066", "CDG", "ATL", "10:45", "14:45", aircrafts[10]));    // Paris-Atlanta
    flights.push_back(new Flight("LH400", "FRA", "ORD", "13:10", "16:00", aircrafts[12]));    // Frankfurt-Chicago
    flights.push_back(new Flight("UA14", "ORD", "LHR", "17:10", "07:05+1", aircrafts[23]));   // Chicago-London
    
    // 欧洲航线 (European Routes)
    flights.push_back(new Flight("BA308", "LHR", "CDG", "07:00", "09:30", aircrafts[2]));     // London-Paris
    flights.push_back(new Flight("LH900", "FRA", "LHR", "06:55", "07:40", aircrafts[1]));     // Frankfurt-London
    flights.push_back(new Flight("AF1518", "CDG", "AMS", "08:00", "09:15", aircrafts[0]));    // Paris-Amsterdam
    flights.push_back(new Flight("BA674", "LHR", "IST", "10:00", "15:30", aircrafts[3]));     // London-Istanbul
    
    // 中东-亚洲航线 (Middle East-Asia Routes)
    flights.push_back(new Flight("EK302", "DXB", "PEK", "03:10", "14:45", aircrafts[12]));    // Dubai-Beijing
    flights.push_back(new Flight("EK306", "DXB", "HKG", "08:30", "20:45", aircrafts[12]));    // Dubai-Hong Kong
    flights.push_back(new Flight("EK308", "DXB", "SIN", "09:45", "21:00", aircrafts[12]));    // Dubai-Singapore
    flights.push_back(new Flight("QR888", "DOH", "PEK", "02:20", "14:30", aircrafts[11]));    // Doha-Beijing
    flights.push_back(new Flight("QR920", "DOH", "SIN", "08:15", "21:55", aircrafts[10]));    // Doha-Singapore
    
    // 中东-欧洲航线 (Middle East-Europe Routes)
    flights.push_back(new Flight("EK001", "DXB", "LHR", "08:00", "12:40", aircrafts[12]));    // Dubai-London
    flights.push_back(new Flight("EK045", "DXB", "CDG", "09:00", "14:00", aircrafts[11]));    // Dubai-Paris
    flights.push_back(new Flight("QR015", "DOH", "LHR", "08:10", "13:10", aircrafts[11]));    // Doha-London
    flights.push_back(new Flight("TK1980", "IST", "LHR", "09:00", "11:30", aircrafts[3]));    // Istanbul-London
    
    // 东南亚-大洋洲航线 (Southeast Asia-Oceania Routes)
    flights.push_back(new Flight("SQ221", "SIN", "SYD", "20:00", "06:00+1", aircrafts[11]));  // Singapore-Sydney
    flights.push_back(new Flight("QF001", "SYD", "SIN", "11:35", "17:00", aircrafts[12]));    // Sydney-Singapore
    flights.push_back(new Flight("SQ212", "SIN", "HKG", "14:00", "17:55", aircrafts[10]));    // Singapore-Hong Kong
    
    // 北美内陆航线 (North American Domestic Routes)
    flights.push_back(new Flight("AA2345", "ATL", "LAX", "09:00", "11:30", aircrafts[16]));   // Atlanta-Los Angeles
    flights.push_back(new Flight("UA1532", "ORD", "LAX", "08:30", "11:00", aircrafts[17]));   // Chicago-Los Angeles
    flights.push_back(new Flight("AA1050", "DFW", "LAX", "10:00", "11:30", aircrafts[15]));   // Dallas-Los Angeles
    flights.push_back(new Flight("AC759", "YYZ", "ORD", "08:00", "09:20", aircrafts[2]));     // Toronto-Chicago
}

// ==================== 初始化机组成员 ====================
void AirlineData::initializeCrewMembers(std::vector<Crew*>& crewMembers) {
    // 尝试从CSV文件加载
    if (DataLoader::loadCrewMembers("data/crew.csv", crewMembers)) {
        return; // 成功从文件加载
    }
    
    // 如果文件加载失败,使用默认数据
    std::cout << "Failed to load from CSV file, using default crew data..." << std::endl;
    // 机长 (Captains) - 持有多机型执照
    Crew* cpt1 = new Crew("P001", "Captain James Anderson", CrewRole::PILOT);
    cpt1->addQualification("B777-200ER");
    cpt1->addQualification("B777-300ER");
    cpt1->addQualification("B787-8");
    cpt1->addQualification("B787-9");
    cpt1->addQualification("B787-10");
    crewMembers.push_back(cpt1);

    Crew* cpt2 = new Crew("P002", "Captain Li Wei", CrewRole::PILOT);
    cpt2->addQualification("A350-900");
    cpt2->addQualification("A350-1000");
    cpt2->addQualification("A330-200");
    cpt2->addQualification("A330-300");
    cpt2->addQualification("A330-900neo");
    crewMembers.push_back(cpt2);

    Crew* cpt3 = new Crew("P003", "Captain Sarah Mitchell", CrewRole::PILOT);
    cpt3->addQualification("A380-800");
    crewMembers.push_back(cpt3);

    Crew* cpt4 = new Crew("P004", "Captain Zhang Ming", CrewRole::PILOT);
    cpt4->addQualification("B737-700");
    cpt4->addQualification("B737-800");
    cpt4->addQualification("B737-900ER");
    cpt4->addQualification("B737 MAX 8");
    cpt4->addQualification("B737 MAX 9");
    crewMembers.push_back(cpt4);

    Crew* cpt5 = new Crew("P005", "Captain Emma Thompson", CrewRole::PILOT);
    cpt5->addQualification("A320-200");
    cpt5->addQualification("A320neo");
    cpt5->addQualification("A321-200");
    cpt5->addQualification("A321neo");
    cpt5->addQualification("A321LR");
    cpt5->addQualification("A321XLR");
    crewMembers.push_back(cpt5);

    Crew* cpt6 = new Crew("P006", "Captain Robert Johnson", CrewRole::PILOT);
    cpt6->addQualification("B747-400");
    cpt6->addQualification("B747-8");
    cpt6->addQualification("B767-300ER");
    crewMembers.push_back(cpt6);

    // 副驾驶 (First Officers)
    Crew* fo1 = new Crew("P101", "First Officer Tom Wilson", CrewRole::PILOT);
    fo1->addQualification("B787-9");
    fo1->addQualification("B777-300ER");
    crewMembers.push_back(fo1);

    Crew* fo2 = new Crew("P102", "First Officer Wang Fang", CrewRole::PILOT);
    fo2->addQualification("A350-900");
    fo2->addQualification("A330-300");
    crewMembers.push_back(fo2);

    Crew* fo3 = new Crew("P103", "First Officer Michael Brown", CrewRole::PILOT);
    fo3->addQualification("B737-800");
    fo3->addQualification("B737 MAX 8");
    crewMembers.push_back(fo3);

    // 乘务长 (Purser)
    crewMembers.push_back(new Crew("C001", "Purser Mary Johnson", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C002", "Purser Liu Mei", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C003", "Purser Jennifer Garcia", CrewRole::CABIN_CREW));

    // 乘务员 (Flight Attendants)
    crewMembers.push_back(new Crew("C101", "FA Linda Davis", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C102", "FA Zhang Li", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C103", "FA Patricia Martinez", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C104", "FA Kim Min-ji", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C105", "FA Sophie Dubois", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C106", "FA Anna Schmidt", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C107", "FA Yuki Tanaka", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C108", "FA Ahmed Hassan", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C109", "FA Isabella Rossi", CrewRole::CABIN_CREW));
    crewMembers.push_back(new Crew("C110", "FA Chen Wei", CrewRole::CABIN_CREW));
}
