#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

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
  // Object of type "ifstream", with var name "filestream". 
  // Equivalent tostd::ifstream filestream = std::ifstream(kOSPath);
  std::ifstream filestream(kOSPath); // kOSPath{"/etc/os-release"}; 
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {

      // Replace the seperator chars with space, so that ">>" can be used to parse the file.
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');

      std::istringstream linestream(line); //input string steam. Use the "line" string as the input for the stream, return to a var called "linestream"
      
      // Split by space
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
  // Create a var called "stream" of type "ifstream" and initialize it with the file at "/proc/version"
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
/**
 * Reads the /proc/meminfo file to calculate the sum and return the system's memory utilization as a float.
 */
float LinuxParser::MemoryUtilization() {
  
  float memTotal{0.0}; float memFree{0.0}; float memAvailable{0.0}; float memUtilization{0.0};
  string line; 

  // Get input file stream for the /proc/meminfo file
  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  /*
    Sample file contents: 
      MemTotal:        8090836 kB
      MemFree:         6288984 kB
  */

  if (stream.is_open()) {
    while (std::getline(stream, line)) { 
      std::string key, unit;
      float value;

      // Parse each line as they are in the format: <key>: <value> <unit>
      std::istringstream linestream(line);
      linestream >> key >> value >> unit;

      // Assign value based on key 
      if (key == "MemTotal:") {
        memTotal = value;
      } else if (key == "MemAvailable:") {
        memAvailable = value;
      } else if (key == "MemFree:") {
        memFree = value; // Record down this value just in case. 
      }

      // Early optimization: Break onces we have all 3 values 
      if (memTotal > 0 && memAvailable > 0 && memFree > 0) {
        break;
      }
    }
  }

  // Check for division by zero
  if (memTotal == 0) return 0.0;

  // Calculate memory utilization as (MemTotal - MemAvailable) / MemTotal
  memUtilization = (memTotal - memAvailable) / memTotal;
  return memUtilization; 
  }

// TODO: Read and return the system uptime
/**
 * Reads the /proc/uptime file to retrieve the system uptime in seconds. 
 * The file contains two numbers: 
 *  1. the first is the total uptime of the system
 *  2. the amount of idle time summed across all CPUs. 
 * Eg: $ cat /proc/uptime
       254.80 2955.02
 * Returns a long (double is truncated to long) representing the total uptime of the system in seconds.
 */
long LinuxParser::UpTime() { 
  double uptime{0}; 
  double idleTime{0};
  string line;

  // Get input file stream for the /proc/uptime file
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idleTime; // Extract the first two numbers from the file
  }

  return static_cast<long>(uptime);  
}

// TODO: Read and return the number of jiffies for the system
/**
 * Reads the /proc/stat file to calculate and return the total number of jiffies for the system.
 * Total jiffies = user + nice + system + idle + iowait + irq + softirq + steal (exclude guest, guestNice)
 */
long LinuxParser::Jiffies() {
  string line; 

  // Get input file stream for the /proc/stat file
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {

    // First line is in the format: cpu  4705 150 2290 1362393 234 0 120 0 0 0
    std::getline(stream, line);
    std::istringstream linestream(line);

    // Parse the line to extract jiffy values
    // Skip the first token ('cpu')
    linestream >> line; 
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guestNice;
    linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guestNice;
    return user + nice + system + idle + iowait + irq + softirq + steal; 
  }

  // Return 0 if the file cannot be opened or read successfully
  return 0; 
}

// TODO: Read and return the number of active jiffies for the system
/**
 * Reads the /proc/stat file to calculate and return the total number of active jiffies for the system.
 * Active jiffies are calculated as the sum of user, nice, system, irq, softirq, and steal jiffies.
 * Exclude guest, iowait and idle jiffies as they represent idle time.
 */
long LinuxParser::ActiveJiffies() {
  
  // Calls helper function to get a map of jiffies with keys
  auto j = CpuJiffiesMap();

  return j["user"] + j["nice"] + j["system"] + j["irq"] + j["softirq"] + j["steal"];
  }

// TODO: Read and return the number of idle jiffies for the system
/**
 * Reads the /proc/stat file to calculate and return the total number of idle jiffies for the system.
 * Idle jiffies are calculated as the sum of idle and iowait jiffies, which represent idle time.
 */
long LinuxParser::IdleJiffies() {
  // Calls helper function to get a map of jiffies with keys
  auto j = CpuJiffiesMap();

  return j["idle"] + j["iowait"];
}

// TODO: Read and return CPU utilization as a vector of strings
/*
 * Reads the /proc/stat file to calculate and return CPU utilization as a vector of strings.
*/
vector<string> LinuxParser::CpuUtilization() {
  string line; 
  vector<string> cpuUtilization;

  // Get input file stream for the /proc/stat file
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {

    // First line is in the format: cpu  4705 150 2290 1362393 234 0 120 0 0 0
    std::getline(stream, line);
    std::istringstream linestream(line);

    // Parse the line to extract jiffy values
    // Skip the first token ('cpu')
    linestream >> line; 
    
    // Order is in the form: user, nice, system, idle, iowait, irq, softirq, steal, guest, guestNice
    // Loop and get all the values as strings and push to the vector
    string value; 
    while (linestream >> value) {
      cpuUtilization.push_back(value);
    }

    return cpuUtilization;
  }

  // Return {} if the file cannot be opened or read successfully
  return {}; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
/**
 * In the file /proc/[pid]/stat, the 14th, 15th, 16th, and 17th values represent utime, stime, cutime, and cstime respectively.
 * Sums up these 4 values and returns as activie jiffies for the process. 
 */
long LinuxParser::ActiveJiffies(int pid) { 
  vector<string> values;

  // Convert argument pid to string to construct the file path 
  string pid_str = to_string(pid);
  std::ifstream filestream (kProcDirectory + pid_str + kStatFilename); // E.g. /proc/5/stat 

  // File is only 1 line. 
  if (filestream.is_open()) {
    string line;
    std::getline(filestream, line);
    std::istringstream linestream(line);

    // Loop through the line and extract all values as strings
    string value; 
    while (linestream >> value) {
      values.push_back(value);
    }

    // Note: This breaks if the process name (which is the 2nd value in the file and is enclosed in brackets) contains spaces.
    // Check if we have at least 17 values to avoid out of range error
    if (values.size() >= 17) {
      long utime = stol(values[13]); // 14th value is at index 13
      long stime = stol(values[14]); // 15th value is at index 14
      long cutime = stol(values[15]); // 16th value is at index 15
      long cstime = stol(values[16]); // 17th value is at index 16

      return utime + stime + cutime + cstime;
    }
  }

  // Return 0 if the file cannot be opened, read successfully, or does not contain enough values
  return 0; 
}

// TODO: Read and return the total number of processes
/**
 * Reads the /proc/stat file to find the line that starts with "processes" and returns the value as the total number of processes created since boot. 
 */
int LinuxParser::TotalProcesses() { 
  string line; 
  string key; 
  string value; 
  
  // Read input file stream for the /proc/stat file 
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {

      std::istringstream linestream(line); 

      // Split by space
      while (linestream >> key >> value) {

        // Retiurn the value if the key is "processes"
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0; 
}

// TODO: Read and return the number of running processes
/**
 * Reads the /proc/stat file to find the line that starts with "procs_running" and returns the value as the total number of running processes. 
 */
int LinuxParser::RunningProcesses() {
  string line; 
  string key; 
  string value; 
  
  // Read input file stream for the /proc/stat file 
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {

      std::istringstream linestream(line); 

      // Split by space
      while (linestream >> key >> value) {

        // Retiurn the value if the key is "procs_running"
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
/**
 * Returns the contents of the "/proc/[PID]/cmdline" file. 
 * Eg: $ cat /proc/10/cmdline 
 *    >> ~bash
 */
string LinuxParser::Command(int pid) { 
  string line; 
  string pid_str = to_string(pid);

  // Read input file stream for the /proc/[PID]/cmdline] file 
  std::ifstream stream(kProcDirectory + pid_str + kCmdlineFilename);

  if (stream.is_open()) {
    std::getline(stream, line); 
    return line;
  };

  // Return nothing if operation fails 
  return string(); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
/**
 * Returns the field 'VmRSS' from the file: /proc/[PID]/status
 */
string LinuxParser::Ram(int pid) { 
  string line; 
  string pid_str = to_string(pid);

  // PID Status file is at /prod/[PID]/status 
  std::ifstream stream(kProcDirectory + pid_str + kStatusFilename); 

  /*
    Sample file contents: 
      VmPeak:  4300488 kB
      VmSize:  4287616 kB
      VmLck:         0 kB
      VmRSS:    162540 k
  */

  // Iterate and extract the value with the key "VmRSS"
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      string key, unit, value; 
      
      // Extract the first 3 values, seperated by whitespaces 
      std::istringstream linestream(line); 
      linestream >> key >> value >> unit; 

      // Extract the key which contains "VmRss"
      if (key == "VmRSS:") {
        return value; 
      }
    }
  }

  return string(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
/**
 * Returns the field 'Uid' from the file: /proc/[PID]/status
 */
string LinuxParser::Uid(int pid) { 
  string line; 
  string pid_str = to_string(pid);

  // User ID is in the file /proc/[PID]/status 
  std::ifstream stream(kProcDirectory + pid_str + kStatusFilename); 

  /*
    Sample file contents: 
      Pid:    850
      PPid:   372
      TracerPid:      0
      Uid:    1000    1000    1000    1000
      Gid:    1000    1000    1000    1000
  */

  // Iterate and extract the value with the key "Uid"
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      string key, value; 
      
      // Extract the first 2 values, seperated by whitespaces 
      std::istringstream linestream(line); 
      linestream >> key >> value; 

      // Extract the key which contains "Uid:"
      if (key == "Uid:") {
        return value; 
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }

// Returns a map of CPU jiffies with keys: user, nice, system, idle, iowait, irq, softirq, steal, guest, guestNice
std::unordered_map<std::string, long> LinuxParser::CpuJiffiesMap() {
  std::unordered_map<std::string, long> jiffies;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    std::string line;
    std::getline(stream, line);

    std::istringstream linestream(line);

    std::string cpu;
    linestream >> cpu;  // skip "cpu"

    std::vector<std::string> keys = {
        "user", "nice", "system", "idle",
        "iowait", "irq", "softirq", "steal",
        "guest", "guestNice"
    };

    for (const auto& key : keys) {
      long value{0};
      linestream >> value;
      jiffies[key] = value;
    }
  }

  return jiffies;
}