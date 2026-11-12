# 乘客数据加载和保存问题修复

## 🐛 发现的问题

### 问题1: 无法读取已存在的乘客
**现象**: 输入已存在于 `passengers.csv` 中的护照号(如 P76543210)后,系统提示"Passenger not found"

**原因**: 
- `loadPassengers()` 函数将乘客加载到 `passengers` 向量中
- `findPassenger()` 函数在 `passengerTree` 树中搜索
- 初始化时只加载到向量,没有构建树结构

### 问题2: 新注册的乘客不保存到CSV
**现象**: 乘客注册成功后,重启程序时乘客信息丢失

**原因**:
- `addPassenger()` 函数只将乘客添加到树中
- `savePassengers()` 函数从 `passengers` 向量保存
- 新乘客没有被添加到向量中

### 问题3: 管理员无法查看乘客列表
**现象**: 选择"View passenger list"后没有显示任何乘客

**原因**:
- `viewPassengerList()` 函数从 `passengerTree` 显示
- 树结构为空(问题1的原因)

---

## ✅ 修复方案

### 修改1: `initializeData()` - 构建乘客树

**文件**: `src/AirlineReservationSystem.cpp`

**修改内容**:
```cpp
void AirlineReservationSystem::initializeData() {
    // ... 其他初始化代码 ...
    
    // 加载乘客数据
    DataLoader::loadPassengers("data/passengers.csv", passengers);
    
    // ⭐ 新增: 将加载的乘客添加到树中
    for (auto* passenger : passengers) {
        PassengerNode* newNode = new PassengerNode(passenger);
        if (passengerTree == nullptr) {
            passengerTree = newNode;
        } else {
            PassengerNode* current = passengerTree;
            while (true) {
                if (passenger->getPassportNumber() < current->passenger->getPassportNumber()) {
                    if (current->left == nullptr) {
                        current->left = newNode;
                        break;
                    }
                    current = current->left;
                } else {
                    if (current->right == nullptr) {
                        current->right = newNode;
                        break;
                    }
                    current = current->right;
                }
            }
        }
    }
    
    // ... 其他初始化代码 ...
}
```

**说明**: 
- 从CSV加载乘客后,遍历所有乘客并插入到二叉搜索树中
- 只添加到树,不重复添加到向量(因为已经在向量中了)

---

### 修改2: `addPassenger()` - 同时添加到树和向量

**文件**: `src/AirlineReservationSystem.cpp`

**修改内容**:
```cpp
void AirlineReservationSystem::addPassenger(Passenger* passenger) {
    // ⭐ 新增: 检查是否已存在
    if (findPassenger(passenger->getPassportNumber()) != nullptr) {
        return; // 已存在,不重复添加
    }
    
    // 添加到树中
    PassengerNode* newNode = new PassengerNode(passenger);
    if (passengerTree == nullptr) {
        passengerTree = newNode;
    } else {
        PassengerNode* current = passengerTree;
        while (true) {
            if (passenger->getPassportNumber() < current->passenger->getPassportNumber()) {
                if (current->left == nullptr) {
                    current->left = newNode;
                    break;
                }
                current = current->left;
            } else {
                if (current->right == nullptr) {
                    current->right = newNode;
                    break;
                }
                current = current->right;
            }
        }
    }
    
    // ⭐ 新增: 同时添加到向量中
    passengers.push_back(passenger);
}
```

**说明**:
- 先检查乘客是否已存在,避免重复添加
- 将乘客添加到树中(用于快速搜索)
- 将乘客添加到向量中(用于保存到CSV)

---

### 修改3: `passengerLogin()` - 保存新注册的乘客

**文件**: `src/AirlineReservationSystem.cpp`

**修改内容**:
```cpp
bool AirlineReservationSystem::passengerLogin() {
    std::string passportNumber;
    std::cout << "Enter your passport number: ";
    std::cin >> passportNumber;
    
    if (findPassenger(passportNumber) == nullptr) {
        std::string name;
        std::cout << "Passenger not found. Please enter your name to register: ";
        std::cin.ignore(); // ⭐ 新增: 清除输入缓冲区
        std::getline(std::cin, name); // ⭐ 修改: 使用getline读取姓名(支持空格)
        addPassenger(new Passenger(name, passportNumber));
        std::cout << "Registration successful. Welcome, " << name << "!" << std::endl;
        // ⭐ 新增: 立即保存到CSV
        savePassengersData();
    } else {
        std::cout << "Welcome back, " << findPassenger(passportNumber)->getName() << "!" << std::endl;
    }
    
    return true;
}
```

**说明**:
- 添加 `std::cin.ignore()` 清除输入缓冲区
- 使用 `std::getline()` 读取姓名,支持包含空格的姓名
- 注册后立即调用 `savePassengersData()` 保存到CSV

---

## 🔍 数据流程图

### 启动时(加载数据)
```
passengers.csv
    ↓
DataLoader::loadPassengers()
    ↓
passengers (vector) ← 存储所有乘客
    ↓
initializeData() 遍历向量
    ↓
passengerTree (BST) ← 构建搜索树
```

### 新乘客注册
```
用户输入护照号和姓名
    ↓
addPassenger(new Passenger(...))
    ├─→ passengerTree (BST) ← 添加到搜索树
    └─→ passengers (vector) ← 添加到向量
        ↓
    savePassengersData()
        ↓
    passengers.csv ← 保存到文件
```

### 查找乘客
```
findPassenger(passportNumber)
    ↓
在 passengerTree 中搜索 (O(log n))
    ↓
返回 Passenger* 或 nullptr
```

### 显示所有乘客
```
viewPassengerList()
    ↓
displayPassengers(passengerTree)
    ↓
中序遍历树 → 按护照号排序显示
```

---

## 📋 测试验证

### 测试1: 读取已存在的乘客 ✅
1. 运行程序
2. 选择"Passenger System"
3. 输入 `P76543210` (Emma Johnson)
4. **期望**: 显示 "Welcome back, Emma Johnson!"

### 测试2: 注册新乘客并保存 ✅
1. 运行程序
2. 选择"Passenger System"
3. 输入新护照号(如 `G99999999`)
4. 输入姓名(如 `Test User`)
5. 重启程序,再次输入 `G99999999`
6. **期望**: 显示 "Welcome back, Test User!"

### 测试3: 查看乘客列表 ✅
1. 以管理员身份登录
2. 选择"View passenger list"
3. **期望**: 显示所有乘客(按护照号排序)

---

## 💡 总结

### 核心问题
系统使用了两种数据结构存储乘客:
- `passengers` 向量: 用于保存到CSV
- `passengerTree` 二叉搜索树: 用于快速查找

但初始化时只填充了向量,没有构建树;添加新乘客时只添加到树,没有更新向量。

### 解决方案
1. 初始化时同步两种数据结构
2. 添加新乘客时同步两种数据结构
3. 新乘客注册后立即保存

### 数据一致性保证
- 加载时: CSV → vector → tree
- 添加时: tree + vector 同时更新
- 保存时: vector → CSV
- 查找时: tree (快速)
- 显示时: tree (有序)
