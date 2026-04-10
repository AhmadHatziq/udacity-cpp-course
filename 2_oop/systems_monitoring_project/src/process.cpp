#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Class constructor 
Process::Process(int pid){
    process_id = pid; 
}

// More efficient initializer list 
/*
// Assigns pid to the private member 'process_id' directly via initializer list 
Process::Process(int pid): process_id(pid) {}
*/

// Return this process's ID
int Process::Pid() { return process_id; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
    int current_process_id = Pid(); 
    float process_cpu_utilization = LinuxParser::ProcessCpuUtilization(current_process_id);
    return process_cpu_utilization; 
}

// Return the command that generated this process
string Process::Command() {
    // Calls the LinuxParser::Command function to get the command associated with this process
    string command = LinuxParser::Command(process_id);
    return command; 
}

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// TODO: Return the user (name) that generated this process
string Process::User() { return string(); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }