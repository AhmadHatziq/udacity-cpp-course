#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// Helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds[[maybe_unused]]) { 
    // Use rounding off and module to get the values 
    long hours = seconds / 3600; 
    long minutes = (seconds % 3600) / 60; 
    long secs = seconds % 60; 

    // Craft output string via stream 
    std::ostringstream stream;
    stream << std::setw(2) << std::setfill('0') << hours << ":"
        << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << secs;

    return stream.str(); 
}
