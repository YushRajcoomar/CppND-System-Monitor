#include <vector>
#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  long CurrentTotal();
  long CurrentIdle();
  long CurrentActive();

  long PrevTotal();
  long PrevIdle();
  long PrevActive();
  void Update(long idle, long active, long total);
  // TODO: Declare any necessary private members
 private:
 long m_total;
 long m_active;
 long m_idle;
};

#endif