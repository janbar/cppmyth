#if defined _MSC_VER
#include <Windows.h>
#include <winsock.h>
#include <time.h>
#endif

#include <mythlivetvplayback.h>
#include <mythwsapi.h>
#include <mythdebug.h>

#include <cstdio>
#include <stdlib.h>
#include <signal.h>
#include <map>

#define MYTAG "[DEMO] "

// Container for MythTV channels
typedef std::map<unsigned, Myth::ChannelPtr> channelMap_t;
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
        channelMap[(*chanIt)->chanId] = *chanIt;
      }
    }
    return true;
  }
  return false;
}

// Spawn live TV and stream out
void liveTVSpawn(const char * server, Myth::ChannelPtr channelPtr)
{

#if !defined _MSC_VER
  // Ignore SIGPIPE and close gracefully
  (void)signal(SIGPIPE, SIG_IGN);
#endif


  Myth::DBGLevel(MYTH_DBG_INFO);
  Myth::LiveTVPlayback lp(server, 6543);

  fprintf(stderr, MYTAG "INFO: spawning live TV\n");
  if (lp.SpawnLiveTV(*channelPtr))
  {
    //FILE* file = fopen("TMP.mpg", "wb");
    FILE* file = stdout;
    char buf[64000];
    int r;
    for (;;)
    {
      r = lp.Read(buf, 64000);
      if (r < 0 || fwrite(buf, 1, r, file) != r)
        break;
    }
    //fclose(file);
    fprintf(stderr, MYTAG "INFO: stopping live TV\n");
    lp.StopLiveTV();
  }
}

int main(int argc, char** argv)
{
  int ret = 0;
#if defined _MSC_VER
  //Initialize Winsock
  WSADATA wsaData;
  if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)))
    return ret;
#endif /* _MSC_VER */

  if (argc > 2)
  {
    // Load all channels from the backend
    if (loadChannels(argv[1]))
    {
      // Find our channel by its 'chanid' and then spawn liveTV
      channelMap_t::const_iterator chanIt = channelMap.find(atoi(argv[2]));
      if (chanIt != channelMap.end())
        liveTVSpawn(argv[1], chanIt->second);
      else
        fprintf(stderr, MYTAG "ERROR: channel %s not found\n", argv[2]);
    }
    else
      fprintf(stderr, MYTAG "ERROR: cannot load channels from the backend %s\n", argv[1]);
  }
  else
    fprintf(stderr, MYTAG "USAGE: %s {backend ip or hostname} {chanid}\n", argv[0]);

#if defined _MSC_VER
  WSACleanup();
#endif /* _MSC_VER */
  return ret;
}
