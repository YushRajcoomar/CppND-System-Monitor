#include <vector>
#include "processor.h"
#include "linux_parser.h"
#include "unistd.h"


long Processor::CurrentTotal() { return LinuxParser::Jiffies(); }
long Processor::CurrentActive() { return LinuxParser::ActiveJiffies(); }
long Processor::CurrentIdle() { return LinuxParser::IdleJiffies(); }
long Processor::PrevTotal() { return m_total; }
long Processor::PrevActive() { return m_active; }
long Processor::PrevIdle() { return m_idle; }
void Processor::Update(long idle, long active, long total) {
  m_idle = idle;
  m_active = active;
  m_total = total;
}

float Processor::Utilization() {
    // util = active/total * 100

    long total_new = CurrentTotal();
    long active_new = CurrentActive();
    long idle_new = CurrentIdle();

    long total_old = PrevTotal();
    long idle_old = PrevIdle();

    Update(idle_new, active_new, total_new);

      float totalDelta = float(total_new) - float(total_old);
      float idleDetla = float(idle_new) - float(idle_old);

     float utilization = (totalDelta - idleDetla) / totalDelta;
  return utilization;
}