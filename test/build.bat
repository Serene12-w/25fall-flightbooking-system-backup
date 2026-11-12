@echo off
echo Compiling Airline Reservation System with GUI (release, static runtime)...
echo.

REM Build with optimizations, strip symbols and request static linking of runtime libs.
g++ -O2 -std=c++17 ^
    src/Aircraft.cpp ^
    src/AirlineData.cpp ^
    src/AirlineReservationSystem.cpp ^
    src/Airport.cpp ^
    src/Crew.cpp ^
    src/DataLoader.cpp ^
    src/Flight.cpp ^
    src/FlightCalculator.cpp ^
    src/FlightPlan.cpp ^
    src/Order.cpp ^
    src/OrderManager.cpp ^
    src/Passenger.cpp ^
    src/PassengerUI.cpp ^
    src/Reservation.cpp ^
    src/Seat.cpp ^
    src/main.cpp ^
    -I include -o airline_reservation.exe ^
    -lcomctl32 -lgdi32 -luser32 ^
    -static -static-libgcc -static-libstdc++ ^
    -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic ^
    -s -mwindows

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation successful!
    echo executable created: airline_reservation.exe
    echo.
    echo Run the program to test:
    echo   1. Double-click airline_reservation.exe
    echo   2. Or run from command line: airline_reservation.exe
    echo.
) else (
    echo.
    echo Compilation failed! Please check the errors above.
    echo.
)

pause