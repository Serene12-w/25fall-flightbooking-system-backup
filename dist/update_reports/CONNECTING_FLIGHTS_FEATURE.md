# 转机航班功能实现文档

## 日期: 2025-11-09

## 功能概述
本次更新为航空预订系统添加了完整的转机航班搜索和预订功能。当没有直飞航班时，系统会自动搜索可行的转机路线，并将联程票作为单个订单处理。

---

## 核心功能

### 1. 智能转机航班搜索

#### 搜索算法
- 使用 **Dijkstra算法** 搜索最优转机路线（最短距离）
- 最多支持 **2次转机**（即最多3段航班）
- 优先队列保证总是优先探索距离最短的路径
- 自动记录每个城市-时间状态的最优解，避免重复计算
- 结果自动按总距离排序，优先推荐最短路线

#### 时间关联性验证
系统会验证每个转机点的停留时间是否合理：
- **最短转机时间**: 90分钟（国际航班标准）
- **最长转机时间**: 12小时
- 超出此范围的路线会被自动过滤

#### 时间计算功能
- `timeToMinutes()`: 将时间字符串转换为分钟数
- `minutesToTime()`: 将分钟数转换为时间字符串
- `calculateLayoverMinutes()`: 计算两个航班之间的转机时间
- `isValidLayover()`: 验证转机时间是否合理

---

### 2. 联程票订单系统

#### Order类扩展
新增字段：
```cpp
std::vector<std::string> connectingFlightNumbers;  // 所有航班号
std::vector<std::string> connectingSeatNumbers;    // 对应的座位号
```

新增方法：
- `isConnectingFlight()`: 检查是否为联程航班
- `setConnectingFlights()`: 设置联程航班信息
- `getConnectingFlightNumbers()`: 获取所有航班号
- `getConnectingSeatNumbers()`: 获取所有座位号

#### 联程票特点
1. **单个订单**: 多段航班作为一个订单处理
2. **统一付款**: 一次性支付所有段的费用
3. **联程优惠**: 自动享受 10% 折扣
4. **会员折扣**: 在联程优惠基础上叠加会员折扣
5. **统一值机**: 一次性为所有段选择座位

---

### 3. 价格计算

#### 联程票价格公式
```
基础价格 = Σ(每段航班的基础价格)
联程优惠价 = 基础价格 × 0.9
最终价格 = 联程优惠价 × (1 - 会员折扣率)
```

#### 示例
假设：
- 第1段基础价: $1000
- 第2段基础价: $800
- 会员等级: GOLD (15% 折扣)

计算过程：
1. 总基础价 = $1000 + $800 = $1800
2. 联程优惠价 = $1800 × 0.9 = $1620
3. 最终价格 = $1620 × 0.85 = $1377

---

### 4. 预订流程

#### 步骤1: 搜索航班
用户输入：
- 出发地机场代码
- 目的地机场代码
- 出发日期

系统行为：
1. 首先搜索直飞航班
2. 如果有直飞，显示直飞航班并允许用户选择查看转机
3. 如果无直飞，自动搜索转机航班
4. 最多显示前5条最优路线

#### 步骤2: 选择路线
系统显示每条路线的详细信息：
- 总段数
- 总距离
- 每段航班详情（航班号、起降时间、机型）
- 每个转机点的停留时间

#### 步骤3: 选择舱位
与直飞航班相同，支持四种舱位：
- 头等舱 (First Class)
- 商务舱 (Business Class)
- 超级经济舱 (Premium Economy)
- 经济舱 (Economy Class)

#### 步骤4: 确认订单
系统创建订单，状态为 "待付款"

---

### 5. 付款流程

联程票的付款流程与直飞相同：
1. 查看待付款订单（显示联程标识和段数）
2. 输入信用卡信息
3. 确认支付
4. 状态变为 "已付款"

---

### 6. 值机流程

#### 联程票特殊处理
系统会为每一段航班单独选择座位：

1. 显示"联程航班值机"提示
2. 对每一段：
   - 显示航班信息（航班号、起降地）
   - 显示座位图
   - 选择座位
   - 验证舱位匹配
   - 预订座位
3. 保存所有座位信息
4. 状态变为 "已值机"

---

### 7. 查看订单

#### 订单显示
联程票订单会显示：
- 订单号
- "CONNECTING FLIGHT" 标识
- 航班段数
- 每段航班号和路线
- 所有座位号（如已值机）
- 总价格
- 订单状态

---

### 8. 数据持久化

#### CSV格式更新
`orders.csv` 新增两列：
- `connectingFlights`: 用分号分隔的航班号列表
- `connectingSeats`: 用分号分隔的座位号列表

#### 示例
```csv
orderNumber,passportNumber,flightNumber,flightDate,seatNumber,seatType,status,createTime,paymentTime,checkInTime,creditCardLast4,ticketPrice,connectingFlights,connectingSeats
ORD2025110900001,G12345678,CA989,2025-11-10,1A,BUSINESS_CLASS,CHECKED_IN,2025-11-09 10:00:00,2025-11-09 10:30:00,2025-11-09 11:00:00,1234,1377.00,"CA989;MU587;CZ327","5B;12C;8A"
```

---

## 技术实现

### 核心数据结构

#### ConnectingFlightRoute
```cpp
struct ConnectingFlightRoute {
    std::vector<Flight*> flights;        // 航班序列
    double totalDistance;                // 总距离
    int totalMinutes;                    // 总时间
    std::vector<int> layoverMinutes;     // 转机停留时间
    bool isValid;                        // 路线是否有效
};
```

### 关键函数

1. **findConnectingFlights()**: BFS搜索转机路线
2. **isValidLayover()**: 验证转机时间合理性
3. **displayConnectingRoute()**: 显示路线详情
4. **bookConnectingFlight()**: 预订联程票
5. **checkIn()**: 增强版值机（支持联程）

---

## 使用场景示例

### 场景1: 北京到洛杉矶（有直飞）
```
Enter origin: PEK
Enter destination: LAX
Enter date: 2025-11-10

--- Available Direct Flights ---
1. CA989 PEK -> LAX (Direct)
2. View connecting flights (Enter 0)

选择: 1
```

### 场景2: 无直飞航班
```
Enter origin: CAN
Enter destination: JFK
Enter date: 2025-11-15

No direct flights found.
Searching for connecting flights...

--- Available Connecting Flights ---
Route 1:
  Total Segments: 2
  Total Distance: 12850.5 km
  
  Segment 1: CZ327 CAN -> LAX (11:30 - 07:00+1)
  Layover at LAX: 4h 0m
  Segment 2: DL123 LAX -> JFK (11:00 - 19:30)

Route 2:
  Total Segments: 2
  Total Distance: 13200.0 km
  ...
```

---

## 注意事项

### 转机时间限制
- 系统只接受90分钟到12小时的转机时间
- 过短的转机时间会导致赶不上后续航班
- 过长的转机时间影响乘客体验

### 联程票限制
- 一旦付款，整张联程票不可退款
- 必须按顺序乘坐所有段
- 不支持部分段单独改签

### 座位选择
- 每段航班的座位舱位必须一致
- 值机时需要为每段分别选择座位
- 座位号分别保存

---

## 系统改进

### 相比之前的版本
1. ✅ 添加了时间关联性验证
2. ✅ 支持联程票作为单一订单
3. ✅ 提供联程优惠折扣
4. ✅ 完整的值机流程
5. ✅ CSV数据持久化
6. ✅ 友好的用户界面

### 未来可扩展功能
- 考虑航班日期衔接（跨天航班）
- 支持更复杂的转机规则
- 添加转机机场设施信息
- **✅ 已完成: 使用Dijkstra算法优化路线推荐**
- 支持混舱联程（不同段不同舱位）

---

## 算法实现

### Dijkstra最短路径算法
系统使用**Dijkstra算法**搜索最优转机路线，保证找到距离最短的路径：

```cpp
// 优先队列：<总距离, <当前路径, 已访问城市>>
std::priority_queue<DijkstraNode, 
                    std::vector<DijkstraNode>, 
                    std::greater<DijkstraNode>> pq;

// 状态记录避免重复计算
std::map<std::pair<std::string, std::string>, double> bestDistance;
```

**算法优势:**
- ✅ 保证找到最短距离路径（数学证明）
- ✅ 优先队列自动按距离排序
- ✅ 状态剪枝提高效率
- ✅ 时间复杂度: O((V+E)logV)

**vs BFS对比:**
- BFS只能找最少转机次数
- Dijkstra找最短距离（更符合实际需求）
- Dijkstra结果质量更优

---

## 代码文件修改清单

### 头文件
- `include/AirlineReservationSystem.h`: 添加转机搜索相关函数声明
- `include/Order.h`: 扩展支持联程航班

### 源文件
- `src/AirlineReservationSystem.cpp`: 实现所有转机功能
- `src/Order.cpp`: 实现联程航班方法
- `src/OrderManager.cpp`: 更新CSV读写以支持联程数据

### 数据文件
- `data/orders.csv`: 添加两列联程字段

---

## 测试建议

### 测试用例1: 直飞航班预订
- 搜索有直飞的路线
- 选择直飞航班
- 完成整个预订流程

### 测试用例2: 转机航班预订
- 搜索无直飞的路线
- 查看转机选项
- 预订联程票
- 为每段值机

### 测试用例3: 订单管理
- 查看联程票订单
- 取消未付款的联程票
- 为已付款的联程票值机

### 测试用例4: 边界情况
- 转机时间刚好90分钟
- 转机时间接近12小时
- 3段航班（2次转机）
- 环路检测

---

## 总结

本次更新成功实现了完整的转机航班功能，使用图算法进行路径搜索，考虑了时间关联性，并将联程票作为统一的订单进行管理。系统提供了良好的用户体验，同时保持了代码的可维护性和可扩展性。
