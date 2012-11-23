/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef _sc_thread_h_
#define _sc_thread_h_

#include <glib.h>
#include "sc_types.h"

typedef GMutex sc_private_mutex;
typedef GCond sc_private_cond;
typedef GThread sc_thread;

/*!
 * Describe waiting mutex.
 */
typedef struct _sc_mutex
{
    //! Flag of waiting
    sc_bool is_waiting;
    //! Mutex
    sc_private_mutex*   mtx;
    //! Waiting condition
    sc_private_cond*    cnd;
} sc_mutex;

#define  SC_MUTEX_LOCK(mutex)   g_mutex_lock(mutex->mtx)

#define  SC_MUTEX_UNLOCK(mutex) g_mutex_unlock(mutex->mtx)

#define  SC_MUTEX_FREE(mutex)   g_mutex_free(mutex->mtx); \
                                g_cond_free(mutex->cnd); \
                                g_free(mutex);

#define  SC_MUTEX_TRY_LOCK(mutex) g_mutex_trylock(mutex->mtx)

#define  SC_MUTEX_CLEAR(mutex)  mutex->is_waiting = SC_TRUE;
#define  SC_MUTEX_WAIT(mutex)   g_atomic_int_set(&mutex->is_waiting, SC_TRUE); \
                                while(g_atomic_int_get(&mutex->is_waiting)) \
                                { \
                                    g_cond_wait (mutex->cnd, mutex->mtx);\
                                }



#define  SC_MUTEX_COMPLITE_WAIT(mutex)  g_cond_signal(mutex->cnd); \
                                        g_atomic_int_set(&mutex->is_waiting, SC_FALSE);

#define SC_TASK_QUEUE_FREE(task)        g_async_queue_push(sc_task_free, task);
#define SC_TASK_QUEUE_CREATE(task)      sc_task* task = (sc_task*) g_async_queue_pop(sc_task_free);
#define SC_TASK_QUEUE_POP(task)         sc_task* task = (sc_task*) g_async_queue_pop(sc_task_g_queue);
#define SC_TASK_QUEUE_ADD(task)         g_async_queue_push(sc_task_g_queue, task);

#ifdef G_OS_WIN32
    #define SC_THREAD_NEW(func) g_thread_create((GThreadFunc) func, NULL, FALSE, NULL);

    #define SC_THREAD_INIT()    g_thread_init(NULL)

    #define SC_MUTEX_NEW(mutex) mutex = g_malloc(sizeof(sc_mutex)); \
                                mutex->mtx = g_mutex_new(); \
                                mutex->cnd = g_cond_new(); \
                                mutex->is_waiting = SC_TRUE;
#endif

#ifdef G_OS_UNIX
    #define SC_THREAD_NEW(func) g_thread_new( "sc-task-thread", (GThreadFunc) func, NULL);

    // since version 2.32, the GLib threading system is automatically initialized at the start of program
    #define SC_THREAD_INIT()

    #define SC_MUTEX_NEW(mutex) mutex = g_malloc(sizeof(sc_mutex)); \
                                mutex->mtx = g_malloc(sizeof(sc_private_mutex)); \
                                g_mutex_init(mutex->mtx); \
                                mutex->cnd = g_malloc(sizeof(sc_private_cond)); \
                                g_cond_init(mutex->cnd); \
                                mutex->is_waiting = SC_TRUE;
#endif

#endif // _sc_thread_h_
