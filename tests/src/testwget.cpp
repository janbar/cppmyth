#if (defined(_WIN32) || defined(_WIN64))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#include <WinSock2.h>
#include <Windows.h>
#include <time.h>
//#define usleep(t) Sleep((DWORD)(t)/1000)
//#define sleep(t)  Sleep((DWORD)(t)*1000)
#else
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#endif

#include "private/wsresponse.h"
#include "private/debug.h"
#include "private/socket.h"

#include <string.h>
#include <cstdio>

#ifndef __WINDOWS__
static void signalHandler(int signal, siginfo_t * info, void * data)
{
  Myth::DBG(DBG_DEBUG, "signal %d catched\n", signal);
  (void)info;
  (void)data;
}

static bool catchSignal(int signal)
{
  struct sigaction act;
  memset(&act, '\0', sizeof(struct sigaction));
  act.sa_sigaction = &signalHandler;
  act.sa_flags |= SA_SIGINFO;
  sigemptyset(&act.sa_mask);
  return (sigaction(signal, &act, 0) == 0);
}
#endif

int main() {

  int ret = 0;
#ifdef __WINDOWS__
  //Initialize Winsock
  WSADATA wsaData;
  if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)))
    return ret;
#else
  catchSignal(SIGPIPE);
#endif /* __WINDOWS__ */

  const char* dest_url = "www.google.fr";

  Myth::DBGLevel(4);

  Myth::WSRequest req(dest_url, 80);
  req.RequestAcceptEncoding(true);
  req.RequestAccept(ws_ctype_to_str(WS_CTYPE_Any));
  req.RequestService("/", WS_METHOD_Get);
  Myth::WSResponse resp(req);
  if (resp.IsSuccessful())
  {
    int l = 0;
    char buf[500];
    while ((l = resp.ReadContent(buf, 500)) > 0) {
      fwrite(buf, l, 1, stdout);
    }
  }

  //out:
#ifdef __WINDOWS__
  WSACleanup();
#endif /* __WINDOWS__ */
  return ret;
}
