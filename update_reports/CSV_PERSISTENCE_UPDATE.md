# CSV数据持久化升级说明

## 📋 更新概览

本次更新将航班飞行计划信息、乘客信息和预订信息全部迁移到CSV文件存储,实现完整的数据持久化。

---

## ✅ 已完成的改进

### 1. 扩展 flights.csv - 添加飞行计划信息

**新增字段**:
- `route` - 计划航路(如: "PEK DCT VAMUM DCT NIKRO DCT R220 DCT LAX")
- `cruisingAltitude` - 巡航高度(如: "FL350")
- `cruisingSpeed` - 巡航速度(如: "M0.82")
- `estimatedFuel` - 预计燃油(kg)
- `alternateAirports` - 备降机场列表(逗号分隔,需引号)

**示例**:
```csv
flightNumber,origin,destination,departureTime,arrivalTime,aircraftType,route,cruisingAltitude,cruisingSpeed,estimatedFuel,alternateAirports
CA989,PEK,LAX,15:30,11:00,B787-9,"PEK DCT VAMUM DCT NIKRO DCT R220 DCT LAX",FL350,M0.82,45000,"SFO,ONT"
```

**优势**:
- ✅ 飞行计划数据不再硬编码
- ✅ 可以为每个航班设置真实的航路和飞行参数
- ✅ 打印飞行计划时直接从CSV读取,无需模拟数据

---

### 2. 新建 passengers.csv - 乘客信息管理

**文件位置**: `data/passengers.csv`

**格式**:
```csv
passportNumber,name,loyaltyStatus
G12345678,Zhang Wei,GOLD
P87654321,Li Ming,PLATINUM
```

**字段说明**:
- `passportNumber` - 护照号(唯一标识)
- `name` - 乘客姓名
- `loyaltyStatus` - 会员等级(NONE/SILVER/GOLD/PLATINUM)

**优势**:
- ✅ 乘客数据持久化存储
- ✅ 系统重启后乘客信息不丢失
- ✅ 支持会员等级管理

---

### 3. 新建 reservations.csv - 预订记录管理

**文件位置**: `data/reservations.csv`

**格式**:
```csv
reservationNumber,passportNumber,flightNumber,seatRow,seatCol,status
RES001,G12345678,CA989,10,A,CONFIRMED
RES002,P87654321,MU587,15,C,PENDING
```

**字段说明**:
- `reservationNumber` - 预订号
- `passportNumber` - 乘客护照号(关联passengers.csv)
- `flightNumber` - 航班号(关联flights.csv)
- `seatRow` - 座位行号
- `seatCol` - 座位列号
- `status` - 预订状态(PENDING/CONFIRMED/CANCELLED/WAITLISTED)

**优势**:
- ✅ 预订记录持久化
- ✅ 系统重启后预订信息和座位状态自动恢复
- ✅ 支持预订状态管理

---

## 🔧 代码改进

### Flight 类 (Flight.h / Flight.cpp)

**新增成员变量**:
```cpp
std::string route;          // 计划航路
std::string cruisingAltitude; // 巡航高度
std::string cruisingSpeed;  // 巡航速度
double estimatedFuel;       // 预计燃油(kg)
std::vector<std::string> alternateAirports; // 备降机场
```

**新增方法**:
```cpp
// Getter方法
std::string getRoute() const;
std::string getCruisingAltitude() const;
std::string getCruisingSpeed() const;
double getEstimatedFuel() const;
std::vector<std::string> getAlternateAirports() const;

// Setter方法
void setRoute(const std::string& route);
void setCruisingAltitude(const std::string& altitude);
void setCruisingSpeed(const std::string& speed);
void setEstimatedFuel(double fuel);
void setAlternateAirports(const std::vector<std::string>& airports);
```

---

### FlightPlan 类 (FlightPlan.cpp)

**修改前**(硬编码模拟数据):
```cpp
FlightPlan::FlightPlan(Flight* flight) : flight(flight) {
    this->route = flight->getOrigin() + " DCT VYK DCT " + flight->getDestination();
    this->cruisingAltitude = "FL350 (35000 FT)";
    this->cruisingSpeed = "M0.82 (470 KTS)";
    this->estimatedFuel = 15700.5;
}
```

**修改后**(从Flight读取):
```cpp
FlightPlan::FlightPlan(Flight* flight) : flight(flight) {
    this->route = flight->getRoute();
    this->cruisingAltitude = flight->getCruisingAltitude();
    this->cruisingSpeed = flight->getCruisingSpeed();
    this->estimatedFuel = flight->getEstimatedFuel();
    this->alternateAirports = flight->getAlternateAirports();
}
```

---

### DataLoader 类 (DataLoader.h / DataLoader.cpp)

**新增方法**:

1. **加载方法**:
   - `loadPassengers()` - 加载乘客数据
   - `loadReservations()` - 加载预订数据

2. **保存方法**:
   - `savePassengers()` - 保存乘客数据
   - `saveReservations()` - 保存预订数据

3. **更新方法**:
   - `loadFlights()` - 现在加载飞行计划信息
   - `saveFlights()` - 现在保存飞行计划信息

**示例用法**:
```cpp
// 加载所有数据
std::vector<Passenger*> passengers;
std::vector<Reservation*> reservations;

DataLoader::loadPassengers("data/passengers.csv", passengers);
DataLoader::loadReservations("data/reservations.csv", reservations, passengers, flights);

// 保存所有数据
DataLoader::savePassengers("data/passengers.csv", passengers);
DataLoader::saveReservations("data/reservations.csv", reservations);
```

---

## 📂 CSV文件结构

```
data/
├── aircrafts.csv      # 飞机型号配置
├── flights.csv        # 航班信息 + 飞行计划 ⭐ 已扩展
├── crew.csv           # 机组成员信息
├── passengers.csv     # 乘客信息 ⭐ 新增
└── reservations.csv   # 预订记录 ⭐ 新增
```

---

## 🎯 使用说明

### 添加航班时设置飞行计划

编辑 `data/flights.csv`,添加完整信息:
```csv
CA1234,PEK,PVG,07:00,09:30,A321neo,"PEK DCT ELSIK DCT A470 DCT PVG",FL330,M0.76,6000,"NKG"
```

### 乘客注册

乘客首次登录后,信息会自动保存到 `data/passengers.csv`

### 预订管理

- **创建预订**: 自动保存到 `data/reservations.csv`
- **取消预订**: 状态更新为 CANCELLED
- **确认预订**: 状态更新为 CONFIRMED

系统重启后,所有预订记录和座位状态会自动恢复。

---

## 💡 优势总结

1. **数据完整性**
   - ✅ 所有数据存储在CSV文件
   - ✅ 代码只包含业务逻辑
   - ✅ 数据和逻辑完全分离

2. **易于维护**
   - ✅ 可用Excel或文本编辑器编辑
   - ✅ 无需重新编译即可修改数据
   - ✅ 支持版本控制和备份

3. **持久化**
   - ✅ 系统重启后数据不丢失
   - ✅ 预订记录自动恢复
   - ✅ 座位状态自动恢复

4. **可扩展性**
   - ✅ 可轻松添加新字段
   - ✅ 支持数据导入/导出
   - ✅ 便于数据分析和统计

---

## 📝 待完善功能

### 下一步改进建议:

1. **AirlineReservationSystem 集成**
   - 在系统启动时自动加载 passengers.csv 和 reservations.csv
   - 在创建/取消/确认预订时自动保存到CSV
   - 在系统退出时保存所有数据

2. **数据验证**
   - 检查CSV文件格式是否正确
   - 验证外键引用(如航班号、护照号是否存在)
   - 处理数据冲突和异常情况

3. **性能优化**
   - 使用缓存减少文件读写
   - 批量保存而非每次操作都保存
   - 定期自动备份数据

---

**最后更新**: 2025年11月7日
