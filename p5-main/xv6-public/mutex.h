typedef struct mutex
{
  uint locked;        // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  uint nice;          // Priority of the lock

  // For debugging:
  int pid; // Process holding lock
} mutex;

void macquire(mutex *m);

void mrelease(mutex *m);

void minit(mutex *m);

int nice(int inc);