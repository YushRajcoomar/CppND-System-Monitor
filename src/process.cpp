#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;


Process::Process(int pid){
    pid_ = pid;
    user_ = LinuxParser::User(pid);
    command_ = LinuxParser::Command(pid);
    ram_ = LinuxParser::Ram(pid);
    uptime_ = LinuxParser::UpTime(pid);

}

// TODO: Return this process's ID
int Process::Pid() {
    return Process::pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    const int HERTZ = sysconf(_SC_CLK_TCK);
    float total_time = LinuxParser::UpTime();
    float proc_time = LinuxParser::ActiveJiffies(pid_);
    long seconds = total_time - (proc_time/HERTZ);
    cpu_ = ((proc_time / HERTZ) /seconds) * 100;

    return cpu_;
}

// TODO: Return the command that generated this process
string Process::Command() {
    return command_;
}

// TODO: Return this process's memory utilization
string Process::Ram() {
    long ram_kb = std::atol(ram_.c_str());
    long ram_mb = ram_kb/1024;
    return to_string(ram_mb);
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
   return user_;
 }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return uptime_;
  }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return this->cpu_ < a.cpu_;
}

bool Process::operator>(Process const& a) const {
    return this->cpu_ > a.cpu_;
}