#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "format.h"

using std::stof;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  // we can calculate memutil_pct with the below formula
  // mem_used/mem_avail where mem_used = mem_total - mem_free
  string mem_total,mem_free,mem_avail, mem_used;
  string line, val, token;
  float mem_avail_pct;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)) {
      char buffer_first_char = line[0];
      if (buffer_first_char == 'M'){
        std::replace(line.begin(), line.end(), ':', ' ');
        std::replace(line.begin(), line.end(), 'k', ' ');
        std::replace(line.begin(), line.end(), 'B', ' ');
        std::replace(line.begin(), line.end(), '"', ' ');

        std::istringstream linestream(line);
        linestream >> token >> val;
        if (val == "MemFree"){
          mem_free = val;
        }
        if (val == "MemTotal"){
          mem_total = val;
        }
        if (val == "MemAvailable"){
          mem_avail = val;
        }
      }
      else{
        filestream.close();
        break;
      }
      mem_avail_pct =  (std::stoi(mem_total) - std::stoi(mem_free)) / stoi(mem_avail);

    }
  }
  return mem_avail_pct;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string value;
  long uptime_in_seconds;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
    if (filestream.is_open()) {
      filestream >> value;
      filestream.close();
  }
  uptime_in_seconds = std::stol(value);
  return uptime_in_seconds;
}
// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  // /proc/stat is in USER_HZ, we assume the tick rate is like 100HZ for now
  // 100HZ means 100jiffys in a sec, 1 jiffy happens in 1/100 seconds
  // num_jiffys = time_jiffy/tick_rate
  string val, time_jiffy;
  string line;
  long num_jiffys;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (std::getline(filestream,line)) { //reads first line
    std::istringstream stream(line);
    stream >> val >> time_jiffy;
  }
  num_jiffys = std::stol(time_jiffy);
  return num_jiffys;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // i will assume that an active jiffy is based on the system column in /proc/stat
  string val, user_jiffy, sys_jiffy,line, word;
  string word;
  vector<string> words;
  long long_user_jiffy, long_sys_jiffy;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (std::getline(filestream,line)) { //reads first line
    std::istringstream stream(line);
    while (stream >> word){
      words.push_back(word);
    }
  }
  user_jiffy = words[1];
  sys_jiffy = words[3];
  long_user_jiffy = std::stol(user_jiffy);
  long_sys_jiffy = std::stol(sys_jiffy);
  return long_user_jiffy + long_sys_jiffy;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  // i will assume that an idle jiffy is based on the system column in /proc/stat
  string val, time_jiffy,line, word;
  string word;
  vector<string> words;
  long num_jiffys;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (std::getline(filestream,line)) { //reads first line
    std::istringstream stream(line);
    while (stream >> word){
      words.push_back(word);
    }
  }
  time_jiffy = words[4];
  num_jiffys = std::stol(time_jiffy);
  return num_jiffys;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
    string line, word;
    string word;
    vector<string> words;
  
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (std::getline(filestream,line)) { //reads first line
      std::istringstream stream(line);
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
    return val;
  }
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
  long cpu_use = 100 * ((total_time / HERTZ) /seconds);
  return to_string(cpu_use);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, tag, val;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  while(std::getline(filestream, line)){
    if (line[0] == 'U') {
      std::stringstream stream(line);
      stream >> tag >> val;
      if (tag == "Uid:"){
        return val;
      }
    }
  }
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, tag, val;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  while(std::getline(filestream, line)){
    if (line[0] == 'V') {
      std::stringstream stream(line);
      stream >> tag >> val;
      if (tag == "VmSize:"){
        return val;
      }
    }
  }
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, tag, val;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  while(std::getline(filestream, line)){
    if (line[0] == 'U') {
      std::stringstream stream(line);
      stream >> tag >> val;
      if (tag == "Uid:"){
        return val;
      }
    }
  }
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, name, tag, val;
  const string delim = ":";
  string uid = Uid(pid);
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  while(std::getline(filestream, line)){
    std::stringstream stream(line);
    
    std::getline(stream,name,':');
    std::getline(stream,tag,':');
    std::getline(stream,val,':');

      if (tag == uid){
        return name;
      }
    }
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
