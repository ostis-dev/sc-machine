extern "C"
{
#include "sc_task_manager.h"
#include "sc_memory.h"
}

void node_test()
{
    sc_uint idx = 0;
    sc_addr id, id2;
    sc_uint32 count = 0;

    timer = g_timer_new();
    print_storage_statistics();
    printf("--- Node creation ---\n");
    g_timer_start(timer);
    for (idx = 0; idx < nodes_append_count; idx++)
    {
        id = sc_memory_node_new(sc_type_node);
    }
    g_timer_stop(timer);
    printf("Time: %f s\n", g_timer_elapsed(timer, 0));
    printf("Nodes per second: %f\n", (float)nodes_append_count / g_timer_elapsed(timer, 0));
    printf("Segments count: %d\n", sc_storage_get_segments_count());
    print_storage_statistics();
}

void task_manager_test()
{
    sc_uint idx = 0;
    sc_addr id, id2;
    sc_uint32 count = 0;

    sc_task_manager_initialize();

    printf("Element size: %d bytes\n", sizeof(sc_element));
    printf("Segment size: %d elements\n", SEGMENT_SIZE);

    timer = g_timer_new();
    print_storage_statistics();
    printf("--- Node creation ---\n");
    g_timer_start(timer);
    for (idx = 0; idx < nodes_append_count; idx++)
    {
        id = sc_memory_node_new(sc_type_node);
        //g_printf("uri: %d\t%d\n", uri.seg, uri.id);
    }
    g_timer_stop(timer);
    printf("Time: %f s\n", g_timer_elapsed(timer, 0));
    printf("Nodes per second: %f\n", (float)nodes_append_count / g_timer_elapsed(timer, 0));
    printf("Segments count: %d\n", sc_storage_get_segments_count());
    print_storage_statistics();

    g_timer_reset(timer);
    printf("--- Node segmentation ---\n");
    count = 0;

    printf("Prepare test...\n");
    for (idx = 1; idx < nodes_remove_count + 1; idx++)
    {
        if (idx % 10 < 5)
        {
            id.seg = idx / SEGMENT_SIZE;
            id.offset = idx % SEGMENT_SIZE;
            /*do
      {
    id = get_random_addr(sc_type_node);
      }
      while(!sc_storage_is_element(id) && is_sc_addr_in_segment_node_vector(id));*/
            if (sc_memory_is_element(id))
                segment_node_del.push_back(id);
        }
    }

    printf("Run test...\n");
    g_timer_start(timer);

    sc_uint32 n = segment_node_del.size();
    for (sc_uint32 i = 0; i < n; ++i)
        sc_memory_element_free(segment_node_del[i]);

    n = nodes_remove_count - n;
    for (sc_uint32 i = 0; i < n; i++)
        sc_memory_node_new(0);

    g_timer_stop(timer);

    segment_node_del.clear();

    printf("Time: %f s\n", g_timer_elapsed(timer, 0));
    printf("Elements per second: %f\n", (float)nodes_remove_count / g_timer_elapsed(timer, 0));
    printf("Segments count: %d\n", sc_storage_get_segments_count());
    printf("Element free calls: %u\n", segment_node_del.size());
    print_storage_statistics();


    g_timer_reset(timer);
    printf("--- Arcs creation ---\n");
    count = 0;

    printf("Prepare...\n");
    for (idx = 0; idx < arcs_append_count; idx++)
    {
        do
        {
            id = get_random_addr(0);
        }while (!sc_memory_is_element(id));

        do
        {
            id2 = get_random_addr(0);
        }while (!sc_memory_is_element(id2));

        arc_creation_vector.push_back(id);
        arc_creation_vector.push_back(id2);
    }

    printf("Run...\n");
    g_timer_start(timer);
    n = arc_creation_vector.size() / 2;
    for (sc_uint32 i = 0; i < n; ++i)
    {
        sc_memory_arc_new(sc_type_arc_common, arc_creation_vector[i], arc_creation_vector[i + n]);
    }

    g_timer_stop(timer);
    printf("Timer: %fs\n", g_timer_elapsed(timer, 0));
    printf("Arcs per second: %f\n", (float)n / g_timer_elapsed(timer, 0));
    printf("Segments count: %d\n", sc_storage_get_segments_count());

    print_storage_statistics();

    g_timer_destroy(timer);}
