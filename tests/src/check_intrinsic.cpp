#include <iostream>

#include "include/testmain.h"

#include "local_config.h"
#include "private/os/threads/threadpool.h"
#include "private/os/threads/atomic.h"
#include "private/os/threads/latch.h"
#include "mythsharedptr.h"
#include "mythlocked.h"

Myth::OS::Atomic* g_counter = nullptr;

class WorkerInc : public Myth::OS::Worker
{
  virtual void Process()
  {
    for (int i = 0; i < 5000100; i++)
    {
      g_counter->increment();
    }
  }
};

class WorkerDec : public Myth::OS::Worker
{
  virtual void Process()
  {
    for (int i = 0; i < 5000000; i++)
    {
      g_counter->decrement();
    }
  }
};

TEST_CASE("Stress atomic counter")
{
  int val = 0;
  g_counter = new Myth::OS::Atomic(val);
  Myth::OS::ThreadPool pool(4);
  pool.Suspend();
  pool.Enqueue(new WorkerInc());
  pool.Enqueue(new WorkerDec());
  pool.SetKeepAlive(100);
  pool.Resume();
  unsigned ps;
  while ((ps = pool.Size()) > 0)
    usleep(100000);
  REQUIRE(g_counter->load() == (val+100));
  delete g_counter;
}

Myth::LockedNumber<int>* g_locked;

class WorkerLockInc : public Myth::OS::Worker
{
  virtual void Process()
  {
    for (int i = 0; i < 500100; i++)
    {
      g_locked->Add(1);
    }
  }
};

class WorkerLockDec : public Myth::OS::Worker
{
  virtual void Process()
  {
    for (int i = 0; i < 500000; i++)
    {
      g_locked->Sub(1);
    }
  }
};

TEST_CASE("Stress locked number")
{
  int val = 0;
  g_locked = new Myth::LockedNumber<int>(val);
  Myth::OS::ThreadPool pool(4);
  pool.Suspend();
  pool.Enqueue(new WorkerLockInc());
  pool.Enqueue(new WorkerLockDec());
  pool.SetKeepAlive(100);
  pool.Resume();
  unsigned ps;
  while ((ps = pool.Size()) > 0)
    usleep(100000);
  REQUIRE(g_locked->Load() == (val+100));
  delete g_locked;
}

Myth::OS::Latch g_latch(false);
Myth::shared_ptr<size_t> g_pointer;

class WorkerPtrClear : public Myth::OS::Worker
{
  virtual void Process()
  {
    for (int i = 0; i < 10000; i++)
    {
      g_latch.lock();
      g_pointer.reset(new size_t(i));
      g_latch.unlock();
      usleep(1);
    }
  }
};

class WorkerPtrCopy : public Myth::OS::Worker
{
  virtual void Process()
  {
    for (int i = 0; i < 10000; i++)
    {
      g_latch.lock_shared();
      Myth::shared_ptr<size_t> ptr(g_pointer);
      g_latch.unlock_shared();
      if (ptr)
        g_counter->increment();
      usleep(1);
    }
  }
};

TEST_CASE("Stress shared pointer")
{
  int val = 0;
  g_counter = new Myth::OS::Atomic(0);
  g_pointer.reset(new size_t(0));
  Myth::OS::ThreadPool pool(4);
  pool.Suspend();
  pool.Enqueue(new WorkerPtrCopy());
  pool.Enqueue(new WorkerPtrClear());
  pool.Enqueue(new WorkerPtrCopy());
  pool.Enqueue(new WorkerPtrCopy());
  pool.SetKeepAlive(100);
  pool.Resume();
  unsigned ps;
  while ((ps = pool.Size()) > 0)
  {
    std::cout << "Running: " << ps
              << " , copy count: " << g_counter->load()
              << std::endl;
    usleep(10000);
  }
  std::cout << "total copy count: " << g_counter->load()
            << std::endl;
  REQUIRE(g_counter->load() == 30000);
  delete g_counter;
}
