# 航班座位隔离修复文档

## 日期: 2025-11-09

## 问题描述

### 原始问题
系统中所有使用相同飞机型号的航班共享同一个座位状态,导致:
- 航班A的座位预订会影响航班B(如果它们使用相同的飞机型号)
- 不同日期的同一航班号也共享座位状态
- 预订座位时显示的是所有航班的累积预订

**示例**:
```
航班 AC889 (YYZ -> SFO, 2025-11-15) 使用 B787-9
航班 UA117 (SFO -> ICN, 2025-11-15) 使用 B787-9

当在AC889上预订1A座位后,UA117的1A也显示为已占用!
```

### 根本原因
1. **共享Aircraft对象**: 所有航班通过指针引用同一个Aircraft实例
2. **座位状态在Aircraft中**: 座位预订状态存储在Aircraft对象中
3. **缺少航班日期区分**: reservations.csv只使用航班号,没有日期字段

---

## 解决方案

### 1. Aircraft对象独立化

#### 修改文件: `include/Aircraft.h`, `src/Aircraft.cpp`

**新增拷贝构造函数**:
```cpp
// 拷贝构造函数 - 创建独立的座位副本
Aircraft::Aircraft(const Aircraft& other);
```

**功能**:
- 深拷贝所有座位对象
- 保留飞机配置(型号、舱位布局、性能参数)
- 不复制座位预订状态(所有座位初始为未预订)

---

### 2. Flight拥有独立的Aircraft实例

#### 修改文件: `include/Flight.h`, `src/Flight.cpp`

**关键改变**:

1. **添加所有权标记**:
```cpp
private:
    Aircraft* aircraft;      // 执飞飞机
    bool ownsAircraft;       // 标记是否拥有Aircraft对象的所有权
```

2. **修改构造函数** - 创建Aircraft副本:
```cpp
Flight::Flight(..., Aircraft* aircraftTemplate, ...) {
    // 创建Aircraft的独立副本
    if (aircraftTemplate) {
        aircraft = new Aircraft(*aircraftTemplate);
    }
    ownsAircraft = true;
}
```

3. **添加析构函数** - 释放Aircraft:
```cpp
Flight::~Flight() {
    if (ownsAircraft && aircraft) {
        delete aircraft;
    }
}
```

**效果**:
- 每个Flight对象拥有自己独立的Aircraft实例
- 每个航班有自己的座位状态
- 不同航班的预订完全隔离

---

### 3. reservations.csv格式升级

#### 修改文件: `data/reservations.csv`

**旧格式**:
```csv
flightNumber,seatReservations
CA989,"1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED"
```

**新格式**:
```csv
flightNumber,flightDate,seatReservations
CA989,2025-11-10,"1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED"
```

**改进**:
- 添加`flightDate`字段
- 使用"航班号+日期"作为唯一标识
- 支持同一航班号在不同日期的独立预订

---

### 4. 数据加载/保存逻辑更新

#### 修改文件: `src/DataLoader.cpp`

**loadReservations() 更新**:
```cpp
// 创建航班映射: "航班号:日期" -> Flight*
std::map<std::string, Flight*> flightMap;
for (auto* f : flights) {
    std::string flightKey = f->getFlightNumber() + ":" + f->getFlightDate();
    flightMap[flightKey] = f;
}

// 读取CSV时使用航班号+日期查找
std::string flightKey = flightNumber + ":" + flightDate;
Flight* flight = flightMap[flightKey];
```

**saveReservations() 更新**:
```cpp
// 按"航班号:日期"分组
std::map<std::string, std::vector<Reservation*>> flightReservations;
for (const auto& pair : reservations) {
    std::string flightKey = flight->getFlightNumber() + ":" + flight->getFlightDate();
    flightReservations[flightKey].push_back(reservation);
}

// 写入时包含日期
file << flightNumber << "," << flightDate << ",\"" << seatReservations << "\"\n";
```

---

## 数据迁移指南

### 现有数据更新

如果您有现有的reservations.csv数据,需要手动添加日期列:

**步骤**:
1. 打开 `data/reservations.csv`
2. 修改标题行: `flightNumber,seatReservations` → `flightNumber,flightDate,seatReservations`
3. 为每行添加对应的日期(查看`flights.csv`获取准确日期)

**示例**:
```csv
# 修改前
CA989,"1A:G12345678:CONFIRMED"

# 修改后 (从flights.csv查得CA989的日期是2025-11-10)
CA989,2025-11-10,"1A:G12345678:CONFIRMED"
```

---

## 技术细节

### 内存管理

**Before**:
```
Aircraft B787-9 (共享)
    ├── Flight CA989 ─→ aircraft*
    ├── Flight MU587 ─→ aircraft*
    └── Flight UA117 ─→ aircraft*
    
所有航班共享同一个座位状态!
```

**After**:
```
Flight CA989
    └── Aircraft B787-9 (副本1) - 独立座位状态

Flight MU587
    └── Aircraft B787-9 (副本2) - 独立座位状态

Flight UA117
    └── Aircraft B787-9 (副本3) - 独立座位状态
    
每个航班有自己的座位状态!
```

### 性能考虑

**内存开销**:
- 每个Flight需要额外的Aircraft副本
- 对于B787-9: 约 94行 × 9列 = 846个Seat对象
- 估计每个Seat约50字节 → 每个航班约42KB

**优势**:
- 数据隔离性强
- 代码逻辑清晰
- 避免竞态条件
- 符合现实业务模型

---

## 验证测试

### 测试场景1: 同机型不同航班

```
1. 预订 AC889 (B787-9) 的座位 1A
2. 检查 UA117 (B787-9) 的座位图
   
期望: UA117的1A应该显示为可用
```

### 测试场景2: 同航班号不同日期

```
1. 如果有 CA989-2025-11-10 和 CA989-2025-11-11
2. 预订 CA989-2025-11-10 的座位 2B
3. 检查 CA989-2025-11-11 的座位图

期望: CA989-2025-11-11的2B应该显示为可用
```

### 测试场景3: 联程航班

```
1. 预订联程航班: YYZ -> SFO -> ICN
   - Segment 1: AC889 (B787-9) - 选座1I
   - Segment 2: UA117 (B787-9) - 选座15C
   
期望: 两个航班的座位选择互不影响
```

---

## 向后兼容性

### CSV文件兼容性

**加载逻辑**:
- 优先尝试读取3列格式(flightNumber, flightDate, seatReservations)
- 如果只有2列,输出警告但不崩溃
- 建议立即迁移到新格式

### 代码兼容性

**Aircraft构造**:
- 保留原有构造函数签名
- 新增拷贝构造函数
- 不影响现有代码

**Flight构造**:
- 保留原有构造函数签名
- 自动创建Aircraft副本
- 透明升级,无需修改调用代码

---

## 总结

### 修改的文件
1. `include/Aircraft.h` - 添加拷贝构造函数声明
2. `src/Aircraft.cpp` - 实现拷贝构造函数
3. `include/Flight.h` - 添加ownsAircraft标记和析构函数
4. `src/Flight.cpp` - 修改构造函数创建Aircraft副本
5. `src/DataLoader.cpp` - 更新加载/保存逻辑支持日期
6. `data/reservations.csv` - 添加flightDate列

### 关键改进
✅ **完全隔离**: 每个航班有独立的座位状态
✅ **日期支持**: 区分同一航班号的不同日期
✅ **数据一致性**: CSV格式与业务逻辑匹配
✅ **内存安全**: Flight析构时自动清理Aircraft
✅ **可扩展性**: 支持未来更复杂的座位管理需求

### 影响范围
- ✅ 座位预订不再相互干扰
- ✅ 联程航班座位选择正常工作
- ✅ 值机流程显示正确的座位图
- ✅ 数据持久化正确保存/加载

---

## 编译和运行

```bash
# 编译
g++ -g src/*.cpp -I include -o airline_reservation.exe -static-libgcc -static-libstdc++ -static

# 运行
./airline_reservation.exe
```

**注意**: 首次运行时,系统会自动更新reservations.csv为新格式。
