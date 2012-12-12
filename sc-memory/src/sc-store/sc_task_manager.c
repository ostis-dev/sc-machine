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
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include "sc_thread.h"
#include "sc_task_manager.h"
#include "sc_storage.h"
#include "sc_types.h"
#include "sc_defines.h"
#include "sc_static_async_queue.h"

/**
 * @brief Name for thread where tasks are executed
 */
sc_bool sc_task_manager_need_shutdown = SC_FALSE;
sc_bool task_manager_is_initialized = SC_FALSE;
static sc_static_async_queue* sc_task_g_queue;
static sc_static_async_queue* sc_task_free;

/**
 * @brief Thread handler for task
 */
void sc_task_handler();

sc_bool sc_task_manager_initialize()
{
    if(task_manager_is_initialized)
    {
        g_error("Trying reinit task manager");
        return SC_FALSE;
    }

    SC_TASK_QUEUE_CREATE(sc_task_g_queue, TASK_QUEUE_SIZE)
    SC_TASK_QUEUE_CREATE(sc_task_free, TASK_QUEUE_SIZE)

    g_assert(sc_task_g_queue != NULL);
    g_assert(sc_task_free != NULL);

    SC_THREAD_INIT();
    SC_THREAD_NEW(sc_task_handler);
    SC_THREAD_NEW(sc_task_handler);

    int i = 0;
    for(i = 0; i< TASK_QUEUE_SIZE; i++)
    {
        SC_TASK_QUEUE_ADD_TASK(sc_task_free, sc_task_manager_private_create_empty());
    }

    task_manager_is_initialized = SC_TRUE;

    return SC_TRUE;
}

sc_bool sc_task_manager_is_initialized()
{
    return task_manager_is_initialized;
}

void sc_task_manager_shutdown()
{
    sc_task_manager_need_shutdown = SC_TRUE;
    return;
}

void sc_task_handler()
{
    while(!sc_task_manager_need_shutdown)
    {
        SC_TASK_QUEUE_POP_TASK(sc_task_g_queue, task);

        SC_MUTEX_LOCK(task->mutex);

        (task->func)(task->data);

        SC_MUTEX_COMPLITE_WAIT(task->mutex);

        SC_MUTEX_UNLOCK(task->mutex);
    }
}

sc_task* sc_task_manager_new(TaskFunc func, sc_task_type type, sc_int8 count, ...)
{
    SC_TASK_QUEUE_POP_TASK(sc_task_free, task);

    task->func = func;
    task->type = type;

    va_list args;
    va_start(args, count);

    int i;
    for(i = 0; i < count; ++i)
      task->data[i] = va_arg(args, void*);
    va_end(args);

    return task;
}

sc_task* sc_task_manager_private_create_empty()
{
    sc_task* task;
    task = g_malloc(sizeof(sc_task) + TASK_MAXIMUM_AMOUNT*sizeof(void*));

    if(task == NULL)
    {
        g_error("Warning: Can'not allocate memmory for task");
        return NULL;
    }

    SC_MUTEX_NEW(task->mutex);

    return task;

}

void sc_task_manager_free(sc_task* task)
{
    g_assert(task != NULL);

    SC_TASK_QUEUE_ADD_TASK(sc_task_free, task);
}

void sc_task_manager_execute(sc_task* task)
{
    SC_MUTEX_LOCK(task->mutex);

    SC_TASK_QUEUE_ADD_TASK(sc_task_g_queue, task);

    SC_MUTEX_WAIT(task->mutex);

    SC_MUTEX_UNLOCK(task->mutex);

    SC_MUTEX_CLEAR(task->mutex);
}

void sc_task_wrapper_is_element(void* args[])
{
    sc_bool* result = (sc_bool*) args[0];
    sc_addr* addr   = (sc_addr*) args[1];

    (*result) = sc_storage_is_element(*addr);
}

void sc_task_wrapper_element_free(void* args[])
{
    sc_result* result   = (sc_result*) args[0];
    sc_addr* addr       = (sc_addr*) args[1];

    (*result) = sc_storage_element_free(*addr);
}

void sc_task_wrapper_node_new(void* args[])
{
    sc_addr* result = (sc_addr*) args[0];
    sc_type* addr   = (sc_type*) args[1];

    (*result) = sc_storage_node_new(*addr);
}

void sc_task_wrapper_link_new(void* args[])
{
    sc_addr* result = (sc_addr*) args[0];

    (*result) = sc_storage_link_new();
}

void sc_task_wrapper_arc_new(void* args[])
{
    sc_addr* result = (sc_addr*) args[0];
    sc_type* type   = (sc_type*) args[1];
    sc_addr* beg    = (sc_addr*) args[2];
    sc_addr* end    = (sc_addr*) args[3];

    (*result) = sc_storage_arc_new(*type, *beg, *end);
}

void sc_task_wrapper_get_element_type(void* args[])
{
    sc_result* result   = (sc_result*) args[0];
    sc_addr* addr       = (sc_addr*) args[1];

    (*result) = sc_storage_get_element_type(*addr, args[2]);
}

void sc_task_wrapper_get_arc_begin(void* args[])
{
    sc_result* result   = (sc_result*) args[0];
    sc_addr* addr       = (sc_addr*) args[1];

    (*result) = sc_storage_get_arc_begin(*addr, args[2]);
}

void sc_task_wrapper_get_arc_end(void* args[])
{
    sc_result* result   = (sc_result*) args[0];
    sc_addr* addr       = (sc_addr*) args[1];

    (*result) = sc_storage_get_arc_end(*addr, args[2]);
}

void sc_task_wrapper_set_link_content(void* args[])
{
    sc_result* result   = (sc_result*) args[0];
    sc_addr* addr       = (sc_addr*) args[1];

    (*result) = sc_storage_set_link_content(*addr, args[2]);
}

void sc_task_wrapper_find_links_with_content(void* args[])
{
    sc_result* result   = (sc_result*) args[0];

    (*result) = sc_storage_find_links_with_content(args[1], args[2], args[3]);
}
