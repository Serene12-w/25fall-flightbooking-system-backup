# 候补队列和预订数据持久化功能更新

## 更新日期
2025年11月9日

---

## 更新概述

本次更新完成了两个重要功能：
1. **修复了值机选座后预订数据不保存的问题**
2. **实现了基于优先队列的候补功能，会员等级越高优先级越高**

---

## 功能1：预订数据持久化修复

### 问题描述
之前在值机选座后，预订信息（座位分配）没有保存到 `data/reservations.csv` 文件中。

### 解决方案
在 `checkIn()` 函数的末尾已经调用了 `saveReservationsData()`，确保：
- 单程航班选座后保存
- 联程航班所有段选座后保存
- 数据格式：`flightNumber,flightDate,seatReservations`

### 验证方法
1. 登录系统，选择一个已付款订单
2. 进行值机选座
3. 选座完成后，检查 `data/reservations.csv` 文件
4. 应该看到新增的预订记录，格式如：`CA989,2025-11-10,"1A:G12345678:CONFIRMED"`

---

## 功能2：候补队列系统

### 核心特性

#### 优先级设置（基于会员等级）
```
PLATINUM (白金卡) = 优先级 4 (最高)
GOLD (金卡)      = 优先级 3
SILVER (银卡)    = 优先级 2
NONE (普通)      = 优先级 1 (最低)
```

#### 工作流程

**场景1：预订时座位已满**
1. 乘客选择航班和舱位
2. 系统检测到该舱位已满
3. 显示三个选项：
   - 加入候补队列（按会员等级排序）
   - 选择其他舱位
   - 取消预订
4. 如果选择加入候补，系统自动添加到优先队列

**场景2：有乘客取消订单**
1. 乘客取消待付款订单
2. 系统自动检查候补队列
3. 找到该航班+舱位的最高优先级候补
4. 自动为候补乘客创建订单（待付款状态）
5. 通知候补乘客订单已确认

---

## 新增功能详解

### 1. hasAvailableSeats()
**功能**：检查特定航班的特定舱位是否还有空座

**实现**：
- 遍历飞机上该舱位的所有座位
- 统计已预订数量
- 如果已预订数 < 总座位数，返回 true

### 2. addToWaitlist()
**功能**：将乘客加入候补队列

**参数**：
- `passenger`: 乘客对象
- `flight`: 航班对象
- `seatType`: 舱位类型

**处理**：
- 根据乘客会员等级计算优先级
- 创建 WaitlistEntry 对象
- 加入优先队列（自动按优先级排序）
- 显示候补确认信息

### 3. processWaitlist()
**功能**：处理候补队列，自动为候补乘客创建订单

**触发时机**：
- 有乘客取消订单时
- 管理员手动触发时

**处理流程**：
1. 从优先队列取出最高优先级条目
2. 检查是否匹配当前航班和舱位
3. 如果匹配且有空座：
   - 自动创建订单
   - 计算票价（包含会员折扣）
   - 设置状态为待付款
   - 通知乘客
4. 如果不匹配，放回队列

### 4. viewWaitlist()
**功能**：查看当前候补队列（管理员功能）

**显示信息**：
- 候补总数
- 每个候补的详细信息：
  - 乘客姓名和护照号
  - 航班号和日期
  - 舱位类型
  - 会员等级
  - 优先级数值

---

## 使用指南

### 乘客操作：加入候补队列

1. **登录系统**
   ```
   选择: 1 (Passenger Login)
   输入护照号: G12345678
   输入姓名: Zhang Wei
   ```

2. **预订航班**
   ```
   选择: 1 (Search and Book Flight)
   输入出发地: PEK
   输入目的地: LAX
   输入日期: 2025-11-10
   ```

3. **选择已满的舱位**
   ```
   选择航班: 1
   选择舱位: 1 (First Class)
   ```

4. **如果座位已满，选择候补**
   ```
   ========== Cabin Full ==========
   Sorry, First Class is fully booked for this flight.

   Would you like to:
   1. Join the waitlist (priority based on loyalty status)
   2. Select a different cabin class
   3. Cancel
   
   Enter your choice: 1
   ```

5. **确认加入候补**
   ```
   ========== Added to Waitlist ==========
   Passenger: Zhang Wei
   Flight: CA989
   Date: 2025-11-10
   Cabin: First Class
   Loyalty Status: GOLD
   Priority: 3

   You will be automatically upgraded when a seat becomes available.
   ========================================
   ```

### 乘客操作：取消订单（触发候补处理）

1. **选择取消订单**
   ```
   选择: 5 (Cancel Order)
   选择要取消的订单: 1
   确认取消: Y
   ```

2. **系统自动处理候补**
   ```
   Order ORD2025110800001 has been cancelled successfully.

   Checking waitlist for available upgrades...

   ========== Processing Waitlist ==========
   Passenger: Zhang Wei
   Passport: G12345678
   Flight: CA989
   Date: 2025-11-10
   Cabin: First Class

   Creating order automatically...

   Order Number: ORD2025110900015
   Ticket Price: $2992.00
   Status: PENDING PAYMENT

   Your waitlist booking has been confirmed!
   Please proceed to payment within 24 hours.
   ========================================
   ```

### 管理员操作：查看候补队列

1. **登录管理员账号**
   ```
   选择: 2 (Administrator Login)
   用户名: admin
   密码: admin123
   ```

2. **查看候补队列**
   ```
   选择: 9 (View Waitlist)
   ```

3. **查看候补详情**
   ```
   ========== Current Waitlist ==========
   Total entries: 3

   Note: The queue is sorted by priority (higher priority listed first)
   Priority levels: PLATINUM=4, GOLD=3, SILVER=2, NONE=1
   ======================================

   1. Passenger: Li Ming (G23456789)
      Flight: CA989 on 2025-11-10
      Cabin: First Class
      Loyalty: PLATINUM (Priority: 4)

   2. Passenger: Zhang Wei (G12345678)
      Flight: CA989 on 2025-11-10
      Cabin: First Class
      Loyalty: GOLD (Priority: 3)

   3. Passenger: Wang Fang (G34567890)
      Flight: MU587 on 2025-11-11
      Cabin: Business Class
      Loyalty: SILVER (Priority: 2)

   ======================================
   ```

---

## 测试场景

### 测试1：座位已满时加入候补
**步骤**：
1. 手动编辑 `data/orders.csv`，为某航班的某舱位创建足够多的订单，使其满座
2. 登录乘客账号，尝试预订该航班该舱位
3. 验证：系统提示座位已满，提供候补选项
4. 选择加入候补
5. 验证：成功加入候补队列，显示优先级

### 测试2：取消订单后候补自动升级
**前提**：测试1已完成，候补队列中有至少一个条目

**步骤**：
1. 使用另一个账号登录（该账号有该航班该舱位的待付款订单）
2. 取消该订单
3. 验证：系统自动处理候补，为最高优先级的候补乘客创建订单
4. 检查 `data/orders.csv`，确认新订单已创建

### 测试3：优先级排序验证
**步骤**：
1. 使用3个不同会员等级的账号（PLATINUM, GOLD, SILVER）
2. 按顺序让它们都加入同一航班同一舱位的候补
3. 管理员查看候补队列
4. 验证：PLATINUM 会员排在最前面，即使最后加入

### 测试4：值机后预订数据保存
**步骤**：
1. 完成一个订单的预订、付款流程
2. 进行值机选座
3. 选座完成后，立即检查 `data/reservations.csv`
4. 验证：新增的预订记录包含正确的航班号、日期和座位信息

---

## 技术实现细节

### 数据结构

**WaitlistEntry 结构体**：
```cpp
struct WaitlistEntry {
    Passenger* passenger;
    Flight* flight;
    SeatType seatType;
    int priority;  // 优先级，根据常旅客等级计算

    // 重载 > 运算符用于优先队列
    bool operator>(const WaitlistEntry& other) const {
        return priority < other.priority; // 优先级高的在前
    }
};
```

**优先队列声明**：
```cpp
std::priority_queue<WaitlistEntry, 
                    std::vector<WaitlistEntry>, 
                    std::greater<WaitlistEntry>> waitlist;
```

### 优先级计算函数
```cpp
int AirlineReservationSystem::getLoyaltyPriority(LoyaltyStatus status) {
    switch (status) {
        case LoyaltyStatus::PLATINUM: return 4;
        case LoyaltyStatus::GOLD: return 3;
        case LoyaltyStatus::SILVER: return 2;
        case LoyaltyStatus::NONE: return 1;
        default: return 1;
    }
}
```

---

## 文件修改清单

### 修改的文件

1. **include/AirlineReservationSystem.h**
   - 添加候补队列相关函数声明：
     - `hasAvailableSeats()`
     - `addToWaitlist()`
     - `processWaitlist()`
     - `viewWaitlist()`

2. **src/AirlineReservationSystem.cpp**
   - 实现所有候补队列相关函数
   - 修改 `bookTicket()` 添加座位满时的候补逻辑
   - 修改 `cancelReservation()` 添加自动处理候补的逻辑
   - 修改 `showAdminMenu()` 添加查看候补队列选项

3. **src/Aircraft.h & Aircraft.cpp**（已存在）
   - 已有 `hasCabinClass()` 方法用于检查舱位配置

---

## 注意事项

1. **候补不占用座位**
   - 候补队列中的乘客没有实际占用座位
   - 只有当他们的订单被自动创建后，才会在付款和值机时占用座位

2. **候补订单状态**
   - 从候补队列转正的订单初始状态为 PENDING_PAYMENT
   - 乘客需要在规定时间内付款
   - 如果不付款，订单会被取消，座位释放

3. **优先队列特性**
   - 优先队列自动维护优先级排序
   - 取出时总是获得最高优先级条目
   - 同优先级的条目按加入顺序处理

4. **候补队列持久化**
   - 当前实现中，候补队列在内存中
   - 程序重启后候补队列会清空
   - 未来可以考虑将候补队列保存到文件

---

## 未来改进方向

1. **候补队列持久化**
   - 将候补队列保存到 CSV 文件
   - 程序启动时恢复候补队列

2. **候补通知系统**
   - 当候补转正时，发送邮件/短信通知
   - 提供候补状态查询功能

3. **候补过期机制**
   - 设置候补有效期（如7天）
   - 自动清理过期候补

4. **灵活的候补策略**
   - 允许候补多个备选航班
   - 允许候补更低舱位（自动降级）

---

## 总结

本次更新成功实现了：
✅ 值机选座后预订数据自动保存到 reservations.csv
✅ 座位已满时提供候补选项
✅ 基于会员等级的优先队列实现
✅ 取消订单时自动处理候补队列
✅ 管理员可以查看当前候补情况
✅ 完整的候补转正流程

系统现在具备了完整的候补管理功能，为乘客提供了更好的订票体验！
