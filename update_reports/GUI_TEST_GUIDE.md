# GUI系统快速测试指南

## 编译步骤

1. 打开VS Code
2. 按 `Ctrl+Shift+B` 运行构建任务
3. 等待编译完成

或在终端运行：
```bash
cd e:\2025\Data_Structure\project\Filghtbooking_v3
g++ -g src/*.cpp -I include -o airline_reservation.exe -lcomctl32 -lgdi32 -luser32 -static-libgcc -static-libstdc++ -static
```

## 运行程序

1. 双击运行 `airline_reservation.exe`
2. 在控制台选择模式：
   - 输入 `1` 进入乘客GUI系统
   - 输入 `2` 进入管理员控制台系统

## 测试乘客GUI系统

### 测试1：注册新乘客
1. 选择模式 `1`（乘客GUI）
2. 在登录界面：
   - Name: `Test User`
   - Passport Number: `TEST001`
3. 点击 `Register` 按钮
4. 应该显示注册成功消息并进入主菜单

### 测试2：登录已有乘客
1. 使用已有的护照号，例如：`G12345678`
2. 点击 `Login` 按钮
3. 应该显示欢迎消息并进入主菜单

### 测试3：搜索航班
1. 点击 `Search Flights`
2. 输入：
   - Origin: `PEK`
   - Destination: `LAX`
   - Date: `2025-11-10`
3. 点击 `Search`
4. 应该显示可用航班列表（如CA989）

### 测试4：预订航班
1. 在搜索结果中选择一个航班（点击列表项）
2. 点击 `Book Selected Flight`
3. 系统会创建订单并显示订单号和价格
4. 记录订单号供后续测试使用

### 测试5：支付订单
1. 返回主菜单，点击 `Pay for Ticket`
2. 输入：
   - Order Number: （使用测试4中的订单号）
   - Credit Card: `1234`
3. 点击 `Pay Now`
4. 应该显示支付成功消息

### 测试6：查看订单
1. 点击 `View My Orders`
2. 应该显示该乘客的所有订单
3. 包括状态（Pending Payment / Paid / Checked In）

### 测试7：办理值机
1. 点击 `Check In`
2. 输入：
   - Order Number: （使用已支付的订单号）
   - Seat Number: `1A`
3. 点击 `Check In Now`
4. 应该显示值机成功消息

### 测试8：查看个人信息
1. 点击 `My Profile`
2. 应该显示姓名、护照号和会员等级

### 测试9：取消订单
1. 创建一个新订单（重复测试3-4）
2. 返回主菜单，点击主窗口的关闭按钮（暂不支持取消按钮在主菜单）
3. 重新启动，选择乘客模式并登录
4. 测试取消功能（待实现）

## 测试管理员控制台系统

1. 运行程序，选择模式 `2`
2. 输入管理员账号：
   - Username: `admin`
   - Password: `admin123`
3. 测试各项管理功能（保持原有控制台交互）

## 已知限制

1. GUI窗口大小固定为800x600
2. 部分功能仍需完善（如座位图可视化）
3. 需要改进错误提示的友好度
4. 暂未实现主菜单的返回按钮

## 数据文件

所有数据保存在 `data` 文件夹：
- `aircrafts.csv` - 飞机数据
- `airports.csv` - 机场数据
- `crew.csv` - 机组数据
- `flights.csv` - 航班数据
- `passengers.csv` - 乘客数据
- `orders.csv` - 订单数据
- `reservations.csv` - 预订数据
- `waitlist.csv` - 候补名单

## 故障排除

### 编译错误
- 确保已安装MinGW-w64
- 确保g++在系统PATH中
- 检查include路径是否正确

### 运行错误
- 确保data文件夹存在
- 确保所有CSV文件格式正确
- 检查文件读写权限

### GUI不显示
- 检查是否选择了正确的模式（1）
- 检查Windows版本是否支持Win32 API
- 查看控制台是否有错误消息
