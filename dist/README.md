# 航空订票系统 (Airline Reservation System)

## 系统简介

这是一个功能完整的航空订票管理系统，支持乘客订票、支付、值机以及管理员的航班管理功能。

**最新更新：** 现已支持图形用户界面(GUI)！乘客可以通过友好的Windows图形界面进行操作。

## 快速开始

### 编译方法1：使用 VS Code（生成带图标的 Flight Ticket System.exe）
1. 打开项目文件夹
2. 按 `Ctrl+Shift+B` 运行默认构建任务（会先调用 windres 编译 `res/app.rc`，然后链接生成 `Flight Ticket System.exe`，使用 `test/icon.ico` 图标）

### 编译方法2：使用批处理文件
双击运行 `build.bat`

### 编译方法3：手动编译（含图标资源）
```powershell
windres res/app.rc app.res
g++ -g -std=c++17 src/*.cpp app.res -I include -o "Flight Ticket System.exe" -mwindows -static -static-libgcc -static-libstdc++ -lwinpthread -lcomctl32 -lgdi32 -luser32
```

### 运行程序
1. 双击 `Flight Ticket System.exe`（或在 VS Code 任务里运行 run Flight Ticket System）
2. 在控制台选择模式：
   - 输入 `1` - 乘客系统（图形界面）
   - 输入 `2` - 管理员系统（命令行）
   - 输入 `0` - 退出

## 管理员账号

- 用户名：`admin`
- 密码：`admin123`

## 系统功能

### 乘客系统 (GUI)
- 注册/登录
- 搜索航班（支持直飞和转机）
- 预订机票
- 在线支付
- 办理值机选座
- 查看订单
- 取消订单
- 查看个人信息和会员权益

### 管理员系统 (控制台)
- 管理飞机型号
- 管理航班信息
- 管理机组成员
- 查看乘客列表
- 查看航班状态
- 生成飞行计划
- 生成飞行简报
- 自动调度航班
- 管理候补队列

## 数据文件

所有数据保存在 `data` 文件夹中，包括：
- `aircrafts.csv` - 飞机型号配置
- `airports.csv` - 机场信息
- `crew.csv` - 机组成员
- `flights.csv` - 航班信息
- `passengers.csv` - 乘客数据
- `orders.csv` - 订单记录
- `reservations.csv` - 预订信息
- `waitlist.csv` - 候补名单

## 项目结构

```
Filghtbooking_v3/
├── include/              # 头文件
│   ├── Aircraft.h
│   ├── AirlineData.h
│   ├── AirlineReservationSystem.h
│   ├── Airport.h
│   ├── Crew.h
│   ├── DataLoader.h
│   ├── Flight.h
│   ├── FlightCalculator.h
│   ├── FlightPlan.h
│   ├── Order.h
│   ├── OrderManager.h
│   ├── Passenger.h
│   ├── PassengerUI.h     # GUI界面
│   ├── Reservation.h
│   ├── Seat.h
│   └── Utils.h
├── src/                  # 源文件
│   ├── *.cpp             # 对应的实现文件
│   └── main.cpp          # 主程序入口
├── data/                 # 数据文件
│   └── *.csv
├── update_reports/       # 更新说明文档
│   ├── GUI_UPDATE.md
│   └── GUI_TEST_GUIDE.md
├── build.bat            # 编译脚本
└── README.md            # 本文件
```

## 技术特性

- **数据结构：** 使用哈希表、二叉搜索树、优先队列、图等数据结构
- **算法：** Dijkstra最短路径算法用于转机航班搜索
- **GUI：** Win32 API原生Windows界面
- **数据持久化：** CSV文件存储
- **静态链接：** 生成独立的可执行文件

## 更多文档

详细的功能说明和测试指南请查看：
- [GUI更新说明](update_reports/GUI_UPDATE.md)
- [GUI测试指南](update_reports/GUI_TEST_GUIDE.md)

## 系统要求

- Windows 7 或更高版本
- MinGW-w64 (包含g++编译器)

## 开发环境

- Visual Studio Code
- C++11 或更高版本
- MinGW-w64 GCC

---

**版本：** v3.0 (GUI Update)  
**最后更新：** 2025-11-11
