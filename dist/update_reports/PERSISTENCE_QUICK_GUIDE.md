# 数据持久化快速使用指南

## 🎯 核心功能

### 现在已实现完整的数据持久化:

1. **航班机组成员** → 保存到 `flights.csv` 的 `crewIds` 字段
2. **乘客信息** → 保存到 `passengers.csv`
3. **预订记录** → 保存到 `reservations.csv`
4. **座位状态** → 通过 `reservations.csv` 自动恢复

---

## 📁 CSV文件示例

### flights.csv (新增 crewIds 列)
```csv
flightNumber,...,crewIds
CA989,...,"P001,CC001,CC002"
```
- 机组成员ID用逗号分隔,必须加引号

### passengers.csv (新建)
```csv
passportNumber,name,loyaltyStatus
G12345678,Zhang Wei,GOLD
P87654321,John Smith,PLATINUM
```

### reservations.csv (新建)
```csv
reservationNumber,passportNumber,flightNumber,seatRow,seatCol,status
RES001,G12345678,CA989,1,A,CONFIRMED
RES002,G23456789,BA168,2,C,PENDING
```
- `status`: PENDING / CONFIRMED / CANCELLED / WAITLISTED

---

## 🔄 自动保存时机

| 操作 | 保存的文件 |
|-----|----------|
| 预订机票 | passengers.csv + reservations.csv |
| 取消预订 | reservations.csv |
| 确认预订 | reservations.csv |
| 添加航班 | flights.csv |
| 分配机组 | flights.csv |

---

## ✅ 自动加载

系统启动时自动加载:
1. `aircrafts.csv` → 飞机型号
2. `crew.csv` → 机组成员
3. `flights.csv` → 航班(含机组成员关联)
4. `passengers.csv` → 乘客信息
5. `reservations.csv` → 预订记录 + **自动恢复座位状态**

座位状态恢复:
- CONFIRMED 或 PENDING 的预订 → 座位被标记为已占用
- CANCELLED 或 WAITLISTED 的预订 → 座位保持空闲

---

## 🚀 使用示例

### 1. 添加航班并分配机组
```
管理员登录
→ 管理航班
→ 添加航班
→ 分配机组成员
→ 自动保存到 flights.csv (包含 crewIds)
```

### 2. 乘客预订
```
乘客登录 (输入护照号)
→ 预订机票
→ 选择航班和座位
→ 自动保存到 passengers.csv (如果是新乘客)
→ 自动保存到 reservations.csv
→ 座位被标记为已占用
```

### 3. 查看座位图
```
系统读取 reservations.csv
→ 恢复所有预订的座位状态
→ 显示座位图时:
   已预订座位显示为 "0"
   空座位显示为舱位等级 (1/2/3/4)
```

---

## ⚠️ 重要提示

### CSV编辑规则:
1. 包含逗号的字段必须用**双引号**包围
   - ✅ `"P001,CC001,CC002"`
   - ❌ `P001,CC001,CC002`

2. 关联字段必须存在
   - 预订的 `passportNumber` 必须在 `passengers.csv` 中
   - 预订的 `flightNumber` 必须在 `flights.csv` 中
   - 航班的 `crewIds` 必须在 `crew.csv` 中

3. 状态值大小写敏感
   - ✅ `CONFIRMED`
   - ❌ `confirmed` 或 `Confirmed`

---

## 📊 数据一致性

```
passengers.csv (护照号)
       ↓
reservations.csv (关联乘客和航班)
       ↓
flights.csv (航班号)
       ↓
座位状态 (自动恢复到 Aircraft 对象)
```

**系统重启后,所有数据和座位状态完全恢复!** ✨
