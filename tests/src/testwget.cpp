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
#endif

#include "private/wsresponse.h"
#include "private/debug.h"
#include "private/socket.h"

#include <string.h>
#include <cstdio>

int main() {

  int ret = 0;
#ifdef __WINDOWS__
  //Initialize Winsock
  WSADATA wsaData;
  if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)))
    return ret;
#endif /* __WINDOWS__ */

  const char* dest_url = "www.google.fr";

  Myth::DBGLevel(4);

  Myth::WSRequest req(dest_url, 80);
  req.RequestAcceptEncoding(true);
  req.RequestService("/", Myth::HRM_GET);
  Myth::WSResponse resp(req);
  if (resp.IsSuccessful())
  {
    int l = 0;
    char buf[500];
    while ((l = resp.ReadContent(buf, 500))) {
      fwrite(buf, l, 1, stdout);
    }
  }

  //out:
#ifdef __WINDOWS__
  WSACleanup();
#endif /* __WINDOWS__ */
  return ret;
}
