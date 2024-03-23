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

  m->locked = 0;
  m->pid = 0;
}

void mrelease(mutex *m)
{
  acquire(&m->lk);
  m->locked = 0;
  m->pid = 0;
  wakeup(m);
  release(&m->lk);
}

void macquire(mutex *m)
{
  acquire(&m->lk);
  while (m->locked)
  {
    sleep(m, &m->lk);
  }
  m->locked = 1;
  m->pid = myproc()->pid;
  release(&m->lk);
}