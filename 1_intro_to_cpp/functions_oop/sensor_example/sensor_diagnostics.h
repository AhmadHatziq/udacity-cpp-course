#ifndef SENSOR_DIAGNOSTICS_H // Header guards. Prevent multiple inclusion of this header
#define SENSOR_DIAGNOSTICS_H // Header guards 

#include <string> // Required for std::string used in function declarations

// Global error counter shared across translation units (defined in sensor_diagnostics.cpp)
extern int totalErrors; // Tells compiler that this var exists somewhere and is shared 

// Runs diagnostics for a given sensor and increments the global error counter.
// sensorName: name of the sensor (e.g., "IMU")
// localErrors: number of errors to simulate for this sensor
void runSensorDiagnostics(const std::string& sensorName, int localErrors); // Exports the function runSensorDiagnostics so main.cpp can use it 

#endif // SENSOR_DIAGNOSTICS_H. Marks end of header guard 