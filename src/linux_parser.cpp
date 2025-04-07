#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"
#include "format.h"


using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
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
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  // Read and return the system memory utilization
  // ? Calculation : (Total memory - free memory) / Total Memory
  string line, key;
  float total = 1, free = 1;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "MemTotal:") {
        linestream >> total;
      } else if (key == "MemAvailable:") {
        linestream >> free;
        break;
      }
    }
  }

  return (total - free) / total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string value, token;
  long uptime_in_seconds;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
    if (filestream.is_open()) {
      filestream >> token >> value;
      filestream.close();
  }
  uptime_in_seconds = std::stol(value);
  return uptime_in_seconds;
}
// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies;
  jiffies = LinuxParser::CpuUtilization();

  
  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) +
         stol(jiffies[CPUStates::kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // i will assume that an idle jiffy is based on the system column in /proc/stat
  vector<string> jiffies;
  jiffies = LinuxParser::CpuUtilization();
  long num_jiffys = std::stol(jiffies[kIdle_]) + std::stol(jiffies[kIOwait_]);
  return num_jiffys;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
    string line, word;
    vector<string> words;
  
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (std::getline(filestream,line)) { //reads first line
      std::istringstream stream(line);
      stream >> word;
      while (stream >> word){
        words.push_back(word);
      }
    }
    return words;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string word,line,first_word,total_proc_str;
  int total_processes;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  while (filestream.is_open()){
    while(std::getline(filestream,line)){
      std::istringstream stream(line);
      stream >> word;
      if (word == "processes") {
        stream >> total_proc_str;
        total_processes = std::stoi(total_proc_str);
        filestream.close();
        break;
      }
    }
  }
  return total_processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string word,line,first_word,total_proc_str;
  int total_running_processes;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  while (filestream.is_open()){
    while(std::getline(filestream,line)){
      std::istringstream stream(line);
      stream >> word;
      if (word == "procs_running") {
        stream >> total_proc_str;
        total_running_processes = std::stoi(total_proc_str);
        filestream.close();
        break;
      }
    }
  }
  return total_running_processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line, val;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);

  while(std::getline(filestream, line)){
    std::stringstream stream(line);
    stream >> val;
    break;
  }
  return val;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  vector<string> proc_store;
  string line,val;

  // const int HERTZ = sysconf(_SC_CLK_TCK);
  // const long sys_uptime = UpTime();
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  while(std::getline(filestream,line)){
    std::stringstream stream(line);
    while (stream >> line){
      proc_store.push_back(line);
    }
  }

  string u_time = proc_store[13];
  string k_time = proc_store[14];
  string c_u_time = proc_store[15];
  string c_k_time = proc_store[16];

  string start_time = proc_store[21];

  long total_time = std::stol(u_time) + std::stol(k_time) +std::stol(c_u_time) + std::stol(c_k_time);
  // long seconds = sys_uptime - (std::stol(start_time)/HERTZ); 
  // long cpu_use = ((total_time / HERTZ) /seconds);
  return total_time;
}

// TODO: Read and return the cpu util used by a process
string LinuxParser::Cpu(int pid) {
  vector<string> proc_store;
  string line,val;

  const int HERTZ = sysconf(_SC_CLK_TCK);
  const long sys_uptime = UpTime();
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  while(std::getline(filestream,line)){
    std::stringstream stream(line);
    while (stream >> line){
      proc_store.push_back(line);
    }
  }

  string u_time = proc_store[13];
  string k_time = proc_store[14];
  string c_u_time = proc_store[15];
  string c_k_time = proc_store[16];

  string start_time = proc_store[21];

  int total_time = std::stoi(u_time) + std::stoi(k_time) +std::stoi(c_u_time) + std::stoi(c_k_time);
  long seconds = sys_uptime - (std::stol(start_time)/HERTZ); 
  long cpu_use = ((total_time / HERTZ) /seconds);
  return to_string(cpu_use);
}


// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, tag, val, ans;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  while(std::getline(filestream, line)){
    if (line[0] == 'V') {
      std::stringstream stream(line);
      stream >> tag >> val;
      if (tag == "VmSize:"){
        ans= val;
        break;
      }
    }
  }
  return ans;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, tag, ans;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  while(std::getline(filestream, line)){
      std::stringstream stream(line);
      stream >> tag;
      if (tag == "Uid:"){
        stream >> ans;
        break;
      }
  }
  return ans;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, ans, name, tag, val;
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);

  while(std::getline(filestream, line)){

    std::replace(line.begin(), line.end(), ':', ' ');
    std::stringstream stream(line);
    stream >> name >> val >> tag;
      if (tag == uid){
        ans = name;
        break;
      }
    }
  return ans;
  }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  vector<string> proc_store;
  string line,val;

  const int HERTZ = sysconf(_SC_CLK_TCK);
  const long sys_uptime = UpTime();
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  while(std::getline(filestream,line)){
    std::stringstream stream(line);
    while (stream >> line){
      proc_store.push_back(line);
    }
  }
  string start_time = proc_store[21];
  long seconds = sys_uptime - (std::stol(start_time)/HERTZ); 
  // string pid_uptime = Format::ElapsedTime(seconds);
  return seconds;
}
