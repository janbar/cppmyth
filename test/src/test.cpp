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

#include <mythdebug.h>
#include <mythtypes.h>
#include <mythcontrol.h>
#include <mytheventhandler.h>
#include <mythfileplayback.h>
#include <mythlivetvplayback.h>
#include <mythrecordingplayback.h>
#include <mythwsapi.h>

#include <cstdio>

int main(int argc, char** argv)
{
  int ret = 0;
#ifdef __WINDOWS__
  //Initialize Winsock
  WSADATA wsaData;
  if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)))
    return ret;
#endif /* __WINDOWS__ */

  std::string backendIP;
  Myth::ChannelPtr chantest;

  if (argc > 1)
    backendIP = argv[1];
  else
    backendIP = "127.0.0.1";

  {
    Myth::DBGLevel(MYTH_DBG_WARN);
    Myth::Control control(backendIP, 6543, 6544, "");

    fprintf(stderr, "\n***\n*** Testing web service GetSetting\n***\n");
    Myth::SettingPtr set = control.GetSetting("LiveTVPriority", true);
    if (set)
      fprintf(stderr,"setting: [%s] = %s\n", set->key.c_str(), set->value.c_str());

    fprintf(stderr, "\n***\n*** Testing web service GetSettings\n***\n");
    Myth::SettingMapPtr sets = control.GetSettings(true);
    for (Myth::SettingMap::iterator it = sets->begin(); it != sets->end(); ++it)
      fprintf(stderr,"setting: [%s] = %s\n", it->second->key.c_str(), it->second->value.c_str());

    fprintf(stderr, "\n***\n*** Testing web service GetRecordedList\n***\n");
    Myth::ProgramListPtr pl = control.GetRecordedList();
    for (Myth::ProgramList::const_iterator it = pl->begin(); it != pl->end(); ++it)
    {
      fprintf(stderr, "%s | %ld | %s | %s |", (*it)->fileName.c_str(), (long)(*it)->fileSize, (*it)->category.c_str(), (*it)->title.c_str());
      fprintf(stderr, " %s | %u\n", (*it)->recording.storageGroup.c_str(), (*it)->channel.chanId);
      for (std::vector<Myth::Artwork>::const_iterator ita = (*it)->artwork.begin(); ita != (*it)->artwork.end(); ++ita)
        fprintf(stderr, ">>>> Artwork: %s | %s\n", ita->fileName.c_str(), ita->type.c_str());
    }

    if (!pl->empty())
    {
      fprintf(stderr, "\n***\n*** Testing web service GetRecorded\n***\n");

      Myth::ProgramPtr prog = control.GetRecorded((*pl)[0]->channel.chanId, (*pl)[0]->recording.startTs);
      if (prog)
        fprintf(stderr, "%s | %lu | %s\n", prog->fileName.c_str(), prog->recording.startTs, prog->title.c_str());
    }

    fprintf(stderr, "\n***\n*** Testing web service GetCaptureCard\n***\n");

    Myth::CaptureCardListPtr card = control.GetCaptureCardList();
    for (Myth::CaptureCardList::const_iterator it2 = card->begin(); it2 != card->end(); ++it2)
      fprintf(stderr, "Card: %u | %s\n", (*it2)->cardId, (*it2)->cardType.c_str());

    fprintf(stderr, "\n***\n*** Testing web service GetVideoSource\n***\n");

    Myth::VideoSourceListPtr source = control.GetVideoSourceList();
    for (Myth::VideoSourceList::const_iterator it3 = source->begin(); it3 != source->end(); ++it3)
      fprintf(stderr, "Source: %u | %s\n", (*it3)->sourceId, (*it3)->sourceName.c_str());

    fprintf(stderr, "\n***\n*** Testing web service GetChannelList\n***\n");

    if (!source->empty())
    {
      Myth::ChannelListPtr channel = control.GetChannelList((*source)[0]->sourceId);
      for (Myth::ChannelList::const_iterator it4 = channel->begin(); it4 != channel->end(); ++it4)
        fprintf(stderr, "Channel: %u | %u | %s | %s | %s\n", (*it4)->sourceId, (*it4)->chanId,
                (*it4)->channelName.c_str(), (*it4)->chanNum.c_str(), (*it4)->iconURL.c_str());
      time_t now = time(NULL);

      fprintf(stderr, "\n***\n*** Testing web service GetProgramGuide\n***\n");
      for (unsigned ci = 0; ci < channel->size(); ++ci)
      {
        Myth::ProgramMapPtr epg = control.GetProgramGuide((*channel)[ci]->chanId, now, now+(24*3600));
        for (Myth::ProgramMap::const_iterator it5 = epg->begin(); it5 != epg->end(); ++it5)
        {
          fprintf(stderr, "Guide ChanId: %u : %lu ", (*channel)[ci]->chanId, it5->first);
          fprintf(stderr, "%lu | ", it5->second->endTime);
          fprintf(stderr, "%s | %s\n", it5->second->category.c_str(), it5->second->title.c_str());
        }
      }

      fprintf(stderr, "\n***\n*** Channel(WSAPI) / CardList(WSAPI) / CardInput(proto) \n***\n");

      if (!channel->empty())
      {
        chantest = (*channel)[0];

        Myth::CaptureCardListPtr clist = control.GetCaptureCardList();
        Myth::DBGLevel(MYTH_DBG_DEBUG);
        for (unsigned i = 0; i < clist->size(); ++i)
        {
          Myth::ProtoRecorder recorder((*clist)[i]->cardId, backendIP, 6543);
          recorder.IsTunable(*chantest);
        }
        Myth::DBGLevel(MYTH_DBG_WARN);
      }
    }

    fprintf(stderr, "\n***\n*** Testing protocol command: starting event handler\n***\n");
    Myth::EventHandler event(backendIP, 6543);
    event.Start();

    Myth::DBGLevel(MYTH_DBG_PROTO);

    if (!pl->empty())
    {
      fprintf(stderr, "\n***\n*** Testing protocol transfer\n***\n");
      int n = pl->size() - 1;

      Myth::RecordingPlayback pb(event);
      //Myth::RecordingPlayback pb(backendIP, 6543);
      //Myth::FilePlayback pb(backendIP, 6543);
      //pb.openTransfer(pl[n]->FileName, pl[n]->Recording.StorageGroup);
      pb.OpenTransfer((*pl)[n]);
      char buf[64000];
      int l = 0;
      for (int i = 0; i < 30; ++i)
      {
        if ((l = pb.Read(buf, 64000)) == 0)
          usleep(100000);
      }
      pb.CloseTransfer();

      fprintf(stderr, "\n***\n*** Testing protocol command queryGenpixmap\n***\n");
      control.QueryGenPixmap(*(*pl)[n]);
    }

    Myth::DBGLevel(MYTH_DBG_DEBUG);

    if (chantest)
    {
      fprintf(stderr, "\n***\n*** Testing protocol LiveTV for 1 min :channum %s Stream is TMP.mpg\n***\n", chantest->chanNum.c_str());

      Myth::LiveTVPlayback lp(event);
      if (lp.SpawnLiveTV(chantest))
      {
        FILE* file = fopen("TMP.mpg", "wb");
        char buf[64000];
        int r = 0;
        for (int i = 0; i < 200; ++i)
        {
          if ((r = lp.Read(buf, 64000)) == 0)
            usleep(100000);
          else if (r < 0)
            break;
          r = fwrite(buf, 1, r, file);
          if (i == 100)
            lp.KeepLiveRecording(true);
          if (i == 300)
            lp.KeepLiveRecording(false);
        }
        fclose(file);
        lp.StopLiveTV();
      }
    }
    event.Stop();

    Myth::DBGLevel(MYTH_DBG_ERROR);

    fprintf(stderr, "\n***\n*** Testing web services RecordSchedule\n***\n");
    unsigned proto = control.CheckService();
    Myth::RecordScheduleListPtr rl = control.GetRecordScheduleList();
    for (unsigned i = 0; i < rl->size(); ++i)
    {
      fprintf(stderr,"List existing record: rec %u | %s | %s | %d | %d | %d | %d\n",
              (*rl)[i]->recordId, (*rl)[i]->type.c_str(), (*rl)[i]->title.c_str(),
              Myth::RuleTypeFromString(proto, (*rl)[i]->type),
              Myth::DupInFromString(proto, (*rl)[i]->dupIn),
              Myth::DupMethodFromString(proto, (*rl)[i]->dupMethod),
              Myth::SearchTypeFromString(proto, (*rl)[i]->searchType));
    }

    if (false && chantest)
    {
      Myth::RecordSchedule rec = Myth::RecordSchedule();
      rec.chanId = chantest->chanId;
      rec.callSign = chantest->callSign;
      rec.title = "Testing LIB";
      rec.startTime = time(NULL) + 3600;
      rec.endTime = rec.startTime + 3600;
      rec.type_t = Myth::RT_SingleRecord;
      rec.searchType_t = Myth::ST_NoSearch;
      rec.dupMethod_t = Myth::DM_CheckNone;
      rec.dupIn_t = Myth::DI_InAll;
      rec.findDay = 0;
      rec.findTime = "00:00:00";

      Myth::DBGLevel(MYTH_DBG_DEBUG);
      control.AddRecordSchedule(rec);

      Myth::RecordSchedulePtr rc = control.GetRecordSchedule(rec.recordId);
      if (rc)
      {
        fprintf(stderr,"New record: rec %u | %s \n", rc->recordId, rc->title.c_str());
        Myth::DBGLevel(MYTH_DBG_DEBUG);
        fprintf(stderr,"Removing record %u returns %d\n", rc->recordId, control.RemoveRecordSchedule(rc->recordId));
        Myth::DBGLevel(MYTH_DBG_WARN);
      }
    }

    {
      Myth::ProgramListPtr ll = control.GetUpcomingList();
      for (unsigned i = 0; i < ll->size(); ++i)
      {
        fprintf(stderr,"List up comming: %s | rec %u | type %u\n", (*ll)[i]->title.c_str(), (*ll)[i]->recording.recordId, (*ll)[i]->recording.recType);
      }
    }
    {
      Myth::ProgramListPtr ll = control.GetConflictList();
      for (unsigned i = 0; i < ll->size(); ++i)
      {
        fprintf(stderr,"List conflict: %s |rec %u | type %u\n", (*ll)[i]->title.c_str(), (*ll)[i]->recording.recordId, (*ll)[i]->recording.recType);
      }
    }

    {
      Myth::ProgramListPtr ll = control.GetExpiringList();
      for (unsigned i = 0; i < ll->size(); ++i)
      {
        fprintf(stderr,"List expiring: %s | rec %u | type %u\n", (*ll)[i]->title.c_str(), (*ll)[i]->recording.recordId, (*ll)[i]->recording.recType);
      }
    }
  }

  //out:
#ifdef __WINDOWS__
  WSACleanup();
#endif /* __WINDOWS__ */
  return ret;
}
