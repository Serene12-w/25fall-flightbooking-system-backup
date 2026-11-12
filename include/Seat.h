#ifndef SEAT_H
#define SEAT_H

#include "Utils.h"

// 座位类
class Seat {
public:
    // 构造函数
    Seat(int row, char col, SeatType type);

    // 获取座位行号
    int getRow() const;

    // 获取座位列号
    char getCol() const;

    // 获取座位类型
    SeatType getType() const;

    // 检查座位是否被预订
    bool isBooked() const;

    // 预订座位
    void book();

    // 取消预订
    void unbook();

private:
    int row;        // 行号
    char col;       // 列号
    SeatType type;  // 座位类型
    bool booked;    // 是否被预订
};

#endif // SEAT_H
