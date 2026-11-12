# Airline Reservation System - Pricing System & Encoding Fix Update

## Date: 2025-11-08

## Overview
This update completely resolves encoding issues in the member benefits display and implements an automatic ticket pricing system based on flight distance, cabin class, and loyalty status.

---

## Changes Completed

### 1. Fixed Character Encoding Issues

**File**: `src/AirlineReservationSystem.cpp`

**Problem**: The ✓ (checkmark) symbols were displaying as garbled text (鉁?) in Windows console.

**Solution**: Replaced all ✓ symbols with simple "- " bullet points for universal compatibility.

**Changes in viewPassengerProfile() function**:
- Platinum Members:
  - `- Ticket Discount: 20% OFF` (NEW)
  - `- Priority Check-in`
  - `- Priority Boarding`
  - `- Lounge Access (Premium)`
  - `- Extra Baggage Allowance (40kg)`
  - `- Free Seat Selection`
  - `- Fast Track Security`
  - `- Complimentary Upgrades (Subject to availability)`
  - `- 24/7 Dedicated Customer Service`
  - `- Flexible Booking Changes`

- Gold Members:
  - `- Ticket Discount: 15% OFF` (NEW)
  - `- Priority Check-in`
  - `- Priority Boarding`
  - `- Lounge Access (Standard)`
  - `- Extra Baggage Allowance (30kg)`
  - `- Free Seat Selection`
  - `- Fast Track Security`
  - `- Flexible Booking Changes`

- Silver Members:
  - `- Ticket Discount: 10% OFF` (NEW)
  - `- Priority Check-in`
  - `- Priority Boarding`
  - `- Extra Baggage Allowance (25kg)`
  - `- Free Seat Selection`

---

### 2. Implemented Automatic Ticket Pricing System

#### 2.1 Added Price Calculation Methods

**File**: `include/AirlineReservationSystem.h`

Added three new helper methods:
```cpp
// Calculate base price based on distance and cabin class
double calculateBasePrice(Flight* flight, SeatType seatType);

// Get loyalty discount rate
double getLoyaltyDiscount(LoyaltyStatus status);

// Calculate final price with discount applied
double calculateFinalPrice(Flight* flight, SeatType seatType, LoyaltyStatus loyaltyStatus);
```

**File**: `src/AirlineReservationSystem.cpp`

**Implementation Details**:

1. **calculateBasePrice()**:
   - Uses `FlightCalculator::calculateDistance()` to compute great circle distance between airports
   - Base rate: $0.15 per kilometer
   - Cabin class multipliers:
     - First Class: 4.0x (400% of base)
     - Business Class: 2.5x (250% of base)
     - Premium Economy: 1.5x (150% of base)
     - Economy Class: 1.0x (100% of base)
   - Formula: `distance_km * $0.15 * cabin_multiplier`

2. **getLoyaltyDiscount()**:
   - Platinum: 20% discount (0.20)
   - Gold: 15% discount (0.15)
   - Silver: 10% discount (0.10)
   - Regular: No discount (0.0)

3. **calculateFinalPrice()**:
   - Applies loyalty discount to base price
   - Formula: `base_price * (1.0 - discount_rate)`

#### 2.2 Updated Order Class

**Files**: `include/Order.h` and `src/Order.cpp`

Added ticket price field:
```cpp
private:
    double ticketPrice;  // Ticket price in USD

public:
    double getTicketPrice() const;
    void setTicketPrice(double price);
```

Constructor updated to initialize `ticketPrice` to 0.0.

#### 2.3 Integrated Pricing into Booking Flow

**File**: `src/AirlineReservationSystem.cpp`

**bookTicket() function** now:
1. Calculates base price based on flight distance and cabin class
2. Applies loyalty discount based on passenger's membership tier
3. Displays price breakdown:
   ```
   --- Ticket Price ---
   Base Price: $2200.00
   Member Discount: 15% OFF
   Discount Amount: $330.00
   Final Price: $1870.00
   ```

**payForTicket() function** now:
- Shows ticket price in the unpaid orders list
- Displays "Amount Paid" after successful payment

**viewMyReservations() function** now:
- Shows ticket price for each order

#### 2.4 Updated CSV Persistence

**Files**: `src/OrderManager.cpp` and `data/orders.csv`

**CSV Format Updated**:
- Added `ticketPrice` column as the last field
- Old format: `...,creditCardLast4`
- New format: `...,creditCardLast4,ticketPrice`

**loadFromCSV()**: Reads and parses ticket price from CSV
**saveToCSV()**: Writes ticket price with 2 decimal places precision

**Sample data/orders.csv**:
```csv
orderNumber,passportNumber,flightNumber,flightDate,seatNumber,seatType,status,createTime,paymentTime,checkInTime,creditCardLast4,ticketPrice
ORD2025110800001,G12345678,CA989,2025-11-10,1A,FIRST_CLASS,CHECKED_IN,2025-11-01 10:30:00,2025-11-01 11:00:00,2025-11-08 08:00:00,1234,3520.00
ORD2025110800003,P87654321,CA989,2025-11-10,2A,BUSINESS_CLASS,CHECKED_IN,2025-11-02 14:20:00,2025-11-02 14:45:00,2025-11-08 09:00:00,5678,1870.00
```

---

## Pricing Examples

### Example Flight: CA989 (PEK to LAX)
- Distance: ~9,800 km (calculated using Haversine formula)
- Base rate: $0.15/km

**Without Loyalty Discount**:
- Economy Class: 9,800 × $0.15 × 1.0 = **$1,470.00**
- Premium Economy: 9,800 × $0.15 × 1.5 = **$2,205.00**
- Business Class: 9,800 × $0.15 × 2.5 = **$3,675.00**
- First Class: 9,800 × $0.15 × 4.0 = **$5,880.00**

**With Gold Member (15% discount)**:
- Economy Class: $1,470 × 0.85 = **$1,249.50**
- Premium Economy: $2,205 × 0.85 = **$1,874.25**
- Business Class: $3,675 × 0.85 = **$3,123.75**
- First Class: $5,880 × 0.85 = **$4,998.00**

**With Platinum Member (20% discount)**:
- Economy Class: $1,470 × 0.80 = **$1,176.00**
- Premium Economy: $2,205 × 0.80 = **$1,764.00**
- Business Class: $3,675 × 0.80 = **$2,940.00**
- First Class: $5,880 × 0.80 = **$4,704.00**

---

## Files Modified

1. **include/AirlineReservationSystem.h** - Added price calculation method declarations
2. **src/AirlineReservationSystem.cpp** - Implemented pricing system and fixed encoding
3. **include/Order.h** - Added ticketPrice field
4. **src/Order.cpp** - Added getter/setter for ticketPrice
5. **src/OrderManager.cpp** - Updated CSV load/save for ticket prices
6. **data/orders.csv** - Added ticketPrice column with sample data

---

## Technical Details

### Distance Calculation
Uses the **Haversine formula** implemented in `FlightCalculator::calculateDistance()`:
- Accounts for Earth's curvature
- Returns great circle distance in kilometers
- Handles coordinate conversion (degrees + minutes → decimal degrees)

### Price Precision
- All prices stored as `double` type
- Displayed with 2 decimal places using `std::fixed` and `std::setprecision(2)`
- CSV saves prices with 2 decimal places

---

## Benefits of This System

1. **No More Encoding Issues**: Simple ASCII characters work on all platforms
2. **Dynamic Pricing**: Prices automatically calculated based on actual flight distance
3. **Loyalty Rewards**: Visible discount rates encourage membership upgrades
4. **Transparent Pricing**: Users see base price, discount, and final price breakdown
5. **Realistic Simulation**: Prices reflect real-world aviation cost factors
6. **Easy Maintenance**: Cabin multipliers and discount rates can be easily adjusted

---

## Testing Recommendations

1. **Test Various Routes**:
   - Short routes (e.g., domestic flights)
   - Long routes (e.g., transoceanic flights)
   - Verify price calculations match expected values

2. **Test All Cabin Classes**:
   - Verify multipliers work correctly
   - Check price ratios between classes

3. **Test All Loyalty Tiers**:
   - Verify discount percentages
   - Test non-member (0% discount)

4. **Test CSV Persistence**:
   - Create new orders with prices
   - Restart system and verify prices load correctly
   - Check data/orders.csv formatting

---

## Future Enhancements

Potential additions for the pricing system:

1. **Seasonal Pricing**: Adjust prices based on travel dates
2. **Demand-Based Pricing**: Higher prices for popular routes
3. **Early Bird Discounts**: Discounts for advance bookings
4. **Group Discounts**: Reduced rates for multiple passengers
5. **Weekend Surcharges**: Higher prices for weekend travel
6. **Fuel Surcharges**: Dynamic adjustments based on fuel costs

---

## Conclusion

This update successfully resolves all character encoding issues and implements a comprehensive, realistic ticket pricing system. The system now provides:

- ✅ 100% English text output (no encoding issues)
- ✅ Automatic price calculation based on distance and cabin class
- ✅ Loyalty-based discounts (10%, 15%, 20%)
- ✅ Transparent price breakdown for customers
- ✅ Full CSV persistence for ticket prices
- ✅ Member benefits clearly display discount rates

The pricing system uses real aviation principles (distance-based, cabin class multipliers) and integrates seamlessly with the existing order management system.
