# 航空订票系统GUI功能详细说明

## 系统架构概述

本系统采用分层架构设计：

```
┌─────────────────────────────────────────┐
│         用户界面层 (UI Layer)            │
│  ┌──────────────┐    ┌──────────────┐  │
│  │ PassengerUI  │    │  Console UI  │  │
│  │   (GUI)      │    │  (Admin)     │  │
│  └──────────────┘    └──────────────┘  │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│        业务逻辑层 (Business Layer)       │
│     AirlineReservationSystem API        │
│  • loginPassenger()                     │
│  • registerPassenger()                  │
│  • searchFlights()                      │
│  • createBooking()                      │
│  • payOrder()                           │
│  • checkInOrder()                       │
│  • cancelOrder()                        │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│        数据访问层 (Data Layer)           │
│  ┌──────────┐  ┌──────────┐            │
│  │ Flight   │  │  Order   │  ......    │
│  │ Manager  │  │ Manager  │            │
│  └──────────┘  └──────────┘            │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│        数据持久层 (Persistence)          │
│          CSV File Storage               │
└─────────────────────────────────────────┘
```

## GUI界面流程

### 1. 程序启动
```
airline_reservation.exe
         │
         ▼
  [控制台选择界面]
         │
    ┌────┴────┐
    │         │
    ▼         ▼
 [GUI]    [Console]
(乘客)     (管理员)
```

### 2. 乘客GUI工作流

```
登录/注册界面
    │
    ├─→ 输入护照号 → 登录 → 成功 → 主菜单
    │                      └→ 失败 → 提示注册
    │
    └─→ 输入姓名+护照 → 注册 → 成功 → 主菜单
                              └→ 失败 → 提示已存在
```

### 3. 主菜单功能流程

#### 搜索航班流程
```
主菜单
  │
  └─→ Search Flights
        │
        ├─→ 输入出发地
        ├─→ 输入目的地
        ├─→ 输入日期
        │
        └─→ 点击Search
              │
              ├─→ 找到航班 → 显示列表
              │              │
              │              └─→ 选择航班 → Book
              │                              │
              │                              └─→ 创建订单
              │
              └─→ 未找到 → 提示无航班
```

#### 预订流程
```
Search结果
    │
    └─→ 选择航班 → Book Selected Flight
                      │
                      ├─→ 创建订单 (PENDING_PAYMENT)
                      │
                      └─→ 显示订单号和价格
```

#### 支付流程
```
主菜单 → Pay for Ticket
           │
           ├─→ 输入订单号
           ├─→ 输入信用卡号
           │
           └─→ Pay Now
                 │
                 ├─→ 验证订单状态
                 ├─→ 处理支付 (PAID)
                 │
                 └─→ 成功/失败提示
```

#### 值机流程
```
主菜单 → Check In
           │
           ├─→ 输入订单号
           ├─→ 输入座位号
           │
           └─→ Check In Now
                 │
                 ├─→ 验证订单(已付款)
                 ├─→ 验证座位可用
                 ├─→ 预订座位
                 ├─→ 更新状态 (CHECKED_IN)
                 │
                 └─→ 成功/失败提示
```

## 订单状态机

```
         [PENDING_PAYMENT]
               │
               │ payOrder()
               ▼
            [PAID]
               │
               │ checkInOrder()
               ▼
          [CHECKED_IN]

任意状态 ──cancelOrder()──→ [CANCELLED]
```

## 数据模型

### Order 订单模型
```cpp
Order {
    orderNumber: string        // 订单号 (ORD2025110800001)
    passportNumber: string     // 乘客护照号
    flightNumber: string       // 航班号
    flightDate: string        // 航班日期
    seatNumber: string        // 座位号
    seatType: SeatType        // 舱位等级
    status: OrderStatus       // 订单状态
    ticketPrice: double       // 票价
    createTime: string        // 创建时间
    paymentTime: string       // 支付时间
    checkInTime: string       // 值机时间
    creditCardLast4: string   // 信用卡后4位
}
```

### Passenger 乘客模型
```cpp
Passenger {
    name: string              // 姓名
    passportNumber: string    // 护照号
    loyaltyStatus: enum       // 会员等级
    // NONE, SILVER, GOLD, PLATINUM
}
```

### Flight 航班模型
```cpp
Flight {
    flightNumber: string      // 航班号
    origin: string           // 出发地
    destination: string      // 目的地
    departureTime: string    // 出发时间
    arrivalTime: string      // 到达时间
    flightDate: string       // 航班日期
    aircraft: Aircraft*      // 飞机对象
    crew: vector<Crew*>      // 机组成员
}
```

## 核心API说明

### 1. loginPassenger()
```cpp
bool loginPassenger(const string& passportNumber)
```
- **功能：** 乘客登录
- **参数：** 护照号
- **返回：** 成功返回true，失败返回false
- **副作用：** 设置currentPassenger

### 2. registerPassenger()
```cpp
bool registerPassenger(const string& name, const string& passportNumber)
```
- **功能：** 注册新乘客
- **参数：** 姓名、护照号
- **返回：** 成功返回true，已存在返回false
- **副作用：** 创建新乘客，保存到CSV

### 3. searchFlights()
```cpp
vector<Flight*> searchFlights(const string& origin, 
                              const string& destination, 
                              const string& date)
```
- **功能：** 搜索航班
- **参数：** 出发地、目的地、日期
- **返回：** 匹配的航班列表

### 4. createBooking()
```cpp
Order* createBooking(const string& flightNumber, 
                    const string& flightDate, 
                    SeatType seatType)
```
- **功能：** 创建订单
- **参数：** 航班号、日期、舱位
- **返回：** 订单对象指针
- **副作用：** 生成订单号，计算价格，保存订单

### 5. payOrder()
```cpp
bool payOrder(const string& orderNumber, 
             const string& creditCard)
```
- **功能：** 支付订单
- **参数：** 订单号、信用卡号
- **返回：** 成功返回true
- **副作用：** 更新订单状态为PAID

### 6. checkInOrder()
```cpp
bool checkInOrder(const string& orderNumber, 
                 const string& seatNumber)
```
- **功能：** 办理值机
- **参数：** 订单号、座位号
- **返回：** 成功返回true
- **副作用：** 预订座位，更新订单状态为CHECKED_IN

### 7. cancelOrder()
```cpp
bool cancelOrder(const string& orderNumber)
```
- **功能：** 取消订单
- **参数：** 订单号
- **返回：** 成功返回true
- **副作用：** 释放座位（如已值机），更新状态为CANCELLED

## GUI控件说明

### 窗口控件ID
```cpp
// 按钮ID
ID_BTN_LOGIN = 101
ID_BTN_REGISTER = 102
ID_BTN_SEARCH = 103
ID_BTN_BOOK = 104
ID_BTN_PAY = 105
ID_BTN_CHECKIN = 106
ID_BTN_VIEW_ORDERS = 107
ID_BTN_CANCEL = 108
ID_BTN_PROFILE = 109

// 编辑框ID
ID_EDIT_PASSPORT = 201
ID_EDIT_NAME = 202
ID_EDIT_ORIGIN = 203
ID_EDIT_DEST = 204
ID_EDIT_DATE = 205

// 列表ID
ID_LIST_FLIGHTS = 301
ID_LIST_ORDERS = 302
```

### 窗口消息处理
```cpp
WM_COMMAND: 处理按钮点击
WM_DESTROY: 处理窗口关闭
```

## 错误处理

### 登录错误
- 护照号为空 → "Please enter your passport number!"
- 乘客不存在 → "Passenger not found. Please register first."

### 注册错误
- 信息不完整 → "Please enter both passport number and name!"
- 护照号已存在 → "This passport number is already registered."

### 预订错误
- 未登录 → "Please login first!"
- 未选择航班 → "Please select a flight to book!"
- 预订失败 → "Booking failed. Please try again."

### 支付错误
- 信息不完整 → "Please fill in all fields!"
- 订单不存在 → "Payment failed. Please check order number."
- 状态不正确 → "Order already paid or cancelled."

### 值机错误
- 信息不完整 → "Please fill in all fields!"
- 订单未支付 → "Please pay for the order first."
- 座位不可用 → "Seat not available or already booked."

## 测试用例

### 测试1：完整流程
1. 注册新用户 → 成功
2. 搜索PEK到LAX的航班 → 找到CA989
3. 预订CA989 → 生成订单ORD2025111100001
4. 支付订单 → 成功
5. 值机选座1A → 成功
6. 查看订单 → 显示CHECKED_IN状态

### 测试2：错误处理
1. 登录不存在的用户 → 提示不存在
2. 重复注册 → 提示已存在
3. 预订不存在的航班 → 失败
4. 支付未预订的订单 → 失败
5. 值机未支付的订单 → 失败
6. 选择已被预订的座位 → 失败

### 测试3：边界情况
1. 空输入 → 提示错误
2. 无效日期格式 → 搜索无结果
3. 无效座位号 → 值机失败
4. 已取消的订单操作 → 失败

## 性能考虑

- 航班搜索：O(n) 线性搜索
- 订单查询：O(1) 哈希表查询
- 乘客查找：O(log n) 二叉搜索树

## 未来改进

1. **UI改进**
   - 添加座位图可视化
   - 改进布局和配色
   - 添加图标和图片

2. **功能增强**
   - 支持多段转机搜索
   - 实时座位图更新
   - 添加退票功能
   - 积分累积和兑换

3. **技术优化**
   - 异步加载数据
   - 缓存搜索结果
   - 多线程支持
   - 数据库替代CSV

## 常见问题

**Q: 为什么选择Win32 API而不是Qt或其他GUI框架？**
A: Win32 API是Windows原生API，无需额外依赖，编译后是单个exe文件，部署简单。

**Q: 数据如何持久化？**
A: 所有操作会自动保存到data文件夹的CSV文件中。

**Q: 可以同时运行多个实例吗？**
A: 可以，但可能会有数据冲突问题，建议单实例运行。

**Q: 如何添加新的飞机型号？**
A: 使用管理员系统，选择"管理飞机型号"功能。

**Q: 订单号格式是什么？**
A: ORD + YYYYMMDD + 5位序号，例如：ORD2025111100001
