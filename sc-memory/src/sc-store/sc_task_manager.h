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

#ifndef _sc_task_manager_h_
#define _sc_task_manager_h_

#include "sc_types.h"
#include "sc_stream.h"
#include "sc_thread.h"
#include <glib.h>

enum _sc_task_type
{
    task_is_initialized,
    task_is_element,
    task_element_free,
    task_node_new,
    task_link_new,
    task_arc_new,
    task_get_element_type,
    task_get_arc_begin,
    task_get_arc_end,
    task_set_link_content,
    task_find_links_with_content
};
typedef enum _sc_task_type sc_task_type;

typedef void (*TaskFunc)(void*[]);

struct _sc_task
{
    TaskFunc func;

    sc_task_type type;
    sc_mutex* mutex;
    void* data[];
};
typedef struct _sc_task sc_task;

/**
 * @brief Initialize of separateted thread for task execution
 * @return Result of initialize. SC_TRUE if task manager initialize successfully, otherwise SC_FALSE
 */
sc_bool sc_task_manager_initialize();

sc_bool sc_task_manager_is_initialized();

/**
 * @brief Turn of task manager thread
 */
void sc_task_manager_shutdown();

/**
 * @brief Create new task
 * @param func Wrapper function
 * @param type Task type
 * @param count Count of arguments
 * @return Created task
 */
sc_task* sc_task_manager_new(TaskFunc func, sc_task_type type, sc_int8 count, ...);

/**
 * @brief Free memmory block of task
 * @param task
 */
void sc_task_manager_free(sc_task* task);

/**
 * @brief Add task to task manager. Thread-safe.
 * Use mutex circular list mutexes for lock
 * @param task
 */
void sc_task_manager_execute(sc_task* task);

/**
 * \defgroup Task wrapper
 * @{
 */
void sc_task_wrapper_is_element(void* args[]);
void sc_task_wrapper_element_free(void* args[]);
void sc_task_wrapper_node_new(void* args[]);
void sc_task_wrapper_link_new(void* args[]);
void sc_task_wrapper_arc_new(void* args[]);
void sc_task_wrapper_get_element_type(void* args[]);
void sc_task_wrapper_get_arc_begin(void* args[]);
void sc_task_wrapper_get_arc_end(void* args[]);
void sc_task_wrapper_set_link_content(void* args[]);
void sc_task_wrapper_find_links_with_content(void* args[]);
/**
 * @}
 */
#endif // _sc_task_manager_h_
