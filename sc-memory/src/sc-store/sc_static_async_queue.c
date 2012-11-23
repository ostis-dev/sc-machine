#include "sc_static_async_queue.h"

sc_static_async_queue* sc_sa_queue_new(sc_int32 length)
{
    const gint size  = sizeof(sc_static_async_queue) + length*sizeof(void*);
    sc_static_async_queue* queue = (sc_static_async_queue*) g_malloc(size);

    g_assert(queue != NULL);

    SC_MUTEX_NEW(queue->mutex_tail);
    SC_MUTEX_NEW(queue->mutex_head);

    g_atomic_int_set(&queue->head, 0);
    g_atomic_int_set(&queue->tail, 0);

    queue->length = length;

    guint i = 0;
    for(i = 0; i < queue->length; i++)
    {
        queue->values[size] = NULL;
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

    SC_MUTEX_FREE(queue->mutex_tail);
    SC_MUTEX_FREE(queue->mutex_head);

    g_free(queue);
    queue = NULL;
}

void* sc_sa_queue_try_pop(sc_static_async_queue* queue)
{
    void* elem = NULL;

    SC_MUTEX_LOCK(queue->mutex_head);

    const gint head_pos = g_atomic_int_get(&queue->head);

    if(queue->values[head_pos] != NULL)
    {
        return NULL; // don't wait
    }
    else
    {
        g_atomic_int_inc(&queue->head); // +1
        g_atomic_int_compare_and_exchange(&queue->head, queue->length, 0); // val == length : 0

        elem = g_atomic_pointer_get(&queue->values[head_pos]);
        g_atomic_pointer_set(&queue->values[head_pos], NULL);

        SC_MUTEX_COMPLITE_WAIT(queue->mutex_head);
    }

    SC_MUTEX_UNLOCK(queue->mutex_head);

    return elem;
}

// ??
void* sc_sa_queue_pop(sc_static_async_queue* queue)
{
    void* elem = NULL;

    g_mutex_lock(queue->mutex_head->mtx);

    gint head_pos = g_atomic_int_get(&queue->head);

    if(queue->values[head_pos] == NULL)
    {
        //SC_MUTEX_WAIT(queue->mutex_head); // wait, until element will added

        g_atomic_int_set(&queue->mutex_head->is_waiting, SC_TRUE);
        while(g_atomic_int_get(&queue->mutex_head->is_waiting))
        {
            g_cond_wait (queue->mutex_head->cnd, queue->mutex_head->mtx);
        }
        head_pos = g_atomic_int_get(&queue->head);
    }

    elem = g_atomic_pointer_get(&queue->values[head_pos]);
    g_atomic_pointer_set(&queue->values[head_pos], NULL);

    g_atomic_int_inc(&queue->head); // +1
    g_atomic_int_compare_and_exchange(&queue->head, queue->length, 0); // val == length : 0

    SC_MUTEX_LOCK(queue->mutex_tail);
    SC_MUTEX_COMPLITE_WAIT(queue->mutex_tail); // free place pushing for element
    SC_MUTEX_UNLOCK(queue->mutex_tail);

    SC_MUTEX_UNLOCK(queue->mutex_head);

    return elem;
}

sc_bool sc_sa_queue_try_push(sc_static_async_queue* queue, void* value)
{
    //! NULL using as null terminator
    g_assert(value != NULL);

    sc_bool result;

    SC_MUTEX_LOCK(queue->mutex_tail);

    const gint tail_pos = g_atomic_int_get(&queue->tail);

    if(queue->values[tail_pos] != NULL)
    {
        result = SC_FALSE;
    }
    else
    {
        g_atomic_int_inc(&queue->tail); // +1
        g_atomic_int_compare_and_exchange(&queue->tail, queue->length, 0); // val == length : 0

        g_atomic_pointer_set(&queue->values[tail_pos], value);

        SC_MUTEX_COMPLITE_WAIT(queue->mutex_head);
        result = SC_TRUE;
    }

    SC_MUTEX_UNLOCK(queue->mutex_tail);

    return result;
}

void sc_sa_queue_push(sc_static_async_queue* queue, void* value)
{
    //! NULL using as null terminator
    g_assert(value != NULL);

    g_mutex_lock(queue->mutex_tail->mtx);

    gint tail_pos = g_atomic_int_get(&queue->tail);

    if(queue->values[tail_pos] != NULL)
    {
        //SC_MUTEX_WAIT(queue->mutex_tail);
        g_atomic_int_set(&queue->mutex_tail->is_waiting, SC_TRUE);
        while(g_atomic_int_get(&queue->mutex_tail->is_waiting))
        {
            g_cond_wait (queue->mutex_tail->cnd, queue->mutex_tail->mtx);
        }
        tail_pos = g_atomic_int_get(&queue->tail);
    }

    g_atomic_pointer_set(&queue->values[tail_pos], value);

    g_atomic_int_inc(&queue->tail); // +1
    g_atomic_int_compare_and_exchange(&queue->tail, queue->length, 0); // val == length : 0

    SC_MUTEX_LOCK(queue->mutex_head);
    SC_MUTEX_COMPLITE_WAIT(queue->mutex_head);
    SC_MUTEX_UNLOCK(queue->mutex_head);

    SC_MUTEX_UNLOCK(queue->mutex_tail);

    return;
}
