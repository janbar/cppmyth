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

#ifndef MYTHSHAREDPTR_H
#define	MYTHSHAREDPTR_H

#include "mythintrinsic.h"

#include <cstddef>  // for NULL

namespace Myth
{

  template<class T>
  class shared_ptr
  {
  public:

    shared_ptr() : p(NULL), c(NULL) { }

    explicit shared_ptr(T* s) : p(s), c(NULL)
    {
      if (p != NULL)
      {
        c = new IntrinsicCounter(1);
      }
    }

    shared_ptr(const shared_ptr& s) : p(s.p), c(s.c)
    {
      if (c != NULL)
        if (c->Increment() < 2)
        {
          c = NULL;
          p = NULL;
        }
    }

    shared_ptr& operator=(const shared_ptr& s)
    {
      if (this != &s)
      {
        reset();
        p = s.p;
        c = s.c;
        if (c != NULL)
          if (c->Increment() < 2)
          {
            c = NULL;
            p = NULL;
          }
      }
      return *this;
    }

    ~shared_ptr()
    {
      reset();
    }

    void reset()
    {
      if (c != NULL)
        if (c->Decrement() == 0)
        {
            delete p;
            delete c;
        }
      c = NULL;
      p = NULL;
    }

    void reset(T* s)
    {
      if (p != s)
      {
        reset();
        if (s != NULL)
        {
          p = s;
          c = new IntrinsicCounter(1);
        }
      }
    }

    T *get() const
    {
      return (c != NULL) ? p : NULL;
    }

    void swap(shared_ptr<T>& s)
    {
      T *tmp_p = p;
      IntrinsicCounter *tmp_c = c;
      p = s.p;
      c = s.c;
      s.p = tmp_p;
      s.c = tmp_c;
    }

    unsigned use_count() const
    {
      return (unsigned) (c != NULL ? c->GetValue() : 0);
    }

    T *operator->() const
    {
      return get();
    }

    T& operator*() const
    {
      return *get();
    }

    operator bool() const
    {
      return p != NULL;
    }

    bool operator!() const
    {
      return p == NULL;
    }

  protected:
    T *p;
    IntrinsicCounter *c;
  };

}

#endif	/* MYTHSHAREDPTR_H */
