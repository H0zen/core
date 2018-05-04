/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LOCKEDQUEUE_H
#define LOCKEDQUEUE_H

#include <ace/Guard_T.h>
#include <ace/Thread_Mutex.h>
#include <deque>
#include <assert.h>
#include "Errors.h"

namespace ACE_Based
{
    template <class T, class LockType, typename StorageType=std::deque<T> >
        class LockedQueue
    {
        //! Lock access to the queue.
        LockType _lock;

        //! Storage backing the queue.
        StorageType _queue;

        public:

            //! Create a LockedQueue.
            LockedQueue() : _lock(), _queue()
            {
            }

            //! Destroy a LockedQueue.
            virtual ~LockedQueue()
            {
            }

            //! Adds an item to the queue.
            void add(const T& item)
            {
                ACE_GUARD (LockType, g, this->_lock);
                _queue.push_back(item);
            }

            //! Gets the next result in the queue, if any.
            bool next(T& result)
            {
                ACE_GUARD_RETURN (LockType, g, this->_lock, false);

                if (_queue.empty())
                    return false;

                result = _queue.front();
                _queue.pop_front();

                return true;
            }

            template<class Checker>
            bool next(T& result, Checker& check)
            {
                ACE_GUARD_RETURN (LockType, g, this->_lock, false);

                if (_queue.empty())
                    return false;

                result = _queue.front();
                if(!check.Process(result))
                    return false;

                _queue.pop_front();
                return true;
            }

            bool empty_unsafe()
            {
                return _queue.empty();
            }

            ///! Checks if we're empty or not with locks held
            bool empty()
            {
                ACE_GUARD_RETURN (LockType, g, this->_lock, false);
                return _queue.empty();
            }
    };
}
#endif
