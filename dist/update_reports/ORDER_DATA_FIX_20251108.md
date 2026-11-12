# Order Data Logic Fix Report

**Date**: 2025-11-08  
**Issue**: Logical errors in orders.csv - PAID status orders had seat numbers assigned

---

## Problem Identified

### Issue 1: Data Inconsistency
Found orders with status `PAID` that already had seat numbers assigned:

```
ORD2025110800008: G10000002, Status=PAID, Seat=5A ❌
ORD2025110800009: P20000002, Status=PAID, Seat=5B ❌
ORD2025110800015: P20000003, Status=PAID, Seat=3A ❌
ORD2025110800019: G10000004, Status=PAID, Seat=12A ❌
```

**This violates the business logic!**

---

## System Logic Confirmation ✓

The correct booking flow is:

### Step 1: Book Ticket (`bookTicket()`)
- Creates order in `orders` table
- Status: `PENDING_PAYMENT`
- Seat Number: **EMPTY**
- Price: Calculated based on distance + cabin + loyalty
- **NOT** written to `reservations` table

### Step 2: Pay for Ticket (`payForTicket()`)
- Updates order in `orders` table
- Status: `PENDING_PAYMENT` → `PAID`
- Seat Number: **STILL EMPTY**
- Payment info: Credit card, payment time recorded
- **NOT** written to `reservations` table

### Step 3: Check-In (`checkIn()`)
- Passenger selects seat
- Updates order in `orders` table:
  - Status: `PAID` → `CHECKED_IN`
  - Seat Number: **NOW ASSIGNED** (e.g., "1A")
  - Check-in time recorded
- **NOW** written to `reservations` table
- Aircraft seat marked as booked

### Unique Identifier
- `passportNumber` is used to retrieve all orders for a passenger

---

## Data Fixed

### Orders Modified

| Order Number | Passport | Flight | Old Seat | New Seat | Status |
|-------------|----------|---------|----------|----------|---------|
| ORD2025110800008 | G10000002 | CA989 | 5A | **(empty)** | PAID |
| ORD2025110800009 | P20000002 | CA989 | 5B | **(empty)** | PAID |
| ORD2025110800015 | P20000003 | CA981 | 3A | **(empty)** | PAID |
| ORD2025110800019 | G10000004 | MU587 | 12A | **(empty)** | PAID |

### Correct Data State

**CHECKED_IN Orders** (Have seat numbers ✓):
```csv
ORD2025110800001,G12345678,CA989,2025-11-10,1A,FIRST_CLASS,CHECKED_IN,...
ORD2025110800002,G12345678,CA989,2025-11-10,1B,FIRST_CLASS,CHECKED_IN,...
ORD2025110800003,P87654321,CA989,2025-11-10,2A,BUSINESS_CLASS,CHECKED_IN,...
ORD2025110800004,G10000001,CA989,2025-11-10,3A,BUSINESS_CLASS,CHECKED_IN,...
ORD2025110800005,P20000001,CA989,2025-11-10,3B,PREMIUM_ECONOMY,CHECKED_IN,...
ORD2025110800006,E30000001,CA989,2025-11-10,4A,PREMIUM_ECONOMY,CHECKED_IN,...
ORD2025110800007,J40000001,CA989,2025-11-10,4B,ECONOMY_CLASS,CHECKED_IN,...
```

**PAID Orders** (No seat numbers ✓):
```csv
ORD2025110800008,G10000002,CA989,2025-11-10,,ECONOMY_CLASS,PAID,...
ORD2025110800009,P20000002,CA989,2025-11-10,,ECONOMY_CLASS,PAID,...
ORD2025110800015,P20000003,CA981,2025-11-10,,PREMIUM_ECONOMY,PAID,...
ORD2025110800019,G10000004,MU587,2025-11-11,,ECONOMY_CLASS,PAID,...
```

**PENDING_PAYMENT Orders** (No seat numbers ✓):
```csv
ORD2025110800010,E30000002,CA989,2025-11-10,,ECONOMY_CLASS,PENDING_PAYMENT,...
ORD2025110800020,P20000004,MU587,2025-11-11,,ECONOMY_CLASS,PENDING_PAYMENT,...
```

---

## Verification

### Rule 1: PENDING_PAYMENT Orders
- ✓ Must NOT have seat numbers
- ✓ Must NOT have payment time
- ✓ Must NOT have check-in time

### Rule 2: PAID Orders
- ✓ Must NOT have seat numbers
- ✓ Must have payment time
- ✓ Must NOT have check-in time

### Rule 3: CHECKED_IN Orders
- ✓ Must have seat numbers
- ✓ Must have payment time
- ✓ Must have check-in time

---

## Database Relationships

### orders.csv
- Primary key: `orderNumber`
- Foreign key: `passportNumber` → passengers.csv
- Foreign key: `flightNumber` → flights.csv
- Contains: Complete order lifecycle (booking → payment → check-in)

### reservations.csv
- Created ONLY when passenger checks in
- Format: `flightNumber,seatReservations`
- Seat format: `"seatNumber:passportNumber:status"`
- Example: `"1A:G12345678:CONFIRMED,1B:G12345678:CONFIRMED"`

---

## Code Implementation Details

### File: `src/AirlineReservationSystem.cpp`

**bookTicket()** (Line 355):
```cpp
// Create order with PENDING_PAYMENT status
Order* order = orderManager.createOrder(
    currentPassenger->getPassportNumber(),
    selectedFlight->getFlightNumber(),
    flightDate,
    selectedSeatType
);
// Seat number is empty at this point
```

**payForTicket()** (Line 488):
```cpp
// Update order status to PAID
selectedOrder->setStatus(OrderStatus::PAID);
// Seat number remains empty
// Not saved to reservations.csv yet
```

**checkIn()** (Line 573):
```cpp
// Assign seat number
selectedOrder->setSeatNumber(seatStr.str());
selectedOrder->setStatus(OrderStatus::CHECKED_IN);

// Create reservation record
Reservation* reservation = new Reservation(...);
reservation->confirm();
reservations[reservationNumber] = reservation;

// Save to both files
orderManager.saveToCSV("data/orders.csv");
saveReservationsData(); // → reservations.csv
```

---

## Summary

✅ **Fixed 4 orders** with incorrect seat assignments  
✅ **Confirmed business logic** is correctly implemented in code  
✅ **Data integrity** restored - all orders now follow proper workflow  
✅ **No code changes needed** - logic was already correct, only data was wrong  

The system is now consistent and ready for use!
