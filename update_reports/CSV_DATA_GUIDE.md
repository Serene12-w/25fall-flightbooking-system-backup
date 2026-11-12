# CSV数据文件说明
# CSV Data Files Guide

本系统使用CSV文件存储所有航空数据，简单易用，易于维护。

---

## 📁 数据文件列表

### 1. aircrafts.csv - 飞机型号配置

**位置**: `data/aircrafts.csv`

**格式**:
```csv
type,firstClassRows,businessClassRows,premiumEconomyRows,economyClassRows,seatsPerRow
A319-100,0,8,0,16,6
B787-9,8,28,20,48,9
```

**字段说明**:
| 字段 | 说明 | 示例 |
|-----|------|------|
| type | 飞机型号名称 | B787-9 |
| firstClassRows | 头等舱行数 | 8 |
| businessClassRows | 商务舱行数 | 28 |
| premiumEconomyRows | 超级经济舱行数 | 20 |
| economyClassRows | 经济舱行数 | 48 |
| seatsPerRow | 每行座位数 | 9 |

---

### 2. flights.csv - 航班信息

**位置**: `data/flights.csv`

**格式**:
```csv
flightNumber,origin,destination,departureTime,arrivalTime,aircraftType
CA989,PEK,LAX,15:30,11:00,B787-9
```

**字段说明**:
| 字段 | 说明 | 示例 |
|-----|------|------|
| flightNumber | 航班号 | CA989 |
| origin | 出发地IATA代码（3字母） | PEK |
| destination | 目的地IATA代码（3字母） | LAX |
| departureTime | 起飞时间（HH:MM） | 15:30 |
| arrivalTime | 到达时间（HH:MM或HH:MM+1） | 11:00+1 |
| aircraftType | 飞机型号（必须存在于aircrafts.csv） | B787-9 |

**注意**: 
- 到达时间后加"+1"表示跨天到达
- aircraftType必须与aircrafts.csv中的type完全匹配

---

### 3. crew.csv - 机组成员信息

**位置**: `data/crew.csv`

**格式**:
```csv
id,name,role,qualifications
P001,Captain Zhang Wei,PILOT,"B787-9,B787-8,B787-10,B777-300ER"
CC001,Flight Attendant Li Na,CABIN_CREW,"A380-800,A350-900,B787-9"
```

**字段说明**:
| 字段 | 说明 | 示例 |
|-----|------|------|
| id | 员工ID（P=飞行员，CC=乘务员） | P001 |
| name | 姓名 | Captain Zhang Wei |
| role | 角色（PILOT或CABIN_CREW） | PILOT |
| qualifications | 资质列表（逗号分隔，需引号） | "B787-9,B777-300ER" |

**注意**: 
- 飞行员的资质表示可以驾驶的飞机型号
- 资质列表包含逗号时必须用双引号包围
- 乘务员的资质可以为空或填写培训过的机型

---

## 🔧 如何编辑CSV文件

### 方法1: 使用Excel
1. 用Excel打开CSV文件
2. 直接编辑单元格
3. 保存为CSV格式（注意选择UTF-8编码）

### 方法2: 使用文本编辑器
1. 用VS Code、Notepad++等打开CSV文件
2. 按格式添加或修改行
3. 保存文件

---

## ✅ CSV文件优势

- **简单直观** - 纯文本格式，易于理解
- **Excel兼容** - 可直接用Excel打开编辑
- **无需编译** - 修改数据后直接运行程序
- **易于备份** - 直接复制CSV文件即可
- **版本控制友好** - 适合Git等版本控制系统
- **原生支持** - C++标准库可轻松解析

---

## ⚠️ 注意事项

1. **文件编码**
   - 建议使用UTF-8编码保存
   - 避免使用Excel的默认ANSI编码（可能导致中文乱码）

2. **CSV格式规则**
   - 第一行是标题行，不要删除或修改
   - 字段用逗号分隔
   - 包含逗号的字段必须用双引号包围
   - 不要在字段中使用双引号（除非转义）

3. **数据一致性**
   - flights.csv中的aircraftType必须在aircrafts.csv中存在
   - 机场代码应使用标准的3字母IATA代码
   - 时间格式统一为"HH:MM"
   - 机组成员的资质应该是真实存在的飞机型号

4. **备份建议**
   - 修改前先备份data文件夹
   - 使用Git等版本控制工具管理数据文件

---

## 🚀 快速开始

1. **添加新飞机型号**:
   ```csv
   B777-300ER,8,32,20,56,9
   ```

2. **添加新航班**:
   ```csv
   MU587,PVG,LAX,13:00,09:00,B787-9
   ```

3. **添加新机组成员**:
   ```csv
   P011,Captain Liu Yang,PILOT,"A350-900,A350-1000"
   ```

4. **保存文件并运行程序** - 数据立即生效！

---

## 📊 常用机场IATA代码

| 代码 | 机场名称 | 城市 |
|-----|---------|------|
| PEK | 北京首都国际机场 | 北京 |
| PVG | 上海浦东国际机场 | 上海 |
| CAN | 广州白云国际机场 | 广州 |
| CTU | 成都双流国际机场 | 成都 |
| LAX | 洛杉矶国际机场 | 洛杉矶 |
| JFK | 肯尼迪国际机场 | 纽约 |
| LHR | 伦敦希思罗机场 | 伦敦 |
| NRT | 成田国际机场 | 东京 |
| SIN | 新加坡樟宜机场 | 新加坡 |
| DXB | 迪拜国际机场 | 迪拜 |

---

**最后更新**: 2025年11月7日
