# 座位隔离修复 - 快速参考

## 问题
所有使用相同飞机型号的航班共享座位状态,导致预订相互干扰。

## 解决方案
每个航班现在拥有独立的Aircraft实例和座位状态。

---

## 主要改变

### 1. 每个航班独立座位
```
Before: 所有B787-9航班 → 共享一个座位图
After:  每个航班 → 自己的座位图副本
```

### 2. reservations.csv新增日期字段
```csv
# 旧格式
flightNumber,seatReservations
CA989,"1A:G12345678:CONFIRMED"

# 新格式
flightNumber,flightDate,seatReservations
CA989,2025-11-10,"1A:G12345678:CONFIRMED"
```

---

## 数据迁移

### 更新现有reservations.csv

**步骤**:
1. 打开 `data/reservations.csv`
2. 修改第一行: 添加 `,flightDate` 在 `flightNumber` 后
3. 为每个预订行添加对应日期(查看flights.csv)

**示例**:
```csv
# 从 flights.csv 找到航班日期
CA989 → 2025-11-10
MU587 → 2025-11-11

# 更新 reservations.csv
CA989,2025-11-10,"1A:G12345678:CONFIRMED"
MU587,2025-11-11,"2B:P87654321:CONFIRMED"
```

---

## 编译

```powershell
g++ -g src/*.cpp -I include -o airline_reservation.exe -static-libgcc -static-libstdc++ -static
```

---

## 验证

### 测试1: 不同航班隔离
1. 登录并预订 AC889 (B787-9) 的座位 1A
2. 预订 UA117 (B787-9) 的另一个座位
3. 检查: UA117 的座位图上 1A 应该显示为可用 ✓

### 测试2: 联程航班
1. 预订联程航班(例如 YYZ → SFO → ICN)
2. 第一段选择 1I
3. 第二段选择 1I
4. 检查: 两段都应该能选择1I(如果都有该座位) ✓

---

## 技术细节

### Aircraft 拷贝构造函数
- 创建完整的座位副本
- 不复制预订状态(全新航班)

### Flight 构造函数
- 自动创建 Aircraft 副本
- 析构时自动释放

### DataLoader
- 使用"航班号:日期"作为唯一标识
- 支持同一航班号不同日期

---

## 常见问题

**Q: 为什么要为每个航班创建Aircraft副本?**
A: 因为每个航班实例需要独立的座位预订状态。

**Q: 会不会占用太多内存?**
A: 每个航班约增加40-50KB,对于现代系统可以忽略不计。

**Q: 旧的reservations.csv还能用吗?**
A: 需要手动添加日期列,否则系统无法加载。

**Q: 如何知道航班的日期?**
A: 查看 `data/flights.csv` 的 `flightDate` 列。

---

## 文件清单

修改的文件:
- ✅ `include/Aircraft.h`
- ✅ `src/Aircraft.cpp`
- ✅ `include/Flight.h`
- ✅ `src/Flight.cpp`
- ✅ `src/DataLoader.cpp`
- ✅ `data/reservations.csv`

新增文档:
- 📄 `update_reports/FLIGHT_SEAT_ISOLATION_FIX.md` (详细文档)
- 📄 `update_reports/FLIGHT_SEAT_ISOLATION_QUICK_GUIDE.md` (本文件)
