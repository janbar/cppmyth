#if (defined(_WIN32) || defined(_WIN64))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#include <WinSock2.h>
#include <Windows.h>
#include <time.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include "local_config.h"
#include "private/os/threads/threadpool.h"
#include "private/os/threads/atomic.h"
#include "mythsharedptr.h"
#include <string>

#include <cstdio>
#include <stdlib.h>

Myth::OS::Atomic* g_counter = 0;

class WorkerInc : public Myth::OS::Worker
{
  virtual void Process()
  {
    for (int i = 0; i < 5000000; i++)
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

int main(int argc, char** argv)
{
  int val = 0;
  if (argc > 1)
    val = atoi(argv[1]);
  g_counter = new Myth::OS::Atomic(val);
  Myth::OS::ThreadPool pool(20);
  pool.Suspend();
  for (int i = 0; i < 5; ++i)
  {
    pool.Enqueue(new WorkerInc());
    pool.Enqueue(new WorkerDec());
  }
  pool.SetKeepAlive(100);
  pool.Resume();
  unsigned ps;
  while ((ps = pool.Size()) > 0)
  {
    printf("Running (%2u): counter=%d\n", ps, g_counter->load());
    usleep(10000);
  }
  printf("Completed   : counter=%d\n", g_counter->load());
  delete g_counter;
  return EXIT_SUCCESS;
}
