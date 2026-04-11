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
float Process::CpuUtilization() const { 
    int current_process_id = process_id; 
    float process_cpu_utilization = LinuxParser::ProcessCpuUtilization(current_process_id);
    return process_cpu_utilization; 
}

// Return the command that generated this process
string Process::Command() {
    // Calls the LinuxParser::Command function to get the command associated with this process
    string command = LinuxParser::Command(process_id);
    return command; 
}

// Return this process's memory utilization
// Uses KB units, as per the LinuxParser::Ram function
string Process::Ram() { 
    int current_process_id = Pid(); 
    string process_ram = LinuxParser::Ram(current_process_id);
    return process_ram;
}

// Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(Pid()); 
}

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime(Pid());
}

// Overload the "less than" comparison operator for Process objects
// Means: Should a come before b in a sorted list of Process objects?
bool Process::operator<(Process const& a) const { 
    return this->CpuUtilization() > a.CpuUtilization();
}