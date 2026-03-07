#if (defined(_WIN32) || defined(_WIN64))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#include <WinSock2.h>
#include <Windows.h>
#include <time.h>
#define usleep(t) Sleep((DWORD)(t)/1000)
#define sleep(t)  Sleep((DWORD)(t)*1000)
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <mythlivetvplayback.h>
#include <mythwsapi.h>
#include <mythdebug.h>

#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <signal.h>
#include <map>

#define MYTAG "[DEMO] "
#define BUFSZ 64000

struct
{
  std::string   host;
  unsigned      proto_port;
  unsigned      wsapi_port;
  std::string   pin;
  std::string   user_name;
  std::string   password;
  bool          ssl;
  std::string   channum;
} config;

// Container for MythTV channels indexed by channum
typedef std::multimap<std::string, Myth::ChannelPtr> channelMap_t;
channelMap_t channelMap;

// Load visible channels into the channelMap
bool loadChannels()
{
  Myth::WSAPI wsapi(config.host, config.wsapi_port, config.pin);
  wsapi.WithAuthorization(config.user_name, config.password)
       .WithSSL(config.ssl);

  if (wsapi.CheckService())
  {
    // Print the version of our backend
    Myth::VersionPtr versionPtr = wsapi.GetVersion();
    fprintf(stderr, MYTAG "MythTV backend version: %s\n", versionPtr->version.c_str());

    // Loop from MythTV sources
    Myth::VideoSourceListPtr sourceList = wsapi.GetVideoSourceList();
    for (Myth::VideoSourceList::const_iterator sourceIt = sourceList->begin(); sourceIt != sourceList->end(); ++sourceIt )
    {
      // Loop from MythTV channels from a source
      Myth::ChannelListPtr chanList = wsapi.GetChannelList((*sourceIt)->sourceId, true); // true for visible
      for (Myth::ChannelList::const_iterator chanIt = chanList->begin(); chanIt != chanList->end(); ++chanIt)
      {
        channelMap.insert(channelMap_t::value_type((*chanIt)->chanNum, *chanIt));
      }
    }
    return true;
  }
  return false;
}

// Spawn live TV and stream out
void liveTVSpawn()
{

#ifndef __WINDOWS__
  // Ignore SIGPIPE and close gracefully
  (void)signal(SIGPIPE, SIG_IGN);
#endif


  Myth::DBGLevel(MYTH_DBG_INFO);
  Myth::LiveTVPlayback lp(config.host, config.proto_port);

  fprintf(stderr, MYTAG "INFO: spawning live TV\n");

  // Spawn will find the channels match this chanNum. Also we can prepare our
  // predefined set of channels.
  Myth::ChannelList chanList;
  channelMap_t::const_iterator it = channelMap.find(config.channum);
  while (it != channelMap.end())
  {
    chanList.push_back(it->second);
    ++it;
  }
  // Spawn Live TV
  if (lp.SpawnLiveTV(config.channum, chanList))
  {
    const Myth::ProgramPtr prog = lp.GetPlayedProgram();
    fprintf(stderr, MYTAG "INFO: live TV is playing channel id %u from source id %u\n",
            prog->channel.chanId, prog->channel.sourceId);
    fprintf(stderr, MYTAG "INFO: program title is: %s\n", prog->title.c_str());
    //FILE* file = fopen("TMP.mpg", "wb");
    FILE* file = stdout;
    char* buf = new char[BUFSZ];
    unsigned int waitus = 100000; // 100ms
    int r;
    for (;;)
    {
      usleep(waitus);
      r = lp.Read(buf, BUFSZ);
      if (r < 0 || (r > 0 && fwrite(buf, 1, r, file) != r))
        break;
      // adjust the wait time
      if (r < BUFSZ)
        waitus *= 1.10f;
      else if (waitus > 5000) // 5ms
        waitus /= 1.10f;
    }
    delete[] buf;
    //fclose(file);
    fprintf(stderr, MYTAG "INFO: stopping live TV\n");
    lp.StopLiveTV();
    fprintf(stderr, "tick = %u usec\n", waitus);
  }
  else
    fprintf(stderr, MYTAG "ERROR: channel %s is unavailable\n", config.channum.c_str());
}

void mySigHandler(int sig)
{
  fprintf(stderr, MYTAG "INFO: signal %d ignored\n", sig);
}

void usage(const char * cmd)
{
  fprintf(stderr,"Usage:\n"
          "  --host=<IP>              The backend IP\n"
          "  --channum=<1>            The channel number to stream\n"
          "  --proto=<6543>           The protocol port     (6543)\n"
          "  --wsapi=<6544>           The service API port  (6544)\n"
          "  --ssl                    Enable SSL for API    (Required for port 6554)\n"
          "  --pin=<0000>             The security PIN      ('0000')\n"
          "  --username=<admin>       The user name         ('admin')\n"
          "  --password=<mythtv>      The user password     ('myhtv')"
          "\n");
  exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
  int ret = 0;
#ifdef __WINDOWS__
  //Initialize Winsock
  WSADATA wsaData;
  if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)))
    return ret;
#else
  (void)signal(SIGALRM, mySigHandler);
#endif /* __WINDOWS__ */

  config.host = "127.0.0.1";
  config.proto_port = 6543;
  config.wsapi_port = 6544;
  config.pin = "0000";
  config.user_name = "admin";
  config.password = "mythtv";
  config.ssl = false;
  config.channum = "1";

  if (argc > 1)
  {
    for (int i = 1; i < argc; ++i)
    {
      if (strncmp("--host=", argv[i], 7) == 0)
        config.host.assign(argv[i]+7);
      else if (strncmp("--channum=", argv[i], 10) == 0)
        config.channum.assign(argv[i]+10);
      else if (strncmp("--proto=", argv[i], 8) == 0)
        config.proto_port = atoi(argv[i]+8);
      else if (strncmp("--wsapi=", argv[i], 8) == 0)
        config.wsapi_port = atoi(argv[i]+8);
      else if (strncmp("--pin=", argv[i], 6) == 0)
        config.pin.assign(argv[i]+6);
      else if (strncmp("--username=", argv[i], 11) == 0)
        config.user_name.assign(argv[i]+11);
      else if (strncmp("--password=", argv[i], 11) == 0)
        config.password.assign(argv[i]+11);
      else if (strncmp("--ssl", argv[i], 6) == 0)
        config.ssl = true;
      else if (strncmp("--help", argv[i], 7) == 0)
        usage(argv[0]);
      else if (strncmp("-h", argv[i], 3) == 0)
        usage(argv[0]);
      else
      {
        fprintf(stderr,"Invalid argument (%s)\n", argv[i]);
        return EXIT_FAILURE;
      }
    }
  }
  else
  {
    usage(argv[0]);
  }

  // Load all channels from the backend
  if (loadChannels())
    liveTVSpawn();
  else
    fprintf(stderr, MYTAG "ERROR: cannot load channels from the backend %s\n", config.host.c_str());

#ifdef __WINDOWS__
  WSACleanup();
#endif /* __WINDOWS__ */
  return ret;
}
