#include <iostream>
#include "sensor_diagnostics.h"

int main() {
    std::cout << "=== Sensor System Boot ===\n";

    runSensorDiagnostics("IMU", 2);
    runSensorDiagnostics("Camera", 1);
    runSensorDiagnostics("LiDAR", 3);

    std::cout << "=== System Summary ===\n";
    std::cout << "Total system-wide errors: " << totalErrors << std::endl; // Uses global var

    // Shadowing the global variable with a local one
    int totalErrors = 999;
    std::cout << "Developer debug mode: local totalErrors = " << totalErrors << std::endl;

    return 0;
}
