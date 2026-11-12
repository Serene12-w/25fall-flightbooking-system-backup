# 航空订票系统 GUI 更新说明

## 更新内容

本次更新为航空订票系统添加了图形用户界面(GUI)，使乘客操作更加直观便捷。

## 系统架构

系统现在支持两种运行模式：

1. **乘客系统（GUI模式）** - 使用Windows图形界面，操作更加友好
2. **管理员系统（控制台模式）** - 保留原有的命令行交互方式

## 启动方式

1. 运行 `airline_reservation.exe`
2. 在控制台中选择模式：
   - 输入 `1` - 启动乘客GUI系统
   - 输入 `2` - 启动管理员控制台系统
   - 输入 `0` - 退出程序

## 乘客GUI系统功能

### 1. 登录/注册界面
- 输入护照号登录已有账户
- 输入姓名和护照号注册新账户

### 2. 主菜单
登录成功后可以访问以下功能：
- **Search Flights** - 搜索航班
- **Pay for Ticket** - 支付机票
- **Check In** - 办理值机
- **View My Orders** - 查看我的订单
- **My Profile** - 查看个人信息

### 3. 搜索航班
- 输入出发地（如 PEK）
- 输入目的地（如 LAX）
- 输入日期（格式：YYYY-MM-DD）
- 点击搜索查看可用航班
- 选择航班后点击 "Book Selected Flight" 预订

### 4. 支付机票
- 输入订单号
- 输入信用卡后4位
- 点击 "Pay Now" 完成支付

### 5. 办理值机
- 输入订单号
- 输入座位号（如 12A）
- 点击 "Check In Now" 完成值机

### 6. 查看订单
- 显示所有订单的详细信息
- 包括：订单号、航班号、日期、座位、价格、状态

### 7. 查看个人信息
- 显示姓名、护照号
- 显示会员等级（None/Silver/Gold/Platinum）

### 8. 取消订单
- 输入订单号
- 点击 "Cancel Order" 取消订单

## 管理员控制台系统

管理员系统保持原有的命令行交互方式，功能包括：
- 管理飞机型号
- 管理航班
- 管理机组成员
- 查看乘客列表
- 查看航班状态
- 生成飞行简报
- 生成飞行计划
- 自动安排航班
- 查看候补队列

**管理员账号：**
- 用户名：admin
- 密码：admin123

## 编译说明

使用以下命令编译（已在 tasks.json 中配置）：

```bash
g++ -g src/*.cpp -I include -o airline_reservation.exe -lcomctl32 -lgdi32 -luser32 -static-libgcc -static-libstdc++ -static
```

或在 VS Code 中按 `Ctrl+Shift+B` 执行默认构建任务。

## 文件结构

新增文件：
- `include/PassengerUI.h` - 乘客GUI界面头文件
- `src/PassengerUI.cpp` - 乘客GUI界面实现

修改文件：
- `src/main.cpp` - 添加模式选择功能
- `include/AirlineReservationSystem.h` - 添加公共API接口
- `src/AirlineReservationSystem.cpp` - 实现公共API方法

## 技术细节

### GUI实现
- 使用 Win32 API 创建原生Windows窗口
- 使用 Common Controls 库支持列表视图等控件
- 静态链接，无需额外DLL依赖

### API接口
系统现在提供以下公共API供GUI调用：
- `loginPassenger()` - 乘客登录
- `registerPassenger()` - 乘客注册
- `searchFlights()` - 搜索航班
- `createBooking()` - 创建订单
- `payOrder()` - 支付订单
- `checkInOrder()` - 办理值机
- `cancelOrder()` - 取消订单
- `getPassengerOrders()` - 获取乘客订单

## 注意事项

1. GUI窗口关闭后会退出程序
2. 所有数据自动保存到 `data` 文件夹的CSV文件中
3. 确保 `data` 文件夹与 exe 文件在同一目录
4. 管理员系统仍使用控制台交互，无GUI界面

## 后续改进方向

- 添加座位选择可视化图
- 添加更多的表单验证
- 改进界面美观度
- 添加更多的用户提示信息
- 支持多语言界面
