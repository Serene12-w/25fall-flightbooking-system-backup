# 完整数据持久化实现说明

## 📋 更新概览

本次更新实现了**完整的数据持久化**功能,包括:
1. ✅ 航班机组成员持久化到 `flights.csv`
2. ✅ 乘客信息持久化到 `passengers.csv`
3. ✅ 预订信息持久化到 `reservations.csv`
4. ✅ 系统启动时自动加载所有数据
5. ✅ 预订/取消/确认时自动保存到CSV

---

## 📁 CSV文件格式

### 1. flights.csv (已扩展)

**新增字段**: `crewIds` - 机组成员ID列表(逗号分隔,需引号)

**完整格式**:
```csv
flightNumber,origin,destination,departureTime,arrivalTime,aircraftType,route,cruisingAltitude,cruisingSpeed,estimatedFuel,alternateAirports,crewIds
CA989,PEK,LAX,15:30,11:00,B787-9,"PEK DCT VAMUM DCT NIKRO DCT R220 DCT LAX",FL350,M0.82,45000,"SFO,ONT","P001,CC001,CC002"
```

**字段说明**:
- `crewIds`: 机组成员ID列表,格式如 `"P001,CC001,CC002"` (飞行员+乘务员)
  - P开头 = 飞行员 (Pilot)
  - CC开头 = 乘务员 (Cabin Crew)

---

### 2. passengers.csv (新建)

**文件位置**: `data/passengers.csv`

**格式**:
```csv
passportNumber,name,loyaltyStatus
G12345678,Zhang Wei,GOLD
P87654321,John Smith,PLATINUM
```

**字段说明**:
- `passportNumber`: 护照号(唯一标识)
- `name`: 乘客姓名
- `loyaltyStatus`: 会员等级
  - `NONE` - 普通乘客
  - `SILVER` - 银卡会员
  - `GOLD` - 金卡会员
  - `PLATINUM` - 白金卡会员

**示例数据**:
- 10个示例乘客,涵盖不同国籍和会员等级
- 护照号格式: G=中国, P=美国, E=欧洲, J=日本

---

### 3. reservations.csv (新建)

**文件位置**: `data/reservations.csv`

**格式**:
```csv
reservationNumber,passportNumber,flightNumber,seatRow,seatCol,status
RES001,G12345678,CA989,1,A,CONFIRMED
RES002,G23456789,BA168,2,C,PENDING
```

**字段说明**:
- `reservationNumber`: 预订号(唯一标识)
- `passportNumber`: 乘客护照号(关联 passengers.csv)
- `flightNumber`: 航班号(关联 flights.csv)
- `seatRow`: 座位行号
- `seatCol`: 座位列号(A-J)
- `status`: 预订状态
  - `PENDING` - 待确认
  - `CONFIRMED` - 已确认
  - `CANCELLED` - 已取消
  - `WAITLISTED` - 候补中

**示例数据**:
- 8个示例预订记录
- 涵盖不同航班、座位等级和预订状态
- 座位分布: 头等舱(1-8行)、商务舱、超级经济舱、经济舱

---

## 🔧 代码改进

### Flight 类 (Flight.h / Flight.cpp)

**新增方法**:
```cpp
// 获取机组成员列表
const std::vector<Crew*>& getAssignedCrew() const;
```

**用途**: 用于保存航班时获取分配的机组成员

---

### DataLoader 类 (DataLoader.h / DataLoader.cpp)

**修改的函数签名**:

1. **loadFlights()** - 新增 crewMembers 参数
```cpp
static bool loadFlights(const std::string& filename, std::vector<Flight*>& flights, 
                       const std::vector<Aircraft*>& aircrafts, 
                       const std::vector<Crew*>& crewMembers);
```

2. **saveFlights()** - 保存机组成员到CSV
```cpp
static bool saveFlights(const std::string& filename, const std::vector<Flight*>& flights);
```
- 自动将航班的 `assignedCrew` 转换为逗号分隔的ID列表

3. **loadReservations()** - 修改为使用 unordered_map
```cpp
static bool loadReservations(const std::string& filename, 
                             std::unordered_map<std::string, Reservation*>& reservations,
                             const std::vector<Passenger*>& passengers, 
                             const std::vector<Flight*>& flights);
```
- 自动恢复座位状态(调用 `bookSeat()`)
- 根据状态字段设置预订状态

4. **saveReservations()** - 修改为使用 unordered_map
```cpp
static bool saveReservations(const std::string& filename, 
                             const std::unordered_map<std::string, Reservation*>& reservations);
```

---

### AirlineReservationSystem 类

**新增成员变量**:
```cpp
std::vector<Passenger*> passengers; // 乘客列表
```

**修改的函数**:

1. **initializeData()** - 自动加载所有数据
```cpp
void AirlineReservationSystem::initializeData() {
    // 1. 加载飞机型号
    AirlineData::initializeAircrafts(aircrafts);
    
    // 2. 加载机组成员
    AirlineData::initializeCrewMembers(crewMembers);
    
    // 3. 加载航班(包含机组成员关联)
    AirlineData::initializeFlights(flights, aircrafts, crewMembers);
    
    // 4. 加载乘客数据
    DataLoader::loadPassengers("data/passengers.csv", passengers);
    
    // 5. 加载预订数据(自动恢复座位状态)
    DataLoader::loadReservations("data/reservations.csv", reservations, passengers, flights);
    
    // 6. 构建航班图
    for (const auto& flight : flights) {
        flightGraph[flight->getOrigin()].push_back(flight);
    }
}
```

2. **cancelReservation()** - 自动保存
```cpp
void AirlineReservationSystem::cancelReservation() {
    // ... 取消逻辑 ...
    
    // 保存更改到CSV
    saveReservationsData();
}
```

3. **confirmReservation()** - 自动保存
```cpp
void AirlineReservationSystem::confirmReservation() {
    // ... 确认逻辑 ...
    
    // 保存更改到CSV
    saveReservationsData();
}
```

4. **新增保存函数**:
```cpp
void savePassengersData();    // 保存乘客数据
void saveReservationsData();  // 保存预订数据
```

---

## 🎯 如何使用

### 1. 系统启动时
```
✅ 自动从 CSV 文件加载所有数据:
   - aircrafts.csv → 22个飞机型号
   - crew.csv → 20个机组成员
   - flights.csv → 16个航班(含机组成员)
   - passengers.csv → 10个乘客
   - reservations.csv → 8个预订(自动恢复座位状态)
```

### 2. 添加新航班时
```
管理员 → 管理航班 → 添加航班
填写航班信息 → 选择机组成员
↓
自动保存到 flights.csv (包含机组成员ID)
```

### 3. 乘客预订时
```
乘客 → 预订机票 → 选择航班和座位
输入护照号和姓名
↓
如果是新乘客:
  - 自动添加到 passengers.csv
  - 自动添加到预订树
↓
自动保存到 reservations.csv
```

### 4. 取消预订时
```
乘客 → 取消预订 → 输入预订号
↓
座位状态更新(unbook)
预订状态更新(CANCELLED)
↓
自动保存到 reservations.csv
```

### 5. 确认预订时
```
乘客 → 确认预订 → 输入预订号
↓
预订状态更新(PENDING → CONFIRMED)
↓
自动保存到 reservations.csv
```

---

## 📊 数据流程图

```
[系统启动]
    ↓
[加载 CSV 数据]
    ├─ aircrafts.csv → 飞机型号
    ├─ crew.csv → 机组成员
    ├─ flights.csv → 航班 + 机组成员关联
    ├─ passengers.csv → 乘客信息
    └─ reservations.csv → 预订 + 座位状态恢复
    ↓
[系统运行]
    ├─ 预订机票 → saveReservationsData()
    ├─ 取消预订 → saveReservationsData()
    ├─ 确认预订 → saveReservationsData()
    ├─ 添加航班 → saveFlightsData()
    └─ 分配机组 → saveFlightsData()
    ↓
[系统关闭]
    └─ 所有数据已持久化到 CSV
```

---

## ✨ 主要优势

### 1. 完全持久化
- ✅ **所有数据**都存储在 CSV 文件中
- ✅ **系统重启后**数据不丢失
- ✅ **座位状态**自动恢复
- ✅ **机组成员分配**自动恢复

### 2. 自动保存
- ✅ 预订/取消/确认时**自动保存**
- ✅ 添加航班/分配机组时**自动保存**
- ✅ 不需要手动调用保存函数

### 3. 数据一致性
- ✅ 乘客通过**护照号**关联
- ✅ 航班通过**航班号**关联
- ✅ 预订通过**预订号**唯一标识
- ✅ 座位状态与预订状态**同步**

### 4. 易于管理
- ✅ CSV 格式简单直观
- ✅ 可用 Excel 或文本编辑器修改
- ✅ 支持批量导入/导出
- ✅ 易于备份和版本控制

---

## 🔍 示例场景

### 场景1: 查看预订的航班座位图
```
1. 系统启动,加载 reservations.csv
2. CA989 航班的座位 1A, 1B, 2A 被标记为已预订
3. 显示座位图时,这些座位显示为 "0" (已占用)
4. 乘客只能选择其他空座位
```

### 场景2: 取消预订后的座位释放
```
1. 乘客取消预订 RES001 (CA989, 1A)
2. 座位 1A 被释放 (unbook)
3. 预订状态更新为 CANCELLED
4. 保存到 reservations.csv
5. 下次系统启动时,座位 1A 为可用状态
```

### 场景3: 查看航班机组成员
```
1. 管理员查看 CA989 航班
2. 系统显示机组成员:
   - P001 (Captain Zhang Wei) - 飞行员
   - CC001 (Li Na) - 乘务员
   - CC002 (Wang Xin) - 乘务员
3. 这些信息来自 flights.csv 的 crewIds 字段
```

---

## 📝 注意事项

### 1. CSV 文件编辑
- ⚠️ 包含逗号的字段必须用**双引号**包围
- ⚠️ 例如: `"P001,CC001,CC002"` 或 `"SFO,ONT"`
- ⚠️ 删除引号会导致解析错误

### 2. 数据关联
- ⚠️ 预订中的 `passportNumber` 必须在 `passengers.csv` 中存在
- ⚠️ 预订中的 `flightNumber` 必须在 `flights.csv` 中存在
- ⚠️ 航班中的 `crewIds` 必须在 `crew.csv` 中存在

### 3. 座位号格式
- ⚠️ 行号: 从 1 开始的整数
- ⚠️ 列号: A-J 的大写字母
- ⚠️ 示例: 1A, 10C, 25F

### 4. 状态值
- ⚠️ 预订状态: PENDING / CONFIRMED / CANCELLED / WAITLISTED
- ⚠️ 会员等级: NONE / SILVER / GOLD / PLATINUM
- ⚠️ 大小写敏感,必须使用大写

---

## 🎉 总结

本次更新实现了**完整的数据持久化系统**,所有航空数据现在都存储在 CSV 文件中:

| CSV 文件 | 内容 | 关键字段 |
|---------|------|---------|
| `aircrafts.csv` | 飞机型号配置 | type, 舱位行数, 每行座位数 |
| `crew.csv` | 机组成员 | id, name, role, qualifications |
| `flights.csv` | 航班信息 | flightNumber, 飞行计划, **crewIds** ⭐ |
| `passengers.csv` | 乘客信息 | passportNumber, name, loyaltyStatus ⭐ 新建 |
| `reservations.csv` | 预订记录 | reservationNumber, 座位, status ⭐ 新建 |

**系统现在具备完整的数据持久化能力!** 🚀
