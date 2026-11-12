# CSV数据持久化修复说明

## 问题描述
之前系统在运行时添加的飞机、航班和机组成员数据只保存在内存中，程序关闭后就会丢失，因为没有将更改写回CSV文件。

## 解决方案

### 1. 新增功能

#### DataLoader.h 和 DataLoader.cpp
添加了三个保存函数：

- `saveAircrafts()` - 保存飞机数据到CSV
- `saveFlights()` - 保存航班数据到CSV  
- `saveCrewMembers()` - 保存机组成员数据到CSV
- `escapeCSVField()` - 辅助函数，处理包含逗号的CSV字段

#### Aircraft.h
添加了获取器方法以便保存数据：
- `getFirstClassRows()`
- `getBusinessClassRows()`
- `getPremiumEconomyRows()`
- `getEconomyClassRows()`
- `getSeatsPerRow()`

#### AirlineReservationSystem.h 和 .cpp
添加了保存数据的方法：
- `saveAllData()` - 保存所有数据
- `saveAircraftsData()` - 保存飞机数据
- `saveFlightsData()` - 保存航班数据
- `saveCrewData()` - 保存机组成员数据

### 2. 自动保存机制

修改了以下函数，在添加/删除数据后自动保存：

#### ✅ `addAircraftModel()`
添加飞机型号后自动调用 `saveAircraftsData()`

#### ✅ `addFlight()`
添加航班后自动调用 `saveFlightsData()`

#### ✅ `removeFlight()`
删除航班后自动调用 `saveFlightsData()`

#### ✅ `addCrewMember()`
添加机组成员后自动调用 `saveCrewData()`

## 使用方法

### 编译程序
```bash
g++ -g src/*.cpp -I include -o airline_reservation.exe -static-libgcc -static-libstdc++ -static
```

### 运行程序
```bash
./airline_reservation.exe
```

### 测试持久化

1. **登录管理员账号**
   - 账号: `admin`
   - 密码: `admin123`

2. **添加新飞机型号**
   - 选择: `1. Manage aircraft models`
   - 选择: `1. Add new aircraft model`
   - 输入飞机信息
   - 系统会显示: `Successfully saved X aircraft models to data/aircrafts.csv`

3. **添加新航班**
   - 选择: `2. Manage flights`
   - 选择: `1. Add new flight`
   - 输入航班信息
   - 系统会显示: `Successfully saved X flights to data/flights.csv`

4. **添加机组成员**
   - 选择: `3. Manage crew`
   - 选择: `1. Add new crew member`
   - 输入成员信息
   - 系统会显示: `Successfully saved X crew members to data/crew.csv`

5. **验证持久化**
   - 退出程序
   - 重新运行程序
   - 检查刚才添加的数据是否还在
   - 或直接打开CSV文件查看

## CSV文件格式

### aircrafts.csv
```csv
type,firstClassRows,businessClassRows,premiumEconomyRows,economyClassRows,seatsPerRow
B787-9,8,28,20,48,9
```

### flights.csv
```csv
flightNumber,origin,destination,departureTime,arrivalTime,aircraftType
CA989,PEK,LAX,15:30,11:00,B787-9
```

### crew.csv
```csv
id,name,role,qualifications
P001,Captain Zhang Wei,PILOT,"B787-9,B787-8,B787-10,B777-300ER"
CC001,Flight Attendant Li Na,CABIN_CREW,"A380-800,A350-900,B787-9"
```

## 技术细节

### CSV写入特性
- 自动处理包含逗号的字段（用引号包围）
- 支持多个资质列表的正确格式化
- UTF-8编码兼容
- 标题行自动生成

### 数据同步
- 内存和文件实时同步
- 每次修改后立即保存
- 避免数据丢失

## 注意事项

⚠️ **重要提示**：
1. 确保 `data/` 目录存在且有写入权限
2. 不要在程序运行时手动编辑CSV文件
3. 建议定期备份CSV文件
4. 如果CSV文件损坏，程序会使用内置的默认数据

## 优势

✅ **数据持久化** - 所有更改永久保存
✅ **即时生效** - 无需手动保存或重启
✅ **易于维护** - 可直接编辑CSV文件
✅ **向后兼容** - 保持原有CSV格式不变
✅ **自动备份** - 每次保存都会覆盖旧数据

## 文件修改列表

修改的文件：
- ✏️ `include/DataLoader.h` - 添加保存函数声明
- ✏️ `src/DataLoader.cpp` - 实现保存函数
- ✏️ `include/Aircraft.h` - 添加获取器方法
- ✏️ `include/AirlineReservationSystem.h` - 添加保存方法声明
- ✏️ `src/AirlineReservationSystem.cpp` - 实现自动保存机制

新增文件：
- 📄 `CSV_PERSISTENCE_FIX.md` - 本说明文档
