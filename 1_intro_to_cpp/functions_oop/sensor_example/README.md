# C++ Exercise: Sensor Diagnostics Tracker

You are building a diagnostic module for a vehicle's onboard system.  
Each sensor can report a number of errors, and your job is to track them properly using clean multi-file C++ structure.

## Objectives

Build a program split across multiple `.cpp` and `.h` files that:

- Tracks a global error count across the system (shared between files)
- Prints per-sensor diagnostics with local error counters
- Uses a file-local `log_level` with `static` (internal linkage)
- Structures everything using `extern`, proper header guards, and best practices

## Student Goals

- Define and update a global variable shared between files
- Write a function that uses local variables (local scope)
- Keep a variable private to one file using `static`
- Use header guards and `extern` declarations correctly in a header file

## Project Structure
sensor_diagnostics/
├── main.cpp
├── sensor_diagnostics.cpp
└── sensor_diagnostics.h


## Tasks

1. Declare a global variable `totalErrors` in the header using `extern`
2. Define `totalErrors` in `sensor_diagnostics.cpp`
3. Implement `runSensorDiagnostics(sensorName, localErrors)`:
   - Use a `for` loop to simulate printed errors per sensor
   - Increment the shared `totalErrors`
   - Use `static int log_level = 2;` inside the `.cpp` file to control verbosity
4. In `main.cpp`, call your function for three different sensors
5. Print a system summary using the global error count
6. Shadow the global `totalErrors` with a local variable in `main()` and demonstrate scope

## Sample Output
=== Sensor System Boot ===
Running diagnostics for IMU...
[Level 2] Error 1: Detected anomaly
[Level 2] Error 2: Detected anomaly
Diagnostics complete. Errors found: 2

Running diagnostics for Camera...
[Level 2] Error 1: Detected anomaly
Diagnostics complete. Errors found: 1

Running diagnostics for LiDAR...
[Level 2] Error 1: Detected anomaly
[Level 2] Error 2: Detected anomaly
[Level 2] Error 3: Detected anomaly
Diagnostics complete. Errors found: 3

=== System Summary ===
Total system-wide errors: 6


## Build Instructions

Compile with g++:

Make sure your build task includes both `.cpp` files.
g++ main.cpp sensor_diagnostics.cpp -o diagnostics
