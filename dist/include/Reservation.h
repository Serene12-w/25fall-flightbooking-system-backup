#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>
#include "Passenger.h"
#include "Flight.h"
#include "Seat.h"
#include "Utils.h"

// 预订类
class Reservation {
public:
    // 构造函数
    Reservation(const std::string& reservationNumber, Passenger* passenger, Flight* flight, Seat* seat);

    // 带订单号的构造函数
    Reservation(const std::string& reservationNumber, const std::string& orderNumber, 
                Passenger* passenger, Flight* flight, Seat* seat);

    // 获取预订号
    std::string getReservationNumber() const;

    // 获取订单号
    std::string getOrderNumber() const;

    // 设置订单号
    void setOrderNumber(const std::string& orderNum);

    // 获取乘客信息
    Passenger* getPassenger() const;

    // 获取航班信息
    Flight* getFlight() const;

    // 获取座位信息
    Seat* getSeat() const;

    // 获取预订状态
    ReservationStatus getStatus() const;

    // 确认预订
    void confirm();

    // 取消预订
    void cancel();

    // 将预订加入候补
    void waitlist();

private:
    std::string reservationNumber; // 预订号
    std::string orderNumber;       // 订单号
    Passenger* passenger;          // 乘客
    Flight* flight;                // 航班
    Seat* seat;                    // 座位
    ReservationStatus status;      // 预订状态
};

#endif // RESERVATION_H
