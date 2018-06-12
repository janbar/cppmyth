#if (defined(_WIN32) || defined(_WIN64))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#include <winsock2.h>
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
#include <stdlib.h>
#include <signal.h>
#include <map>

#define MYTAG "[DEMO] "
#define BUFSZ 64000

// Container for MythTV channels indexed by channum
typedef std::multimap<std::string, Myth::ChannelPtr> channelMap_t;
channelMap_t channelMap;

// Load visible channels into the channelMap
bool loadChannels(const char * server)
{
  Myth::WSAPI wsapi(server, 6544, "");
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
void liveTVSpawn(const char * server, const char * chanNum)
{

#ifndef __WINDOWS__
  // Ignore SIGPIPE and close gracefully
  (void)signal(SIGPIPE, SIG_IGN);
#endif


  Myth::DBGLevel(MYTH_DBG_INFO);
  Myth::LiveTVPlayback lp(server, 6543);

  fprintf(stderr, MYTAG "INFO: spawning live TV\n");

  // Spawn will find the channels match this chanNum. Also we can prepare our
  // predefined set of channels.
  Myth::ChannelList chanList;
  channelMap_t::const_iterator it = channelMap.find(chanNum);
  while (it != channelMap.end())
  {
    chanList.push_back(it->second);
    ++it;
  }
  // Spawn Live TV
  if (lp.SpawnLiveTV(chanNum, chanList))
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
      if (r < BUFSZ)
        waitus *= 1.10f;
      else if (waitus > 100)
        waitus /= 1.10f;
      if (r < 0 || (r > 0 && fwrite(buf, 1, r, file) != r))
        break;
    }
    delete[] buf;
    //fclose(file);
    fprintf(stderr, MYTAG "INFO: stopping live TV\n");
    lp.StopLiveTV();
  }
  else
    fprintf(stderr, MYTAG "ERROR: channel %s is unavailable\n", chanNum);
}

int main(int argc, char** argv)
{
  int ret = 0;
#ifdef __WINDOWS__
  //Initialize Winsock
  WSADATA wsaData;
  if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)))
    return ret;
#endif /* __WINDOWS__ */

  if (argc > 2)
  {
    // Load all channels from the backend
    if (loadChannels(argv[1]))
      liveTVSpawn(argv[1], argv[2]);
    else
      fprintf(stderr, MYTAG "ERROR: cannot load channels from the backend %s\n", argv[1]);
  }
  else
    fprintf(stderr, MYTAG "USAGE: %s {backend ip or hostname} {channum}\n", argv[0]);

#ifdef __WINDOWS__
  WSACleanup();
#endif /* __WINDOWS__ */
  return ret;
}
