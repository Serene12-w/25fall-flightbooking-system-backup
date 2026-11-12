# Flight Ticket System (FlightTicketSystem)

## If you have any questions while reviewing our project, or encounter compilation issues, feel free to contact us at any time! Email: 2661567025@qq.com (Team Leader 徐至谦)

## Project Introduction

This is a flight ticket management system based on a data structures course project, implementing core functions such as flight information management, ticket booking, and queries. The system is developed in C/C++ and utilizes various data structures (linked lists, trees, graphs, hash tables, etc.) to optimize flight queries and route planning algorithms.

## Project Structure

```
FlightTicketSystem/
├── src/              # Source code directory
├── include/          # Header files directory
├── data/             # Data files directory
├── test/             # Test files directory
├── scripts/          # Script files directory
├── res/              # Resource files directory
├── dist/             # Compilation output directory
├── release/          # Release version directory
├── update_reports/   # Update reports directory
├── .vscode/          # VS Code configuration files
└── FlightTicketSystem.exe/     # Main program
```


## Development Environment

- **Programming Language**: C/C++ (C++11 and above)
- **Development Tool**: Visual Studio Code
- **Compiler**: GCC/G++ or MSVC
- **Version Control**: Git
- **Build System**: Makefile / CMake

## Quick Start

## Please modify the compiler path in .vscode before compiling!!!

### Compile Project

```bash
# Linux/Mac
cd scripts
./build.sh

# Windows
# Use VS Code build tasks or command line compilation
g++ -std=c++11 src/*.cpp -Iinclude -o dist/FlightTicketSystem
```

### Run Program

```bash
# Enter compilation output directory
cd dist

# Run main program
./FlightTicketSystem
```

### Testing

```bash
# Compile and run tests
cd test
g++ -std=c++11 test_*.cpp ../src/*.cpp -I../include -o test_suite
./test_suite
```

## User Guide

1. **First Run**: The system will initialize data files and create a default administrator account
2. **Administrator Account**: Username `admin`, Password `admin123` (Please change after first login)
3. **Data Backup**: It is recommended to regularly backup data files in the `data/` directory
4. **Configuration Modification**: System configuration can be modified through `res/config.ini`

## Function Demonstration

```
=================================
  Welcome to Flight Ticket Management System
=================================
1. User Login
2. User Registration
3. Query Flights
4. Exit System
=================================
Please select function (1-4): 
```

## Important Notes

- ⚠️ Data files are in binary format, do not edit manually
- ⚠️ Ensure the `data/` directory exists and has read/write permissions
- ⚠️ Administrator operations should be performed carefully to avoid deleting important data
- ⚠️ Administrator account is admin, password is admin123

## Project Status

✅ Core functions development completed  
🔧 Continuous optimization in progress

## Update Log

See version update documents in the `update_reports/` directory

## Development Team

Group 8 All Members
## 徐至谦 (Team Leader)
## 张欣玥
## 伍思哲
## 李扬天
## 谢振宇
## 寥子曹

## License

This project is for learning and research purposes only

---

*Data Structures Course Project - 2025*
