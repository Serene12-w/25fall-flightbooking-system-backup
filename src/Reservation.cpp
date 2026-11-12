#include "Reservation.h"

// 构造函数
Reservation::Reservation(const std::string& reservationNumber, Passenger* passenger, Flight* flight, Seat* seat)
    : reservationNumber(reservationNumber), orderNumber(""), passenger(passenger), flight(flight), seat(seat), status(ReservationStatus::PENDING) {}

// 带订单号的构造函数
Reservation::Reservation(const std::string& reservationNumber, const std::string& orderNumber,
                         Passenger* passenger, Flight* flight, Seat* seat)
    : reservationNumber(reservationNumber), orderNumber(orderNumber), passenger(passenger), flight(flight), seat(seat), status(ReservationStatus::PENDING) {}

// 获取预订号
std::string Reservation::getReservationNumber() const {
    return reservationNumber;
}

// 获取订单号
std::string Reservation::getOrderNumber() const {
    return orderNumber;
}

// 设置订单号
void Reservation::setOrderNumber(const std::string& orderNum) {
    orderNumber = orderNum;
}

// 获取乘客信息
Passenger* Reservation::getPassenger() const {
    return passenger;
}

// 获取航班信息
Flight* Reservation::getFlight() const {
    return flight;
}

// 获取座位信息
Seat* Reservation::getSeat() const {
    return seat;
}

// 获取预订状态
ReservationStatus Reservation::getStatus() const {
    return status;
}

// 确认预订
void Reservation::confirm() {
    status = ReservationStatus::CONFIRMED;
}

// 取消预订
void Reservation::cancel() {
    status = ReservationStatus::CANCELLED;
}

// 将预订加入候补
void Reservation::waitlist() {
    status = ReservationStatus::WAITLISTED;
}
