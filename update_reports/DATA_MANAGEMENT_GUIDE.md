# 航空数据管理快速指南
# Quick Guide for Airline Data Management

**⚠️ 重要提示**: 所有航空数据使用CSV文件管理，简单易用！

---

## 📁 数据文件位置

**数据文件夹**: `data/`

包含三个CSV文件：
- ✈️ `aircrafts.csv` - 所有飞机型号
- 🛫 `flights.csv` - 所有航班信息
- 👨‍✈️ `crew.csv` - 所有机组成员

---

## 🔧 如何添加/修改数据

### 1️⃣ 添加新飞机型号

**文件**: `data/aircrafts.csv`

**CSV格式**:
```csv
type,firstClassRows,businessClassRows,premiumEconomyRows,economyClassRows,seatsPerRow
B787-9,8,28,20,48,9
```

**字段说明**:
- `type`: 飞机型号名称
- `firstClassRows`: 头等舱行数
- `businessClassRows`: 商务舱行数
- `premiumEconomyRows`: 超级经济舱行数
- `economyClassRows`: 经济舱行数
- `seatsPerRow`: 每行座位数

**操作步骤**:
1. 用Excel或文本编辑器打开 `data/aircrafts.csv`
2. 在最后添加新的一行
3. 保存文件
4. 重新运行程序即可
    8,            // 头等舱行数
    32,           // 商务舱行数  
    8,            // 超级经济舱行数 ⭐新增
    28,           // 经济舱行数
    9             // 每行座位数
));
// 这将是下一个索引,如索引26
```

**重要**: 记住飞机在数组中的索引（从0开始），添加航班时需要用到！

---

### 2️⃣ 添加新航班

**文件**: `data/flights.csv`

**CSV格式**:
```csv
flightNumber,origin,destination,departureTime,arrivalTime,aircraftType
CA989,PEK,LAX,15:30,11:00,B787-9
```

**字段说明**:
- `flightNumber`: 航班号（航空公司代码+数字）
- `origin`: 出发地IATA代码（3字母）
- `destination`: 目的地IATA代码（3字母）
- `departureTime`: 起飞时间（HH:MM格式）
- `arrivalTime`: 到达时间（HH:MM或HH:MM+1表示跨天）
- `aircraftType`: 飞机型号（必须在aircrafts.csv中存在）

**操作步骤**:
1. 用Excel或文本编辑器打开 `data/flights.csv`
2. 在最后添加新的一行
3. 确保aircraftType在aircrafts.csv中存在
4. 保存文件
5. 重新运行程序即可

**常用航空公司代码**:
- CA = 中国国际航空
- MU = 中国东方航空
- CZ = 中国南方航空
- BA = 英国航空
- AF = 法国航空
- EK = 阿联酋航空
- UA = 美联航
- AA = 美国航空

---

### 3️⃣ 添加机组成员

**文件**: `data/crew.csv`

**CSV格式**:
```csv
id,name,role,qualifications
P001,Captain Zhang Wei,PILOT,"B787-9,B787-8,B787-10,B777-300ER"
CC001,Flight Attendant Li Na,CABIN_CREW,"A380-800,A350-900,B787-9"
```

**字段说明**:
- `id`: 员工ID（P开头=飞行员，CC开头=乘务员）
- `name`: 姓名
- `role`: 角色（PILOT 或 CABIN_CREW）
- `qualifications`: 资质列表（多个用逗号分隔，需用引号包围）

**操作步骤**:
1. 用Excel或文本编辑器打开 `data/crew.csv`
2. 在最后添加新的一行
3. 飞行员需要填写资质（可驾驶的飞机型号）
4. 乘务员的资质可以为空或填写培训过的机型
5. 保存文件
6. 重新运行程序即可

---

## 🪑 座位图显示说明

系统现在使用数字显示座位类型：

| 数字 | 含义 | 说明 |
|-----|------|------|
| **4** | 头等舱 | First Class |
| **3** | 商务舱 | Business Class |
| **2** | 超级经济舱 | Premium Economy ⭐新增 |
| **1** | 经济舱 | Economy Class |
| **0** | 已被占用 | Occupied/Booked |

**示例座位图**:
```
   A B C D E F
 1 4 4 4 4 4 4   <- 头等舱
 2 3 3 3 3 3 3   <- 商务舱
 3 3 0 3 3 3 3   <- 商务舱 (B座已被占用)
 4 2 2 2 2 2 2   <- 超级经济舱
 5 1 1 1 1 1 1   <- 经济舱
```

---

## � CSV文件优势

✅ **简单直观** - 纯文本格式，易于理解  
✅ **Excel兼容** - 可直接用Excel打开编辑  
✅ **无需编译** - 修改数据后直接运行程序  
✅ **易于备份** - 直接复制CSV文件即可  
✅ **版本控制友好** - 适合Git等版本控制系统  

---

## ⚠️ 注意事项

1. **CSV格式要求**
   - 第一行是标题行，不要删除
   - 字段用逗号分隔
   - 包含逗号的字段需要用引号包围（如资质列表）
   
2. **数据一致性**
   - flights.csv中的aircraftType必须在aircrafts.csv中存在
   - 机场代码应使用标准的3字母IATA代码
   - 时间格式统一为 "HH:MM"

3. **编码格式**
   - 建议使用UTF-8编码保存CSV文件
   - 避免使用Excel的默认编码（可能导致中文乱码）

---

## 🛠️ 编译和运行

### 方法1: 命令行
```bash
g++ -g src/*.cpp -I include -o airline_reservation.exe
./airline_reservation.exe
```

### 方法2: VS Code
按 `Ctrl+Shift+B` 或选择 `Terminal > Run Build Task`

---

## ✅ 修改检查清单

每次修改数据后，请检查：
- [ ] 在 `src/AirlineData.cpp` 中修改了数据
- [ ] 飞机索引使用正确
- [ ] 机场代码为3字母IATA代码（大写）
- [ ] 航班号格式正确
- [ ] 时间格式为 "HH:MM"
- [ ] 跨天航班使用 "+1" 标记
- [ ] 机型参数包含5个值（新增超级经济舱）
- [ ] 编译成功无错误

---

## 📝 示例：添加完整航线

假设我们要添加从上海到洛杉矶的航班：

```cpp
// 1. 选择或添加飞机（已存在B787-9，索引24）

// 2. 在 initializeFlights() 中添加
flights.push_back(new Flight(
    "MU577",        // 东航577
    "PVG",          // 上海浦东
    "LAX",          // 洛杉矶
    "16:30",        // 下午4:30起飞
    "12:00",        // 中午12点到达（当天）
    aircrafts[24]   // B787-9
));

// 3. 重新编译运行即可！
```

---

## 🔍 常见机场IATA代码

| 代码 | 城市 |
|-----|------|
| PEK | 北京首都 |
| PVG | 上海浦东 |
| HKG | 香港 |
| HND | 东京羽田 |
| ICN | 首尔仁川 |
| SIN | 新加坡 |
| LAX | 洛杉矶 |
| SFO | 旧金山 |
| JFK | 纽约肯尼迪 |
| LHR | 伦敦希思罗 |
| CDG | 巴黎戴高乐 |
| FRA | 法兰克福 |
| DXB | 迪拜 |
| SYD | 悉尼 |

完整列表请参考 `AIRPORT_DATABASE.md`

---

## 💡 提示

1. **不要直接修改** `AirlineReservationSystem.cpp` 中的 `initializeData()` 函数
2. **所有数据** 都在 `AirlineData.cpp` 中管理
3. **添加飞机后** 记得更新索引
4. **测试新航班** 前先编译确保无错误

---

**祝您使用愉快！** ✈️

*最后更新: 2025年11月7日*
