#include <vector>
#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    // util = active/total * 100
    long total_jiffies = LinuxParser::Jiffies();
    long active_jiffies = LinuxParser::ActiveJiffies();
    long idle_jiffies = LinuxParser::IdleJiffies();
    float cpu_util = (active_jiffies-idle_jiffies)/total_jiffies;
    return cpu_util;
}

std::vector<int> Pids(){
    return LinuxParser::Pids();
}