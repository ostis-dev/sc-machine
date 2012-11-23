#ifndef SC_STATIC_ASYNC_QUEUE_H
#define SC_STATIC_ASYNC_QUEUE_H

#include <glib.h>
#include "sc_types.h"
#include "sc_thread.h"

#define G_ATOMIC_LOCK_FREE

typedef struct _sc_static_async_queue
{
    //! Head own mutex
    sc_mutex* mutex_head;

    //! Tail own mutex
    sc_mutex* mutex_tail;

    //! Points on the first element in queue
    gint head;

    //! Points on over last element
    gint tail;

    //! Length of queue (size)
    size_t length;

    //! Data container
    void* values[];
} sc_static_async_queue;

typedef void (sc_sa_queue_value_destructor)(void*);

sc_static_async_queue* sc_sa_queue_new(sc_int32  length);

void sc_sa_queue_destroy(sc_static_async_queue* queue, sc_sa_queue_value_destructor func);

/**
  * @brief
  */
void* sc_sa_queue_try_pop(sc_static_async_queue* queue);

/**
  * @brief
  */
void* sc_sa_queue_pop(sc_static_async_queue* queue);

/**
  * @brief
  */
sc_bool sc_sa_queue_try_push(sc_static_async_queue* queue, void* value);

/**
  * @brief
  */
void sc_sa_queue_push(sc_static_async_queue* queue, void* value);

#endif // SC_STATIC_ASYNC_QUEUE_H
