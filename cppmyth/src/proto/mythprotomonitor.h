/*
 *      Copyright (C) 2014 Jean-Luc Barriere
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301 USA
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifndef MYTHPROTOMONITOR_H
#define	MYTHPROTOMONITOR_H

#include "mythprotobase.h"
#include "mythprotorecorder.h"

#include <vector>

#define PROTO_MONITOR_RCVBUF      64000

namespace Myth
{

  class ProtoMonitor : public ProtoBase
  {
  public:
    ProtoMonitor(const std::string& server, unsigned port);
    ProtoMonitor(const std::string& server, unsigned port, bool frontend);

    virtual bool Open();
    virtual void Close();
    virtual bool IsOpen();

    ProtoRecorderPtr GetRecorderFromNum(int rnum)
    {
      return GetRecorderFromNum75(rnum);
    }
    bool QueryFreeSpaceSummary(int64_t *total, int64_t *used)
    {
      return QueryFreeSpaceSummary75(total, used);
    }
    std::string GetSetting(const std::string& hostname, const std::string& setting)
    {
      return GetSetting75(hostname, setting);
    }
    bool SetSetting(const std::string& hostname, const std::string& setting, const std::string& value)
    {
      return SetSetting75(hostname, setting, value);
    }
    bool QueryGenpixmap(const Program& program)
    {
      return QueryGenpixmap75(program);
    }
    bool DeleteRecording(const Program& program, bool force = false, bool forget = false)
    {
      return DeleteRecording75(program, force, forget);
    }
    bool UndeleteRecording(const Program& program)
    {
      return UndeleteRecording75(program);
    }
    bool StopRecording(const Program& program)
    {
      return StopRecording75(program);
    }
    bool CancelNextRecording(int rnum, bool cancel)
    {
      return CancelNextRecording75(rnum, cancel);
    }
    StorageGroupFilePtr QuerySGFile(const std::string& hostname, const std::string& sgname, const std::string& filename)
    {
      return QuerySGFile75(hostname, sgname, filename);
    }
    MarkListPtr GetCutList(const Program& program)
    {
      return GetCutList75(program);
    }
    MarkListPtr GetCommBreakList(const Program& program)
    {
      return GetCommBreakList75(program);
    }
    bool BlockShutdown()
    {
      return BlockShutdown75();
    }
    bool AllowShutdown()
    {
      return AllowShutdown75();
    }
    /**
     * @brief Asks the backend for a list of free inputs. A free input is one that is connected but not busy, not in a busy input group or is locked.
     * @param rnum Since protocol 87: Input to exclude (not counted as busy). Defaulted with 0 for none.
     * @return
     */
    CardInputListPtr GetFreeInputs(int rnum = 0)
    {
      if (m_protoVersion >= 91) return GetFreeInputs91(rnum);
      if (m_protoVersion >= 90) return GetFreeInputs90(rnum);
      if (m_protoVersion >= 89) return GetFreeInputs89(rnum);
      if (m_protoVersion >= 87) return GetFreeInputs87(rnum);
      if (m_protoVersion >= 81) return GetFreeInputs81();
      if (m_protoVersion >= 79) return GetFreeInputs79();
      return GetFreeInputs75();
    }

  private:
    bool m_frontend;

    bool Announce75();
    bool Announce88();
    ProtoRecorderPtr GetRecorderFromNum75(int rnum);
    bool QueryFreeSpaceSummary75(int64_t *total, int64_t *used);
    std::string GetSetting75(const std::string& hostname, const std::string& setting);
    bool SetSetting75(const std::string& hostname, const std::string& setting, const std::string& value);
    bool QueryGenpixmap75(const Program& program);
    bool DeleteRecording75(const Program& program, bool force, bool forget);
    bool UndeleteRecording75(const Program& program);
    bool StopRecording75(const Program& program);
    bool CancelNextRecording75(int rnum, bool cancel);
    StorageGroupFilePtr QuerySGFile75(const std::string& hostname, const std::string& sgname, const std::string& filename);
    MarkListPtr GetCutList75(const Program& program);
    MarkListPtr GetCommBreakList75(const Program& program);
    bool BlockShutdown75();
    bool AllowShutdown75();
    std::vector<int> GetFreeCardIdList75();
    CardInputListPtr GetFreeInputs75();
    CardInputListPtr GetFreeInputs79();
    CardInputListPtr GetFreeInputs81();
    CardInputListPtr GetFreeInputs87(int rnum);
    CardInputListPtr GetFreeInputs89(int rnum);
    CardInputListPtr GetFreeInputs90(int rnum);
    CardInputListPtr GetFreeInputs91(int rnum);

    // Not implemented
    //int64_t GetBookmark75(Program& program);
    //bool SetBookmark75(Program& program, int64_t bookmark);
    //bool UpdateSchedules75();
    //bool UpdateSchedules75(uint32_t recordid);
  };

}

#endif	/* MYTHPROTOMONITOR_H */
