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
#include <stdlib.h>

#include "sc_memory.h"
#include "sc-store/sc_storage.h"
#include "sc-store/sc_task_manager.h"

#include <glib.h>

sc_bool sc_memory_initialize(const sc_char *repo_path)
{
    return sc_storage_initialize(repo_path);
}

void sc_memory_shutdown()
{
    sc_storage_shutdown();
}

sc_bool sc_memory_is_initialized()
{

    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_is_initialized();
    #endif
    return sc_storage_is_initialized();
}

sc_bool sc_memory_is_element(sc_addr addr)
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_is_element(addr);
    #endif

    sc_bool result = SC_FALSE;
//    void* arg1 = (void*) &result;
//    void* arg2 = (void*) &addr;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_is_element,
                task_is_element,
                2, &result, &addr);

    sc_task_manager_execute(task);

    sc_task_manager_free(task);
    return result;
}

sc_result sc_memory_element_free(sc_addr addr)
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_element_free(addr);
    #endif

#if DEBUG
    printf("sc_memory_memory_free\n");
#endif

    sc_result result;
//    void* arg1 = (void*) &result;
//    void* arg2 = (void*) &addr;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_element_free,
                task_element_free,
                2, &result, &addr);
    sc_task_manager_execute(task);

    sc_task_manager_free(task);
    return result;
}

sc_addr sc_memory_node_new(sc_type type)
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_node_new(type);
    #endif

#if DEBUG
    printf("sc_memory_node_new\n");
#endif

    sc_addr result;
//    void* arg1 = (void*) &result;
//    void* arg2 = (void*) &type;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_node_new,
                task_node_new,
                2, &result, &type);

    sc_task_manager_execute(task);
    sc_task_manager_free(task);

    return result;
}

sc_addr sc_memory_link_new()
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_link_new();
    #endif

#if DEBUG
    printf("sc_memory_link_new\n");
#endif
    sc_addr result;
//    void* arg1 = (void*) &result;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_link_new,
                task_link_new,
                1, &result);

    sc_task_manager_execute(task);
    sc_task_manager_free(task);

    return result;

}

sc_addr sc_memory_arc_new(sc_type type, sc_addr beg, sc_addr end)
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_arc_new(type, beg, end);
    #endif

#if DEBUG
    printf("sc_memory_arc_new\n");
#endif
    sc_addr result;
//    void* arg1 = (void*) &result;
//    void* arg2 = (void*) &type;
//    void* arg3 = (void*) &beg;
//    void* arg4 = (void*) &end;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_arc_new,
                task_arc_new,
                4, &result, &type, &beg, &end);

    sc_task_manager_execute(task);
    sc_task_manager_free(task);

    return result;

}

sc_result sc_memory_get_element_type(sc_addr addr, sc_type *result_type)
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_get_element_type(addr, result_type);
    #endif
    sc_result result;
//    void* arg1 = (void*) &result;
//    void* arg2 = (void*) &addr;
//    void* arg3 = (void*) &result_type;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_get_element_type,
                task_get_element_type,
                3, &result, &addr, &result_type);

    sc_task_manager_execute(task);
    sc_task_manager_free(task);

    return result;
}

sc_result sc_memory_get_arc_begin(sc_addr addr, sc_addr *result_addr)
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_get_arc_begin(addr, result_addr);
    #endif

    sc_result result;
//    void* arg1 = (void*) &result;
//    void* arg2 = (void*) &addr;
//    void* arg3 = (void*) &result_addr;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_get_arc_begin,
                task_get_arc_begin,
                3, &result, &addr, &result_addr);

    sc_task_manager_execute(task);
    sc_task_manager_free(task);

    return result;
}

sc_result sc_memory_get_arc_end(sc_addr addr, sc_addr *result_addr)
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_get_arc_end(addr, result_addr);
    #endif

    sc_result result;
//    void* arg1 = (void*) &result;
//    void* arg2 = (void*) &addr;
//    void* arg3 = (void*) &result_addr;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_get_arc_end,
                task_get_arc_end,
                3, &result, &addr, &result_addr);

    sc_task_manager_execute(task);
    sc_task_manager_free(task);

    return result;
}

sc_result sc_memory_set_link_content(sc_addr addr, const sc_stream *stream)
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_set_link_content(addr, stream);
    #endif

    sc_result result;
//    void* arg1 = (void*) &result;
//    void* arg2 = (void*) &addr;
//    void* arg3 = (void*) &stream;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_get_arc_end,
                task_get_arc_end,
                3, &result, &addr, &stream);

    sc_task_manager_execute(task);
    sc_task_manager_free(task);

    return result;
}

sc_result sc_memory_find_links_with_content(const sc_stream *stream, sc_addr **result_addr, sc_uint32 *result_count)
{
    #if !USE_TASK_MANAGER
    //! @todo make it thread-safe
    return sc_storage_find_links_with_content(stream, result_addr, result_count);
    #endif

    sc_result result;
//    void* arg1 = (void*) &result;
//    void* arg2 = (void*) &stream;
//    void* arg3 = (void*) &result_addr;
//    void* arg4 = (void*) &result_count;

    sc_task* task = sc_task_manager_new(
                &sc_task_wrapper_get_arc_end,
                task_get_arc_end,
                4, &result, &stream, &result_addr, &result_count);

    sc_task_manager_execute(task);
    sc_task_manager_free(task);

    return result;
}
