#include <iostream>
#include <string>
#include "sensor_diagnostics.h"

// Definition of the global variable declared as extern in the header
int totalErrors = 0;

// File-local variable (internal linkage) to control verbosity
static int log_level = 2;

void runSensorDiagnostics(const std::string& sensorName, int localErrors) {
    std::cout << "\nRunning diagnostics for " << sensorName << "...\n";

    // Local variable: per-sensor counter (local scope)
    int sensorErrorCount = 0;

    for (int i = 1; i <= localErrors; ++i) {
        ++sensorErrorCount;
        ++totalErrors; // update shared global count

        if (log_level >= 2) {
            std::cout << "[Level " << log_level << "] Error " << i
                      << ": Detected anomaly\n";
        }
    }

    std::cout << "Diagnostics complete. Errors found: " << sensorErrorCount << "\n";
}
