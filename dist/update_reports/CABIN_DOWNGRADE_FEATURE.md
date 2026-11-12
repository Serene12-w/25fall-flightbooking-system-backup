# 联程航班舱位自动降级功能实现文档

## 日期: 2025-11-09

## 功能概述
本次更新为联程航班预订系统添加了智能舱位降级功能。当用户预订联程航班时，如果某段航班的飞机没有用户选择的舱位，系统会自动降级到该飞机上最接近的可用舱位，并在预订前让用户确认。

---

## 问题背景

在联程航班预订中，可能会遇到以下情况：
- 用户选择头等舱，但某段航班使用的是窄体客机（如A320），没有头等舱
- 用户选择超级经济舱，但某段航班使用老旧机型，只有商务舱和经济舱
- 不同机型的舱位配置不同，导致联程航班无法统一舱位

**解决方案**：
系统在预订时自动检测每段航班的舱位可用性，对于没有对应舱位的航段，自动降级到最接近的可用舱位，并明确告知用户，让用户确认后再完成预订。

---

## 核心功能

### 1. 飞机舱位检测功能

#### 新增方法：`Aircraft::hasCabinClass(SeatType)`

**文件**：`include/Aircraft.h`, `src/Aircraft.cpp`

**功能**：检查飞机是否配置了特定舱位

**实现**：
```cpp
bool Aircraft::hasCabinClass(SeatType seatType) const {
    switch (seatType) {
        case SeatType::FIRST_CLASS:
            return firstClassRows > 0;
        case SeatType::BUSINESS_CLASS:
            return businessClassRows > 0;
        case SeatType::PREMIUM_ECONOMY:
            return premiumEconomyRows > 0;
        case SeatType::ECONOMY_CLASS:
            return economyClassRows > 0;
        default:
            return false;
    }
}
```

**示例**：
```cpp
Aircraft* aircraft = flight->getAircraft();
if (!aircraft->hasCabinClass(SeatType::FIRST_CLASS)) {
    // 该飞机没有头等舱
}
```

---

### 2. 舱位降级算法

#### 新增方法：`AirlineReservationSystem::getDowngradedCabinClass()`

**文件**：`include/AirlineReservationSystem.h`, `src/AirlineReservationSystem.cpp`

**降级优先级**：
```
头等舱 (FIRST_CLASS)
   ↓ 如不可用
商务舱 (BUSINESS_CLASS)
   ↓ 如不可用
超级经济舱 (PREMIUM_ECONOMY)
   ↓ 如不可用
经济舱 (ECONOMY_CLASS)
```

**降级规则**：
1. **从头等舱降级**：商务舱 → 超级经济舱 → 经济舱
2. **从商务舱降级**：超级经济舱 → 经济舱
3. **从超级经济舱降级**：经济舱
4. **从经济舱降级**：无法降级（经济舱是最低级）

**实现逻辑**：
```cpp
SeatType getDowngradedCabinClass(SeatType originalClass, Aircraft* aircraft) {
    switch (originalClass) {
        case SeatType::FIRST_CLASS:
            if (aircraft->hasCabinClass(SeatType::BUSINESS_CLASS))
                return SeatType::BUSINESS_CLASS;
            if (aircraft->hasCabinClass(SeatType::PREMIUM_ECONOMY))
                return SeatType::PREMIUM_ECONOMY;
            if (aircraft->hasCabinClass(SeatType::ECONOMY_CLASS))
                return SeatType::ECONOMY_CLASS;
            break;
        // ... 其他舱位类似
    }
    return originalClass; // 无法降级时返回原舱位
}
```

---

### 3. 预订流程增强

#### 修改函数：`bookConnectingFlight()`

**文件**：`src/AirlineReservationSystem.cpp`

**新增预订流程**：

1. **舱位检测阶段**
   ```cpp
   // 检查每段航班的舱位是否匹配
   for (size_t i = 0; i < route.flights.size(); ++i) {
       Flight* flight = route.flights[i];
       Aircraft* aircraft = flight->getAircraft();
       
       if (!aircraft->hasCabinClass(seatType)) {
           // 需要降级，记录调整信息
           SeatType adjustedClass = getDowngradedCabinClass(seatType, aircraft);
           adjustments.push_back({i, flight, seatType, adjustedClass});
       }
   }
   ```

2. **用户确认阶段**
   ```cpp
   if (!adjustments.empty()) {
       // 显示所有需要降级的航段
       for (const auto& adj : adjustments) {
           显示航段号、航班号、飞机型号、原舱位、调整后舱位
       }
       
       // 让用户确认
       cout << "Do you want to proceed with this booking? (Y/N): ";
       cin >> confirm;
       
       if (confirm != 'Y' && confirm != 'y') {
           cout << "Booking cancelled." << endl;
           return;
       }
   }
   ```

3. **价格计算阶段**
   ```cpp
   // 根据实际舱位计算价格
   for (size_t i = 0; i < route.flights.size(); ++i) {
       SeatType actualSeatType = seatType;
       
       // 检查是否需要降级
       for (const auto& adj : adjustments) {
           if (adj.segmentIndex == i) {
               actualSeatType = adj.adjustedClass;
               break;
           }
       }
       
       totalPrice += calculateBasePrice(flight, actualSeatType);
   }
   ```

---

### 4. 值机流程增强

#### 修改函数：`checkIn()` - 联程航班部分

**文件**：`src/AirlineReservationSystem.cpp`

**新增值机流程**：

1. **动态舱位检测**
   ```cpp
   for (size_t i = 0; i < flightNumbers.size(); ++i) {
       Flight* flight = findFlight(flightNumbers[i]);
       Aircraft* aircraft = flight->getAircraft();
       SeatType requestedClass = selectedOrder->getSeatType();
       SeatType actualClass = requestedClass;
       
       if (!aircraft->hasCabinClass(requestedClass)) {
           // 需要降级
           actualClass = getDowngradedCabinClass(requestedClass, aircraft);
           cout << "Note: This aircraft does not have " 
                << seatTypeToString(requestedClass) << ".\n";
           cout << "      Your seat will be in " 
                << seatTypeToString(actualClass) << " instead.\n";
       }
   }
   ```

2. **座位验证调整**
   ```cpp
   // 验证座位类型 - 使用实际舱位而非订单舱位
   if (seat->getType() != actualClass) {
       cout << "Error: This seat does not match your assigned cabin class." << endl;
       cout << "Your assigned cabin: " << seatTypeToString(actualClass) << endl;
       return;
   }
   ```

---

## 用户体验流程

### 场景示例：预订联程航班

**用户操作**：
1. 登录系统
2. 搜索从 CAN（广州）到 JFK（纽约）的航班
3. 选择联程路线：CAN → PEK → JFK
4. 选择头等舱

**系统响应**：

```
========== CABIN CLASS ADJUSTMENT NOTICE ==========
Some flight segments do not offer First Class.
The following adjustments will be applied:

Segment 2: Flight CA981 (PEK -> JFK)
  Aircraft: B737 MAX 8
  Original: First Class
  Adjusted to: Business Class

Note: You will be charged for First Class for segments that offer it,
      and adjusted pricing for segments with cabin downgrades.

Do you want to proceed with this booking? (Y/N):
```

**如果用户确认 (Y)**：
```
========== Connecting Flight Booking Successful! ==========
Order Number: ORD2025110900123
Type: CONNECTING FLIGHT (2 segments)
Route: CAN -> JFK

Flight Segments:
  1. CZ327 (CAN -> PEK) - 2025-11-12
  2. CA981 (PEK -> JFK) - 2025-11-12 [Business Class]

Primary Cabin Class: First Class (with adjustments as noted above)

--- Ticket Price ---
Base Price (all segments): $4500.00
Connecting Flight Discount: 10% OFF
Price after discount: $4050.00
Member Discount: 15% OFF
Additional discount: $607.50
Final Price: $3442.50

Status: PENDING PAYMENT
```

### 场景示例：值机选座

**用户操作**：
1. 选择已付款的联程航班订单
2. 进行值机

**系统响应**（对于需要降级的航段）：

```
--- Segment 2: Flight CA981 (PEK -> JFK) ---
Note: This aircraft (B737 MAX 8) does not have First Class.
      Your seat will be in Business Class instead.
Cabin class: Business Class

[显示座位图]

Enter seat row number: 3
Enter seat column (A-J): A
```

---

## 价格计算规则

### 基本原则
- 用户按**实际乘坐的舱位**付费
- 如果某段降级，该段按降级后的舱位价格计算
- 仍然享受联程票10%优惠
- 会员折扣照常叠加

### 价格计算公式

```
每段价格 = calculateBasePrice(航班, 实际舱位)
总基础价格 = Σ(每段价格)
联程优惠价 = 总基础价格 × 0.9
最终价格 = 联程优惠价 × (1 - 会员折扣率)
```

### 价格对比示例

**场景**：CAN → PEK → JFK，用户选择头等舱，但 PEK → JFK 降级为商务舱

| 航段 | 原舱位 | 实际舱位 | 基础价格 |
|------|--------|----------|----------|
| CAN → PEK | 头等舱 | 头等舱 | $2000 |
| PEK → JFK | 头等舱 | **商务舱** | $2500 ↓ (原$3500) |
| **总计** | | | **$4500** |

```
联程优惠后：$4500 × 0.9 = $4050
会员折扣(15%)：$4050 × 0.85 = $3442.50
```

**相比全头等舱节省**：$(2000 + 3500) × 0.9 × 0.85 = $4207.50
**实际节省**：$4207.50 - $3442.50 = **$765**

---

## 技术优势

### 1. 灵活性
- 自动适应不同机型配置
- 支持任意舱位组合的联程航班
- 无需手动处理舱位不匹配问题

### 2. 用户友好
- 明确显示哪些航段需要降级
- 显示降级原因（飞机型号、舱位配置）
- 价格透明，用户清楚了解如何计费

### 3. 公平定价
- 按实际舱位计费，不会多收费
- 保持联程优惠和会员折扣
- 自动调整价格，无需人工干预

### 4. 扩展性
- 降级算法独立封装，易于修改规则
- 可轻松添加其他降级策略（如基于价格、基于座位数等）
- 支持未来添加升级功能

---

## 代码变更摘要

### 新增文件
无

### 修改文件

#### 1. `include/Aircraft.h`
- **新增方法**：`bool hasCabinClass(SeatType seatType) const;`

#### 2. `src/Aircraft.cpp`
- **新增实现**：`hasCabinClass()` 方法

#### 3. `include/AirlineReservationSystem.h`
- **新增方法**：
  - `SeatType getDowngradedCabinClass(SeatType originalClass, Aircraft* aircraft);`
  - `std::string seatTypeToString(SeatType seatType);`

#### 4. `src/AirlineReservationSystem.cpp`
- **修改函数**：`bookConnectingFlight()` - 添加舱位检测和用户确认流程
- **修改函数**：`checkIn()` - 联程航班部分，添加动态舱位检测
- **新增实现**：
  - `getDowngradedCabinClass()` - 舱位降级算法
  - `seatTypeToString()` - 舱位类型转字符串

---

## 测试场景

### 测试用例1：全部舱位匹配
**航线**：PEK → LAX (直飞)
**飞机**：B787-9（有头等舱）
**用户选择**：头等舱
**预期结果**：正常预订，无降级提示

### 测试用例2：部分舱位不匹配
**航线**：CAN → PEK → JFK (联程)
- 航段1：CZ327 (B777-300ER - 有头等舱)
- 航段2：CA981 (B737 MAX 8 - 无头等舱)
**用户选择**：头等舱
**预期结果**：
- 航段2降级为商务舱
- 显示降级通知
- 要求用户确认
- 价格按实际舱位计算

### 测试用例3：多段降级
**航线**：HKG → SHA → CAN → CTU (联程)
- 航段1：A350-900（有头等舱）
- 航段2：A320neo（无头等舱，有商务舱）
- 航段3：B737-800（无头等舱，无商务舱，有超级经济舱）
**用户选择**：头等舱
**预期结果**：
- 航段2降级为商务舱
- 航段3降级为超级经济舱
- 显示所有降级信息
- 价格反映所有降级

### 测试用例4：用户拒绝降级
**航线**：同测试用例2
**用户操作**：看到降级提示后选择 N
**预期结果**：取消预订，不创建订单

---

## 未来改进方向

### 1. 价格补偿策略
- 对降级航段提供额外折扣
- 例如：降级一档额外5%折扣，降级两档10%折扣

### 2. 舱位升级选项
- 如果高级舱位有空座，提示用户付费升级
- 会员积分兑换升级

### 3. 智能路线推荐
- 在搜索联程航班时，优先推荐无需降级的路线
- 标记需要降级的路线

### 4. 舱位匹配评分
- 给每条联程路线计算"舱位匹配度"
- 帮助用户选择最符合期望的路线

### 5. 降级历史记录
- 记录用户接受降级的情况
- 积累数据用于优化航班调度

---

## 总结

本次更新完美解决了联程航班舱位不匹配的问题：

✅ **自动检测**：系统自动识别舱位不匹配
✅ **智能降级**：按优先级自动降级到最接近的可用舱位
✅ **用户确认**：明确告知用户并要求确认
✅ **公平定价**：按实际舱位收费，不会多收
✅ **流程完整**：从预订到值机全程支持降级处理

用户体验显著提升，系统更加灵活可靠！
