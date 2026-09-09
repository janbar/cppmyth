/*
 *      Copyright (C) 2014-2026 Jean-Luc Barriere
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

#include "mythsharedptr.h"
#include "private/os/threads/atomic.h"

using namespace Myth;

shared_ptr_base::shared_ptr_base()
: pc(nullptr)
, spare(nullptr) { }

shared_ptr_base::~shared_ptr_base()
{
  reset_counter();
  if (spare != nullptr)
    delete spare;
}

shared_ptr_base::shared_ptr_base(const shared_ptr_base& s)
: pc(s.pc)
, spare(nullptr)
{
  if (pc != nullptr && pc->fetch_add(1) < 1)
    pc = nullptr;
}

shared_ptr_base& shared_ptr_base::operator=(const shared_ptr_base& s)
{
  if (this != &s)
  {
    pc = s.pc;
    if (pc != nullptr && pc->fetch_add(1) < 1)
      pc = nullptr;
  }
  return *this;
}

bool shared_ptr_base::reset_counter()
{
  if (pc != nullptr && pc->fetch_sub(1) == 1)
  {
    /* delete later */
    if (spare != nullptr)
      delete spare;
    spare = pc;
    pc = nullptr;
    return true;
  }
  pc = nullptr;
  return false;
}

void shared_ptr_base::renew_counter()
{
  if (spare != nullptr)
  {
    /* reuse the spare */
    spare->store(1);
    pc = spare;
    spare = nullptr;
  }
  else
  {
    /* create a new */
    pc = new OS::Atomic(1);
  }
}

void shared_ptr_base::swap_counter(shared_ptr_base& s)
{
  OS::Atomic* _pc = pc;
  pc = s.pc;
  s.pc = _pc;
}

int shared_ptr_base::get_count() const
{
  return (pc != nullptr ? pc->load() : 0);
}

