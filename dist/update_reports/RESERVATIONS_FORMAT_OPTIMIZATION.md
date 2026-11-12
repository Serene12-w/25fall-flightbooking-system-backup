# Reservations.csv 格式优化说明

## 🎯 优化目标

将预订数据从**每个预订一行**改为**每个航班一行**,提高查询效率和数据组织性。

---

## 📊 格式对比

### 旧格式 (每个预订一行)
```csv
reservationNumber,passportNumber,flightNumber,seatRow,seatCol,status
RES001,G12345678,CA989,1,A,CONFIRMED
RES002,G12345678,CA989,1,B,CONFIRMED
RES003,P87654321,CA989,2,A,CONFIRMED
```

**问题**:
- ❌ 查找某航班的所有预订需要遍历整个文件
- ❌ 数据分散,不利于快速读取座位图
- ❌ 文件行数随预订增加快速增长

---

### 新格式 (每个航班一行) ⭐

```csv
flightNumber,seatReservations
CA989,"1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED,2A:P87654321:CONFIRMED"
BA168,"1C:G23456789:CONFIRMED"
```

**座位预订格式**: `行列:护照号:状态`
- 例如: `1A:G12345678:CONFIRMED` 表示座位1A被护照号G12345678的乘客预订,状态为已确认

**优势**:
- ✅ 按航班组织,查找特定航班的预订只需读取一行
- ✅ 座位信息集中,便于快速生成座位图
- ✅ 文件行数等于航班数,大大减少文件大小
- ✅ 更符合业务逻辑(航班简报、座位图都是按航班查询)

---

## 🔧 实现细节

### DataLoader::loadReservations() 

**解析流程**:
1. 读取每一行 → 获取 `flightNumber` 和 `seatReservations`
2. 按逗号分割 `seatReservations` → 得到每个座位信息
3. 按冒号分割座位信息 → 得到 `座位位置:护照号:状态`
4. 解析座位位置 (如 "1A") → 行号=1, 列号='A'
5. 创建 Reservation 对象并调用 `bookSeat()` 恢复座位状态

**关键代码**:
```cpp
// 解析单个座位信息: "1A:G12345678:CONFIRMED"
std::string seatPos = parts[0];      // "1A"
std::string passportNumber = parts[1]; // "G12345678"
std::string statusStr = parts[2];     // "CONFIRMED"

// 提取行号和列号
int seatRow = 0;
char seatCol = 0;
size_t i = 0;
while (i < seatPos.length() && std::isdigit(seatPos[i])) {
    seatRow = seatRow * 10 + (seatPos[i] - '0');
    i++;
}
if (i < seatPos.length()) {
    seatCol = seatPos[i];
}
```

---

### DataLoader::saveReservations()

**保存流程**:
1. 遍历所有预订 → 按 `flightNumber` 分组
2. 对每个航班,遍历其预订列表
3. 格式化每个预订为 `行列:护照号:状态`
4. 用逗号连接同一航班的所有预订
5. 写入CSV,用双引号包围座位预订列表

**关键代码**:
```cpp
// 按航班分组预订
std::map<std::string, std::vector<Reservation*>> flightReservations;
for (const auto& pair : reservations) {
    Reservation* reservation = pair.second;
    std::string flightNumber = reservation->getFlight()->getFlightNumber();
    flightReservations[flightNumber].push_back(reservation);
}

// 格式化输出: "行列:护照号:状态"
file << reservation->getSeat()->getRow()
     << reservation->getSeat()->getCol()
     << ":"
     << reservation->getPassenger()->getPassportNumber()
     << ":"
     << statusStr;
```

---

## 📈 性能提升

### 查询某航班座位图

**旧方式**:
```
遍历 reservations.csv (假设1000条记录)
→ 过滤出特定航班的预订 (约10条)
→ 标记座位为已占用
时间复杂度: O(n) n=总预订数
```

**新方式**:
```
直接读取航班对应的行 (1行)
→ 解析该行的座位预订列表
→ 标记座位为已占用
时间复杂度: O(1) 直接定位
```

**效率提升**: 从 O(n) 降低到 O(1) 🚀

---

### 查看航班简报

**场景**: 管理员查看CA989航班的乘客名单和座位分布

**旧方式**:
- 遍历所有预订记录
- 筛选出CA989的预订
- 汇总统计

**新方式**:
- 读取CA989这一行
- 直接获取所有座位预订信息
- 立即显示结果

---

## 🎯 使用示例

### 示例1: 快速查看某航班座位图

```cpp
// 读取 reservations.csv
// 只需找到 CA989 这一行
// 解析: "1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED,2A:P87654321:CONFIRMED"
// 结果: 座位 1A, 1B, 2A 已被预订

显示座位图:
   A B C D E F G H I
1  0 0 3 3 3 3 3 3 3  ← 1A, 1B 已占用
2  0 3 3 3 3 3 3 3 3  ← 2A 已占用
3  3 3 3 3 3 3 3 3 3
...
```

### 示例2: 航班简报生成

```
CA989 航班简报
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
航班号: CA989
出发地: PEK  →  目的地: LAX
机组成员: P001, CC001, CC002, CC007

乘客名单 (3名):
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
座位  护照号        姓名          状态
1A    G12345678    Zhang Wei     ✓ 已确认
1B    G12345678    Zhang Wei     ✓ 已确认
2A    P87654321    John Smith    ✓ 已确认
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

只需读取 reservations.csv 中CA989的一行!

---

## ⚠️ 注意事项

### CSV编辑规则

1. **座位预订列表必须用双引号包围**
   ```csv
   ✅ CA989,"1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED"
   ❌ CA989,1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED
   ```

2. **座位格式**: `行列:护照号:状态`
   - 行号: 数字 (如 1, 10, 25)
   - 列号: 大写字母 (A-J)
   - 护照号: 必须在 passengers.csv 中存在
   - 状态: PENDING / CONFIRMED / CANCELLED / WAITLISTED

3. **多个座位用逗号分隔**
   ```csv
   "1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED,2A:P87654321:CONFIRMED"
   ```

---

## 🎉 总结

通过将预订数据按航班组织,实现了:

| 方面 | 改进 |
|-----|------|
| **查询效率** | O(n) → O(1) |
| **文件大小** | 预订数行 → 航班数行 |
| **业务逻辑** | 更符合航班管理需求 |
| **座位图生成** | 直接读取,无需遍历 |
| **可读性** | 按航班分组,更清晰 |

**新格式更高效、更简洁、更符合实际业务场景!** ✨
