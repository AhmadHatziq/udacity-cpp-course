#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;

  // Read contents of the OS file 
  std::ifstream filestream(kOSPath); // kOSPath{"/etc/os-release"};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line); //input string steam 
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;

  // Read contents of the kernel file at "/proc/version"
  std::ifstream stream(kProcDirectory + kVersionFilename); // kProcDirectory{"/proc/"}; kVersionFilename{"/version"};
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    
    // Sample /proc/version file: // Linux version 6.6.87.2-microsoft-standard-WSL2 (root@439a258ad544) (gcc (GCC) 11.2.0, GNU ld (GNU Binutils) 2.37) #1 SMP PREEMPT_DYNAMIC Thu Jun  5 18:30:46 UTC 2025
    // Extract only the first 3 strings: Linux version <kernel>
    linestream >> os >> version >> kernel; 
  }
  return kernel;
}

/**
 * @brief Retrieves the list of active process IDs from the Linux files/process in `/proc`
 *
 * This function scans the /proc directory and identifies entries whose names
 * consist entirely of digits. In Linux, such directories correspond to running
 * processes and are named after their process IDs (PIDs).
 *
 * The function:
 * 1. Opens the /proc directory.
 * 2. Iterates through each entry using readdir().
 * 3. Checks if the entry is a directory.
 * 4. Verifies that the directory name contains only numeric characters.
 * 5. Converts the directory name to an integer PID.
 * 6. Adds the PID to a vector of process IDs.
 *
 * @return std::vector<int> A vector containing all detected process IDs.
 *
 * @note This implementation relies on POSIX directory APIs (opendir, readdir).
 *       It could alternatively be implemented using std::filesystem in modern C++.
 */
// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;

  // Directory pointer to the /proc directory
  DIR* directory = opendir(kProcDirectory.c_str()); // kProcDirectory{"/proc/"};
  struct dirent* file;

  while ((file = readdir(directory)) != nullptr) { // While file pointer is valid 
    // Is this a directory?
    if (file->d_type == DT_DIR) { // Checks for valid directory 
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) { // Checks if the filename consists of digits only
        int pid = stoi(filename); // Convert the filename (which is a string) to an integer PID
        pids.push_back(pid);
      }
    }
  }
  closedir(directory); // Close the directory stream to free resources
  return pids; // A vector of process IDs 
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { return 0.0; }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { return 0; }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return 0; }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
