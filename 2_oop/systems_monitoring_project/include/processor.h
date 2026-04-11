#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  long prevIdle{0};  // To store previous idle time for CPU utilization calculation
  long prevTotal{0}; // To store previous total time for CPU utilization calculation
};

#endif