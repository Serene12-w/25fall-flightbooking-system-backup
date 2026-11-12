# Build and package FlightTicketSystem as a zip with source, icon, and data
param(
    [string]$OutName = "Flight Ticket System"
)

$ErrorActionPreference = 'Stop'

# Prefer absolute tool paths to avoid PATH issues
$GPP = 'C:/WinLibs/mingw64/bin/g++.exe'
$WINDRES = 'C:/WinLibs/mingw64/bin/windres.exe'
if (-not (Test-Path $GPP)) { throw "g++ not found at $GPP. Please adjust the path in scripts/make_release.ps1" }
if (-not (Test-Path $WINDRES)) { throw "windres not found at $WINDRES. Please adjust the path in scripts/make_release.ps1" }

# Paths
$root = Get-Location
$dist = Join-Path $root 'dist'
$release = Join-Path $root 'release'

# Clean dist
if (Test-Path $dist) { Remove-Item -Recurse -Force $dist }
New-Item -ItemType Directory -Path $dist | Out-Null

# Build resource and exe (static link)
if (Test-Path app.o) { Remove-Item app.o -Force }
& $WINDRES -O coff -i res/app.rc -o app.o
if ($LASTEXITCODE -ne 0) { throw "windres failed" }

& $GPP -g -std=c++17 `
    src/Aircraft.cpp src/AirlineData.cpp src/AirlineReservationSystem.cpp src/Airport.cpp src/Crew.cpp `
    src/DataLoader.cpp src/Flight.cpp src/FlightCalculator.cpp src/FlightPlan.cpp src/Order.cpp `
    src/OrderManager.cpp src/Passenger.cpp src/PassengerUI.cpp src/Reservation.cpp src/Seat.cpp src/main.cpp `
    app.o -I include -o FlightTicketSystem.exe -mwindows -static -static-libgcc -static-libstdc++ `
    -lwinpthread -lcomctl32 -lgdi32 -luser32
if ($LASTEXITCODE -ne 0) { throw "g++ failed" }

# Copy runtime artifacts
Copy-Item -Force "FlightTicketSystem.exe" $dist
# Data folder for runtime
if (Test-Path 'data') { Copy-Item -Recurse -Force 'data' (Join-Path $dist 'data') }

# Include source, headers, resources, and helpful configs
Copy-Item -Recurse -Force 'src' (Join-Path $dist 'src')
Copy-Item -Recurse -Force 'include' (Join-Path $dist 'include')
if (Test-Path 'res') { Copy-Item -Recurse -Force 'res' (Join-Path $dist 'res') }
if (Test-Path 'test/icon.ico') { New-Item -ItemType Directory -Force (Join-Path $dist 'test') | Out-Null; Copy-Item -Force 'test/icon.ico' (Join-Path $dist 'test/icon.ico') }
if (Test-Path '.vscode') { Copy-Item -Recurse -Force '.vscode' (Join-Path $dist '.vscode') }
if (Test-Path 'README.md') { Copy-Item -Force 'README.md' $dist }
if (Test-Path 'update_reports') { Copy-Item -Recurse -Force 'update_reports' (Join-Path $dist 'update_reports') }

# Create release zip
if (-not (Test-Path $release)) { New-Item -ItemType Directory -Path $release | Out-Null }
$zipPath = Join-Path $release ("$OutName.zip")
if (Test-Path $zipPath) { Remove-Item -Force $zipPath }
Compress-Archive -Path (Join-Path $dist '*') -DestinationPath $zipPath

Write-Host "Release created: $zipPath"
