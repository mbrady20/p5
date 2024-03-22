// Long-term locks for processes
struct sleeplock
{
  uint locked;        // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  char *name; // Name of lock.
  int pid;    // Process holding lock
};

struct mutex
{
  uint locked;        // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  uint nice;          // Priority of the lock

  // For debugging:
  char *name; // Name of lock.
  int pid;    // Process holding lock
};