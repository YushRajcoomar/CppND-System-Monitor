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

    std::string cpu_s = LinuxParser::Cpu(pid);
    cpu_ = std::stof(cpu_s);
    long seconds = LinuxParser::UpTime() - uptime_;
    long totaltime = LinuxParser::ActiveJiffies(pid);
    try {
        m_utilization = float(totaltime) / float(seconds);

    } catch (...) {
        m_utilization = 0;
    }
}

// TODO: Return this process's ID
int Process::Pid() {
    return Process::pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {

    return m_utilization;
}

// TODO: Return the command that generated this process
string Process::Command() {
    return command_;
}

// TODO: Return this process's memory utilization
string Process::Ram() {
    return ram_;
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
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    if (a.pid_ < Process::pid_){
        return true;
    }
    else {
        return false;
    }
}