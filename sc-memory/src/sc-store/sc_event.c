/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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

#include <glib.h>
#include "sc_event.h"

// Pointer to hash table that contains events
GHashTable *events_table = 0;

guint events_table_hash_func(gconstpointer pointer)
{
    const sc_addr *addr = (const sc_addr*)pointer;
    return SC_ADDR_LOCAL_TO_INT(*addr);
}

gboolean events_table_equal_func(gconstpointer a, gconstpointer b)
{
    const sc_addr *addr1 = (const sc_addr*)a;
    const sc_addr *addr2 = (const sc_addr*)b;
    return SC_ADDR_IS_EQUAL(*addr1, *addr2);
}

//! Inserts specified event into events table
sc_result insert_event_into_table(sc_event *event)
{
    GSList *element_events_list = 0;

    // first of all, if table doesn't exist, then create it
    if (events_table == nullptr)
        events_table = g_hash_table_new(events_table_hash_func, events_table_equal_func);

    // if there are no events for specified sc-element, then create new events list
    element_events_list = (GSList*)g_hash_table_lookup(events_table, (gconstpointer)&event->element);
    element_events_list = g_slist_append(element_events_list, (gpointer)event);
    g_hash_table_insert(events_table, (gpointer)&event->element, (gpointer)element_events_list);

    return SC_RESULT_OK;
}

//! Remove specified sc-event from events table
sc_result remove_event_from_table(sc_event *event)
{
    GSList *element_events_list = 0;
    g_assert(events_table != nullptr);

    element_events_list = (GSList*)g_hash_table_lookup(events_table, (gconstpointer)&event->element);
    if (element_events_list == nullptr)
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // remove event from list of events for specified sc-element
    element_events_list = g_slist_remove(element_events_list, (gconstpointer)event);
    if (element_events_list == nullptr)
        g_hash_table_remove(events_table, (gconstpointer)&event->element);
    else
        g_hash_table_insert(events_table, (gpointer)&event->element, (gpointer)element_events_list);

    // if there are no more events in table, then delete it
    if (g_hash_table_size(events_table) == 0)
    {
        g_hash_table_destroy(events_table);
        events_table = nullptr;
    }

    return SC_RESULT_OK;
}


sc_event* sc_event_new(sc_addr el, sc_event_type type, sc_uint32 id, fEventCallback callback, fDeleteCallback delete_callback)
{
    sc_event *event = g_new0(sc_event, 1);
    event->element = el;
    event->type = type;
    event->id = id;
    event->callback = callback;
    event->delete_callback = delete_callback;

    g_assert(callback != nullptr);

    // register created event
    if (insert_event_into_table(event) != SC_RESULT_OK)
    {
        g_free(event);
        return nullptr;
    }

    return event;
}

sc_result sc_event_destroy(sc_event *event)
{
    if (remove_event_from_table(event) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    g_free(event);

    return SC_RESULT_OK;
}

sc_result sc_event_notify_element_deleted(sc_addr element)
{
    GSList *element_events_list = 0;
    sc_event *event = 0;

    // do nothing, if there are no registered events
    if (events_table == nullptr)
        return SC_RESULT_OK;

    // lookup for all registered to specified sc-elemen events
    element_events_list = (GSList*)g_hash_table_lookup(events_table, (gconstpointer)&element);

    // destoroy events
    while (element_events_list != nullptr)
    {
        event = (sc_event*)element_events_list->data;
        if (event->delete_callback != nullptr)
            event->delete_callback(event);
        element_events_list = g_slist_delete_link(element_events_list, element_events_list);
    }


    return SC_RESULT_OK;
}

sc_result sc_event_emit(sc_addr el, sc_event_type type, sc_addr arg)
{
    GSList *element_events_list = 0;
    sc_event *event = 0;

    // if table is empty, then do nothing
    if (events_table == nullptr)
        return SC_RESULT_OK;

    // lookup for all registered to specified sc-elemen events
    element_events_list = (GSList*)g_hash_table_lookup(events_table, (gconstpointer)&el);
    while (element_events_list != nullptr)
    {
        event = (sc_event*)element_events_list->data;

        if (event->type == type)
        {
            g_assert(event->callback != nullptr);
            event->callback(event, arg);
        }

        element_events_list = element_events_list->next;
    }

    return SC_RESULT_OK;
}

