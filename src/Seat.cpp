#include "Seat.h"

// 构造函数
Seat::Seat(int row, char col, SeatType type)
    : row(row), col(col), type(type), booked(false) {}

// 获取座位行号
int Seat::getRow() const {
    return row;
}

// 获取座位列号
char Seat::getCol() const {
    return col;
}

// 获取座位类型
SeatType Seat::getType() const {
    return type;
}

// 检查座位是否被预订
bool Seat::isBooked() const {
    return booked;
}

// 预订座位
void Seat::book() {
    booked = true;
}

// 取消预订
void Seat::unbook() {
    booked = false;
}
