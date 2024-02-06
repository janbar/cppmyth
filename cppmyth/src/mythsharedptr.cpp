/*
 *      Copyright (C) 2015 Jean-Luc Barriere
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

#include "local_config.h"
#include "private/os/threads/atomic.h"

using namespace Myth;

shared_ptr_base::shared_ptr_base()
: c(NULL)
, deleted(NULL) { }

shared_ptr_base::~shared_ptr_base()
{
  clear_counter();
  if (deleted != NULL)
    delete deleted;
}

shared_ptr_base::shared_ptr_base(const shared_ptr_base& s)
: c(s.c)
, deleted(NULL)
{
  /* handles race condition with clearing of s */
  if (c != NULL && c->add_fetch(1) == 1)
    c = NULL;
}

shared_ptr_base& shared_ptr_base::operator=(const shared_ptr_base& s)
{
  if (this != &s)
  {
    clear_counter();
    c = s.c;
    /* handles race condition with clearing of s */
    if (c != NULL && c->add_fetch(1) == 1)
      c = NULL;
  }
  return *this;
}

bool shared_ptr_base::clear_counter()
{
  if (c != NULL && c->sub_fetch(1) == 0)
  {
    /* delete later */
    if (deleted != NULL)
      delete deleted;
    deleted = c;
    c = NULL;
    return true;
  }
  c = NULL;
  return false;
}

void shared_ptr_base::reset_counter(int val)
{
  clear_counter();
  if (deleted != NULL)
  {
    c = deleted;
    deleted = NULL;
    c->store(val);
  }
  else
    c = new OS::Atomic(val);
}

void shared_ptr_base::swap_counter(shared_ptr_base& s)
{
  OS::Atomic* _c = c;
  c = s.c;
  s.c = _c;
}

int shared_ptr_base::get_counter() const
{
  return (c != NULL ? c->load() : 0);
}
