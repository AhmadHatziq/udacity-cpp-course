#include <vector>
#include <string>

#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
/**
 * CPU Utilization = Active Time / Total Time 
 * Active Time = Total Time - Idle Time
 * We need to take 2 snapshots of CPU times at two different points in time to calculate the change in active and total times. 
 * This allows us to compute the CPU utilization over that interval.
 * CPU Utilization = (Active Time at T2 - Active Time at T1) / (Total Time at T2 - Total Time at T1)
 */
float Processor::Utilization() { 
    
    // Get current CPU values 
    // Order is in the form: user, nice, system, idle, iowait, irq, softirq, steal, guest, guestNice
    std::vector<std::string> cpu_util_values = LinuxParser::CpuUtilization();

    // Check size of cpu_util_values to ensure we have the expected number of values (at least 8 for the relevant values)
    if (cpu_util_values.size() < 8) {
       throw std::runtime_error("Invalid CPU data from /proc/stat via LinuxParser::CpuUtilization()"); // Error here as not enough values to calculate CPU utilization
    }

    // Extract current values 
    long user = std::stol(cpu_util_values[0]);
    long nice = std::stol(cpu_util_values[1]);
    long system = std::stol(cpu_util_values[2]);
    long idle = std::stol(cpu_util_values[3]);
    long iowait = std::stol(cpu_util_values[4]);
    long irq = std::stol(cpu_util_values[5]);
    long softirq = std::stol(cpu_util_values[6]);
    long steal = std::stol(cpu_util_values[7]);

    // Calculate current times
    long currentIdle = idle + iowait;
    long currentTotal = user + nice + system + idle + iowait + irq + softirq + steal;
    long currentActive = currentTotal - currentIdle;

    // Check previous times. If 0, this is the first call and we cannot calculate utilization yet. Store current times and return 0.0. 
    if (prevTotal == 0 || prevIdle == 0) {
        prevIdle = currentIdle;
        prevTotal = currentTotal;
        return 0.0; 
    }

    // Calculate deltas
    long deltaIdle = currentIdle - prevIdle;
    long deltaTotal = currentTotal - prevTotal;
    long deltaActive = deltaTotal - deltaIdle;

    // Update previous times for the next call
    prevIdle = currentIdle;
    prevTotal = currentTotal;

    // Return value of CPU utilization. Check if deltaTotal is greater than 0 to avoid division by zero.
    if (deltaTotal == 0) {
        return 0.0;
    }

    float cpuUtilization = static_cast<float>(deltaActive) / static_cast<float>(deltaTotal);
    return cpuUtilization; 
}
