// Sleeping locks

#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "mutex.h"
#include "param.h"

void initsleeplock(struct sleeplock *lk, char *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

void acquiresleep(struct sleeplock *lk)
{
  acquire(&lk->lk);
  while (lk->locked)
  {
    sleep(lk, &lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

void releasesleep(struct sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

int holdingsleep(struct sleeplock *lk)
{
  int r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  return r;
}

void sys_macquire(void)
{

  mutex *m;
  if (argptr(0, (void *)&m, sizeof(struct mutex)) < 0)
    return; // failure
  macquire(m);
}

void sys_mrelease()
{
  mutex *m;
  if (argptr(0, (void *)&m, sizeof(struct mutex)) < 0)
    return; // failure

  mrelease(m);
}

void sys_minit()
{
  mutex *m;
  if (argptr(0, (void *)&m, sizeof(struct mutex)) < 0)
    return; // failure

  minit(m);
}

void minit(mutex *m)
{
  initlock(&m->lk, "sleep lock");
  m->numWaiters = 0;
  m->nice = 21;
  m->locked = 0;
  m->pid = 0;
  for (int i = 0; i < NPROC; i++)
  {
    m->waiterPids[i] = 0;
  }
}

void mrelease(mutex *m)
{
  acquire(&m->lk);

  m->locked = 0;
  m->pid = 0;
  m->nice = 21;
  setProcNice(m->waiterPids); // this sets the locknice value of the processes waiting on this lock to the nice value
  m->numWaiters = 0;
  for (int i = 0; i < NPROC; i++)
  {
    m->waiterPids[i] = 0;
  }
  myproc()->lockNice = myproc()->nice; // set the locknice value of the process releasing the lock to its nice value
  wakeup(m);
  release(&m->lk);
}

void macquire(mutex *m)
{
  acquire(&m->lk);
  while (m->locked) // if the lock is already locked, sleep
  {
    if (m->nice > myproc()->nice) // if the lock is held by a process with a higher nice value than this process (so lower priority)
    {
      m->nice = myproc()->nice;                       // update the lock's nice value to the current process's nice value
      m->waiterPids[m->numWaiters++] = myproc()->pid; // add the current process to the list of waiters;
      struct proc *p = getproc(m->pid);               // get the process holding this lock
      if (m->nice < p->lockNice)
        p->lockNice = m->nice; // update the lock niceness value of the process holding the lock to be the same as the process trying to acquire the lock
      p->niceChanged = 1;      // indicate that locknice has been changed
    }

    sleep(m, &m->lk);
  }
  m->locked = 1;            // lock acquired
  m->nice = myproc()->nice; // set lock's nice value to the process's nice value
  m->pid = myproc()->pid;   // set lid's pid to the process's pid
  release(&m->lk);          // release lock's lock (see mutex.h for our implementation and sleeplock.h for more the original implementation)
}

int sys_nice(void)
{
  int inc;
  if (argint(0, &inc) < 0)
    return -1;
  return nice(inc);
}

int nice(int inc) // increment the nice value of the process
{
  struct proc *p = myproc();
  int new = p->nice + inc;

  if (new < -20) // cap new niceness in range [-20, 19]
    new = -20;
  if (new > 19)
    new = 19;

  p->nice = new;
  if (p->niceChanged == 0) // check to see if the lock nice has been changed, if not keep it line with the process nice
  {
    p->lockNice = new;
  }
  return 0;
}