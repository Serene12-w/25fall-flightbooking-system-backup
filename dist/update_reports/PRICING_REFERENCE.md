# Quick Reference: Ticket Pricing System

## Pricing Formula

```
Base Price = Distance (km) × $0.15 × Cabin Multiplier
Final Price = Base Price × (1 - Loyalty Discount)
```

## Cabin Class Multipliers

| Cabin Class       | Multiplier | Example (1000km) |
|-------------------|------------|------------------|
| Economy Class     | 1.0×       | $150.00          |
| Premium Economy   | 1.5×       | $225.00          |
| Business Class    | 2.5×       | $375.00          |
| First Class       | 4.0×       | $600.00          |

## Loyalty Discounts

| Membership Tier | Discount | Example ($1000) |
|-----------------|----------|-----------------|
| Regular         | 0%       | $1000.00        |
| Silver          | 10%      | $900.00         |
| Gold            | 15%      | $850.00         |
| Platinum        | 20%      | $800.00         |

## Member Benefits Summary

### Platinum Members
- ✅ 20% OFF all tickets
- Priority check-in & boarding
- Premium lounge access
- 40kg baggage allowance
- Free seat selection
- Fast track security
- Complimentary upgrades
- 24/7 customer service
- Flexible booking changes

### Gold Members
- ✅ 15% OFF all tickets
- Priority check-in & boarding
- Standard lounge access
- 30kg baggage allowance
- Free seat selection
- Fast track security
- Flexible booking changes

### Silver Members
- ✅ 10% OFF all tickets
- Priority check-in & boarding
- 25kg baggage allowance
- Free seat selection

### Regular Members
- Standard 20kg baggage allowance
- No discounts (upgrade to save!)

## Sample Routes & Prices

### PEK (Beijing) ↔ LAX (Los Angeles)
**Distance**: ~9,800 km

| Class / Member    | Regular  | Silver   | Gold     | Platinum |
|-------------------|----------|----------|----------|----------|
| Economy           | $1,470   | $1,323   | $1,250   | $1,176   |
| Premium Economy   | $2,205   | $1,985   | $1,874   | $1,764   |
| Business          | $3,675   | $3,308   | $3,124   | $2,940   |
| First Class       | $5,880   | $5,292   | $4,998   | $4,704   |

### PEK (Beijing) ↔ NRT (Tokyo)
**Distance**: ~2,100 km

| Class / Member    | Regular  | Silver   | Gold     | Platinum |
|-------------------|----------|----------|----------|----------|
| Economy           | $315     | $284     | $268     | $252     |
| Premium Economy   | $473     | $425     | $402     | $378     |
| Business          | $788     | $709     | $670     | $630     |
| First Class       | $1,260   | $1,134   | $1,071   | $1,008   |

### PEK (Beijing) ↔ LHR (London)
**Distance**: ~8,100 km

| Class / Member    | Regular  | Silver   | Gold     | Platinum |
|-------------------|----------|----------|----------|----------|
| Economy           | $1,215   | $1,094   | $1,033   | $972     |
| Premium Economy   | $1,823   | $1,640   | $1,549   | $1,458   |
| Business          | $3,038   | $2,734   | $2,582   | $2,430   |
| First Class       | $4,860   | $4,374   | $4,131   | $3,888   |

## How to Use in System

1. **During Booking**: Price is automatically calculated and displayed
2. **Viewing Orders**: All orders show the ticket price paid
3. **Payment Screen**: Shows price before payment
4. **Profile Page**: Shows your discount rate in member benefits

## Code Implementation

The pricing system is implemented in:
- `AirlineReservationSystem::calculateBasePrice()` - Distance × Rate × Cabin
- `AirlineReservationSystem::getLoyaltyDiscount()` - Membership discounts
- `AirlineReservationSystem::calculateFinalPrice()` - Applies discount
- `Order::ticketPrice` - Stores the final price
- `OrderManager` - Persists prices to CSV

## Distance Calculation

Uses the **Haversine Formula** to calculate great circle distance:
```
a = sin²(Δlat/2) + cos(lat1) × cos(lat2) × sin²(Δlon/2)
c = 2 × atan2(√a, √(1−a))
distance = Earth_radius × c
```

Where:
- Earth radius = 6,371 km
- Coordinates from `airports.csv` (latitude/longitude)
- Accounts for Earth's curvature

## CSV Format

**data/orders.csv** now includes:
```csv
orderNumber,passportNumber,flightNumber,flightDate,seatNumber,seatType,status,createTime,paymentTime,checkInTime,creditCardLast4,ticketPrice
ORD2025110800001,G12345678,CA989,2025-11-10,1A,FIRST_CLASS,CHECKED_IN,...,3520.00
```

The `ticketPrice` field is:
- Stored as a double with 2 decimal places
- Displayed with $ prefix and 2 decimal formatting
- Automatically loaded and saved with orders

---

**Last Updated**: 2025-11-08
