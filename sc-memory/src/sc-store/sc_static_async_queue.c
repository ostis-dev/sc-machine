#include "sc_static_async_queue.h"

// scope pop
// 2 mutex: 1 on queue, 2-nd on empty
sc_static_async_queue* sc_sa_queue_new(sc_int32 length)
{
    const gint size  = sizeof(sc_static_async_queue) + length*sizeof(void*);
    sc_static_async_queue* queue = (sc_static_async_queue*) g_malloc(size);

    g_assert(queue != NULL);

    SC_MUTEX_NEW(queue->mutex);

    queue->head = 0;
    queue->tail = 0;

    queue->length = length;

    guint i = 0;
    for(i = 0; i < queue->length; i++)
    {
        queue->values[i] = NULL;
    }

    return queue;
}

void sc_sa_queue_destroy(sc_static_async_queue* queue, sc_sa_queue_value_destructor func)
{
    void* elem;
    do
    {
        elem = sc_sa_queue_try_pop(queue);
        func(elem);
    }while(elem != NULL);

    SC_MUTEX_FREE(queue->mutex);

    g_free(queue);
    queue = NULL;
}

void* sc_sa_queue_try_pop(sc_static_async_queue* queue)
{
    void* elem = NULL;

    SC_MUTEX_LOCK(queue->mutex);

    if(queue->values[queue->head] == NULL)
    {
        elem = queue->values[queue->head];
        queue->values[queue->head] = NULL;

        queue->head++;
        if(queue->head == queue->length)
        {
            queue->head = 0;
        }

        SC_MUTEX_COMPLITE_WAIT(queue->mutex);
    }

    SC_MUTEX_UNLOCK(queue->mutex);

    return elem;
}

// ??
void* sc_sa_queue_pop(sc_static_async_queue* queue)
{
    void* elem = NULL;

    SC_MUTEX_LOCK(queue->mutex);

    while(queue->values[queue->head] == NULL)
    {
        SC_MUTEX_WAIT(queue->mutex); // wait, until element will added
    }

    elem = queue->values[queue->head];
    queue->values[queue->head] = NULL;

    queue->head++;
    if(queue->head == queue->length)
    {
        queue->head = 0;
    }

    SC_MUTEX_COMPLITE_WAIT(queue->mutex);

    SC_MUTEX_UNLOCK(queue->mutex);

    g_assert(elem != NULL);
    return elem;
}

sc_bool sc_sa_queue_try_push(sc_static_async_queue* queue, void* value)
{
    //! NULL using as null terminator
    g_assert(value != NULL);

    sc_bool result = SC_FALSE;

    SC_MUTEX_LOCK(queue->mutex);

    if(queue->values[queue->tail] == NULL)
    {
        queue->values[queue->tail] = value;

        queue->tail++;
        if(queue->tail == queue->length)
        {
            queue->tail = 0;
        }

        SC_MUTEX_COMPLITE_WAIT(queue->mutex);
        result = SC_TRUE;
    }

    SC_MUTEX_UNLOCK(queue->mutex);

    return result;
}

void sc_sa_queue_push(sc_static_async_queue* queue, void* value)
{
    //! NULL using as null terminator
    g_assert(value != NULL);

    SC_MUTEX_LOCK(queue->mutex);

    if(queue->values[queue->tail] != NULL)
    {
        SC_MUTEX_WAIT(queue->mutex);
    }

    queue->values[queue->tail] = value;

    queue->tail++;
    if(queue->tail == queue->length)
    {
        queue->tail = 0;
    }

    SC_MUTEX_COMPLITE_WAIT(queue->mutex);

    SC_MUTEX_UNLOCK(queue->mutex);

    return;
}
