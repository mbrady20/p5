#include "param.h"
typedef struct mutex
{
  uint locked;        // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  int nice;          // Priority of the lock

  // For debugging:
  int numWaiters; // Number of processes waiting for lock
  int waiterPids[NPROC]; // Pids of processes waiting for lock
  int pid; // Process holding lock
} mutex;

void macquire(mutex *m);

void mrelease(mutex *m);

void minit(mutex *m);

int nice(int inc);
