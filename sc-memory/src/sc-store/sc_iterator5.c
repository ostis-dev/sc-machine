#include "search.h"
#include "sc_iterator5.h"

#include <glib.h>

sc_iterator5* sc_iterator5_new(sc_iterator5_type type, sc_iterator_param p1, sc_iterator_param p2, sc_iterator_param p3, sc_iterator_param p4, sc_iterator_param p5)
{

    // check params with template
    switch (type)
    {
    case sc_iterator5_f_a_a_a_f:
        if (p1.is_type || !p2.is_type || !p3.is_type || !p4.is_type || p5.is_type)
            return (sc_iterator5*)nullptr;
        break;
    case sc_iterator5_a_a_f_a_f:
        if (!p1.is_type || !p2.is_type || p3.is_type || !p4.is_type || p5.is_type)
            return (sc_iterator5*)nullptr;
        break;
    case sc_iterator5_f_a_f_a_f:
        if (p1.is_type || !p2.is_type || p3.is_type || !p4.is_type || p5.is_type)
            return (sc_iterator5*)nullptr;
        break;
    case sc_iterator5_f_a_f_a_a:
        if (p1.is_type || !p2.is_type || p3.is_type || !p4.is_type || !p5.is_type)
            return (sc_iterator5*)nullptr;
        break;
    case sc_iterator5_f_a_a_a_a:
        if (p1.is_type || !p2.is_type || !p3.is_type || !p4.is_type || !p5.is_type)
            return (sc_iterator5*)nullptr;
        break;
    case sc_iterator5_a_a_f_a_a:
        if (!p1.is_type || !p2.is_type || p3.is_type || !p4.is_type || !p5.is_type)
            return (sc_iterator5*)nullptr;
        break;
    };

    sc_iterator5 *it = g_new0(sc_iterator5, 1);

    it->params[0] = p1;
    it->params[1] = p2;
    it->params[2] = p3;
    it->params[3] = p4;
    it->params[4] = p5;

    it->type = type;

    // create main cycle iterator
    switch (type)
    {
    case sc_iterator5_f_a_a_a_f:
        it->it_main = sc_iterator3_f_a_a_new(p1.addr ,p2.type, p3.type);
        it->it_attr = nullptr;
        it->results[0] = p1.addr;
        it->results[4] = p5.addr;
        break;
    case sc_iterator5_a_a_f_a_f:
        it->it_main = sc_iterator3_a_a_f_new(p1.type, p2.type, p3.addr);
        it->it_attr = nullptr;
        it->results[2] = p3.addr;
        it->results[4] = p5.addr;
        break;
    case sc_iterator5_f_a_f_a_f:
        it->it_main = sc_iterator3_f_a_f_new(p1.addr, p2.type, p3.addr);
        it->it_attr = nullptr;
        it->results[0] = p1.addr;
        it->results[2] = p3.addr;
        it->results[4] = p5.addr;
        break;
    case sc_iterator5_f_a_f_a_a:
        it->it_main = sc_iterator3_f_a_f_new(p1.addr, p2.type, p3.addr);
        it->it_attr = nullptr;
        it->results[0] = p1.addr;
        it->results[2] = p3.addr;
        break;
    case sc_iterator5_a_a_f_a_a:
        it->it_main = sc_iterator3_a_a_f_new(p1.type,p2.type,p3.addr);
        it->it_attr = nullptr;
        it->results[2] = p3.addr;
        break;
    case sc_iterator5_f_a_a_a_a:
        it->it_main = sc_iterator3_f_a_a_new(p1.addr, p2.type, p3.type);
        it->it_attr = nullptr;
        it->results[0] = p1.addr;
        break;
    };

    return it;
}

sc_iterator5* sc_iterator5_f_a_a_a_f_new(sc_addr p1, sc_type p2, sc_type p3, sc_type p4, sc_addr p5)
{
    sc_iterator_param _p1,_p2,_p3,_p4,_p5;
    _p1.is_type = SC_FALSE;
    _p1.addr = p1;
    _p2.is_type = SC_TRUE;
    _p2.type = p2;
    _p3.is_type = SC_TRUE;
    _p3.type = p3;
    _p4.is_type = SC_TRUE;
    _p4.type = p4;
    _p5.is_type = SC_FALSE;
    _p5.addr = p5;

    return sc_iterator5_new(sc_iterator5_f_a_a_a_f, _p1, _p2, _p3, _p4, _p5);
}

sc_iterator5* sc_iterator5_a_a_f_a_f_new(sc_type p1, sc_type p2, sc_addr p3, sc_type p4, sc_addr p5)
{
    sc_iterator_param _p1,_p2,_p3,_p4,_p5;
    _p1.is_type = SC_TRUE;
    _p1.type = p1;
    _p2.is_type = SC_TRUE;
    _p2.type = p2;
    _p3.is_type = SC_FALSE;
    _p3.addr = p3;
    _p4.is_type = SC_TRUE;
    _p4.type = p4;
    _p5.is_type = SC_FALSE;
    _p5.addr = p5;

    return sc_iterator5_new(sc_iterator5_a_a_f_a_f, _p1, _p2, _p3, _p4, _p5);
}

sc_iterator5* sc_iterator5_f_a_f_a_f_new(sc_addr p1, sc_type p2, sc_addr p3, sc_type p4, sc_addr p5)
{
    sc_iterator_param _p1,_p2,_p3,_p4,_p5;
    _p1.is_type = SC_FALSE;
    _p1.addr = p1;
    _p2.is_type = SC_TRUE;
    _p2.type = p2;
    _p3.is_type = SC_FALSE;
    _p3.addr = p3;
    _p4.is_type = SC_TRUE;
    _p4.type = p4;
    _p5.is_type = SC_FALSE;
    _p5.addr = p5;

    return sc_iterator5_new(sc_iterator5_f_a_f_a_f, _p1, _p2, _p3, _p4, _p5);
}

sc_iterator5* sc_iterator5_f_a_f_a_a_new(sc_addr p1, sc_type p2, sc_addr p3, sc_type p4, sc_type p5)
{
    sc_iterator_param _p1,_p2,_p3,_p4,_p5;
    _p1.is_type = SC_FALSE;
    _p1.addr = p1;
    _p2.is_type = SC_TRUE;
    _p2.type = p2;
    _p3.is_type = SC_FALSE;
    _p3.addr = p3;
    _p4.is_type = SC_TRUE;
    _p4.type = p4;
    _p5.is_type = SC_TRUE;
    _p5.type = p5;

    return sc_iterator5_new(sc_iterator5_f_a_f_a_a, _p1, _p2, _p3, _p4, _p5);
}

sc_iterator5* sc_iterator5_f_a_a_a_a_new(sc_addr p1, sc_type p2, sc_type p3, sc_type p4, sc_type p5)
{
    sc_iterator_param _p1,_p2,_p3,_p4,_p5;
    _p1.is_type = SC_FALSE;
    _p1.addr = p1;
    _p2.is_type = SC_TRUE;
    _p2.type = p2;
    _p3.is_type = SC_TRUE;
    _p3.type = p3;
    _p4.is_type = SC_TRUE;
    _p4.type = p4;
    _p5.is_type = SC_TRUE;
    _p5.type = p5;

    return sc_iterator5_new(sc_iterator5_f_a_a_a_a, _p1, _p2, _p3, _p4, _p5);
}

sc_iterator5* sc_iterator5_a_a_f_a_a_new(sc_type p1, sc_type p2, sc_addr p3, sc_type p4, sc_type p5)
{
    sc_iterator_param _p1,_p2,_p3,_p4,_p5;
    _p1.is_type = SC_TRUE;
    _p1.type = p1;
    _p2.is_type = SC_TRUE;
    _p2.type = p2;
    _p3.is_type = SC_FALSE;
    _p3.addr = p3;
    _p4.is_type = SC_TRUE;
    _p4.type = p4;
    _p5.is_type = SC_TRUE;
    _p5.type = p5;

    return sc_iterator5_new(sc_iterator5_a_a_f_a_a, _p1, _p2, _p3, _p4, _p5);
}


void sc_iterator5_free(sc_iterator5 *it)
{
    if (it->it_attr != nullptr)
        sc_iterator3_free(it->it_attr);
    if (it->it_main != nullptr)
        sc_iterator3_free(it->it_main);
    g_free(it);
}

sc_bool _sc_iterator5_a_a_f_a_f_next(sc_iterator5 *it)
{
    if (it->type != sc_iterator5_a_a_f_a_f)
        return SC_FALSE;

    SC_ADDR_MAKE_EMPTY(it->results[0])
    SC_ADDR_MAKE_EMPTY(it->results[1])
    SC_ADDR_MAKE_EMPTY(it->results[3])

    if (it->it_attr != nullptr && sc_iterator3_next(it->it_attr))
    {
        it->results[0] = it->it_main->results[0];
        it->results[1]=it->it_main->results[1];
        it->results[3]=it->it_attr->results[1];
        return SC_TRUE;
    }else
    {
        while( it->it_attr == nullptr || !sc_iterator3_next(it->it_attr))
        {
            if (it->it_attr != nullptr)
                sc_iterator3_free(it->it_attr);

            if (!sc_iterator3_next(it->it_main))
                return SC_FALSE;

            it->it_attr = sc_iterator3_f_a_f_new(it->params[4].addr,
                                                 it->params[3].type,
                                                 it->it_main->results[1]);
            if (it->it_attr != nullptr)
                continue;
        }
        it->results[0] = it->it_main->results[0];
        it->results[1] = it->it_main->results[1];
        it->results[3] = it->it_attr->results[1];
        return SC_TRUE;
    }

    return SC_FALSE;
}

sc_bool _sc_iterator5_f_a_a_a_f_next(sc_iterator5 *it)
{
    if (it->type != sc_iterator5_f_a_a_a_f)
        return SC_FALSE;

    SC_ADDR_MAKE_EMPTY(it->results[1])
    SC_ADDR_MAKE_EMPTY(it->results[2])
    SC_ADDR_MAKE_EMPTY(it->results[3])

    if (it->it_attr != nullptr && sc_iterator3_next(it->it_attr))
    {
        it->results[1] = it->it_main->results[1];
        it->results[2] = it->it_main->results[2];
        it->results[3] = it->it_attr->results[1];
        return SC_TRUE;
    }else
    {
        while( it->it_attr == nullptr || !sc_iterator3_next(it->it_attr))
        {
            if (it->it_attr != nullptr)
                sc_iterator3_free(it->it_attr);
            if (!sc_iterator3_next(it->it_main))
                return SC_FALSE;

            it->it_attr = sc_iterator3_f_a_f_new(it->params[4].addr,
                                                 it->params[3].type,
                                                 it->it_main->results[1]);
            if (it->it_attr != nullptr)
                continue;
        }
        it->results[1] = it->it_main->results[1];
        it->results[2] = it->it_main->results[2];
        it->results[3] = it->it_attr->results[1];
        return SC_TRUE;
    }

    return SC_FALSE;
}

sc_bool _sc_iterator5_f_a_f_a_f_next(sc_iterator5 *it)
{
    if (it->type != sc_iterator5_f_a_f_a_f)
        return SC_FALSE;

    SC_ADDR_MAKE_EMPTY(it->results[1])
    SC_ADDR_MAKE_EMPTY(it->results[3])

    if (it->it_attr != nullptr && sc_iterator3_next(it->it_attr))
    {
        it->results[1] = it->it_main->results[1];
        it->results[3] = it->it_attr->results[1];
        return SC_TRUE;
    }else
    {
        while( it->it_attr == nullptr || !sc_iterator3_next(it->it_attr))
        {
            if (it->it_attr != nullptr)
                sc_iterator3_free(it->it_attr);
            if (!sc_iterator3_next(it->it_main))
                return SC_FALSE;

            it->it_attr = sc_iterator3_f_a_f_new(it->params[4].addr,
                                                 it->params[3].type,
                                                 it->it_main->results[1]);
            if (it->it_attr != nullptr)
                continue;
        }
        it->results[1] = it->it_main->results[1];
        it->results[3] = it->it_attr->results[1];
        return SC_TRUE;
    }

    return SC_FALSE;
}

sc_bool _sc_iterator5_f_a_f_a_a_next(sc_iterator5 *it)
{
    if (it->type!=sc_iterator5_f_a_f_a_a)
        return SC_FALSE;

    SC_ADDR_MAKE_EMPTY(it->results[1])
    SC_ADDR_MAKE_EMPTY(it->results[3])
    SC_ADDR_MAKE_EMPTY(it->results[4])

    if (it->it_attr != nullptr && sc_iterator3_next(it->it_attr))
    {
        it->results[1] = it->it_main->results[1];
        it->results[3] = it->it_attr->results[1];
        it->results[4] = it->it_attr->results[0];
        return SC_TRUE;
    }else
    {
        while( it->it_attr == nullptr || !sc_iterator3_next(it->it_attr))
        {
            if (it->it_attr != nullptr)
                sc_iterator3_free(it->it_attr);

            if (!sc_iterator3_next(it->it_main))
                return SC_FALSE;

            it->it_attr = sc_iterator3_a_a_f_new(it->params[4].type,
                                               it->params[3].type,
                                               it->it_main->results[1]);
            if (it->it_attr != nullptr)
                continue;
        }
        it->results[1] = it->it_main->results[1];
        it->results[3] = it->it_attr->results[1];
        it->results[4] = it->it_attr->results[0];
        return SC_TRUE;
    }

    return SC_FALSE;
}

sc_bool _sc_iterator5_f_a_a_a_a_next(sc_iterator5 *it)
{
    if (it->type != sc_iterator5_f_a_a_a_a)
        return SC_FALSE;

    SC_ADDR_MAKE_EMPTY(it->results[1])
    SC_ADDR_MAKE_EMPTY(it->results[2])
    SC_ADDR_MAKE_EMPTY(it->results[3])
    SC_ADDR_MAKE_EMPTY(it->results[4])

    if (it->it_attr != nullptr && sc_iterator3_next(it->it_attr))
    {
        it->results[1] = it->it_main->results[1];
        it->results[2] = it->it_main->results[2];
        it->results[3] = it->it_attr->results[1];
        it->results[4] = it->it_attr->results[0];
        return SC_TRUE;
    }else
    {
        while(it->it_attr == nullptr || !sc_iterator3_next(it->it_attr))
        {
            if (it->it_attr != nullptr)
                sc_iterator3_free(it->it_attr);
            if (!sc_iterator3_next(it->it_main))
                return SC_FALSE;

            it->it_attr = sc_iterator3_a_a_f_new(it->params[4].type,
                                                 it->params[3].type,
                                                 it->it_main->results[1]);
            if (it->it_attr != nullptr)
                continue;
        }
        it->results[1] = it->it_main->results[1];
        it->results[2] = it->it_main->results[2];
        it->results[3] = it->it_attr->results[1];
        it->results[4] = it->it_attr->results[0];
        return SC_TRUE;
    }

    return SC_FALSE;
}

sc_bool _sc_iterator5_a_a_f_a_a_next(sc_iterator5 *it)
{
    if (it->type != sc_iterator5_a_a_f_a_a)
        return SC_FALSE;

    SC_ADDR_MAKE_EMPTY(it->results[1])
    SC_ADDR_MAKE_EMPTY(it->results[0])
    SC_ADDR_MAKE_EMPTY(it->results[3])
    SC_ADDR_MAKE_EMPTY(it->results[4])

    if (it->it_attr != nullptr && sc_iterator3_next(it->it_attr))
    {
        it->results[0] = it->it_main->results[0];
        it->results[1] = it->it_main->results[1];
        it->results[3] = it->it_attr->results[1];
        it->results[4] = it->it_attr->results[0];
        return SC_TRUE;
    }else
    {
        while(it->it_attr == nullptr || !sc_iterator3_next(it->it_attr))
        {
            if (it->it_attr != nullptr)
                sc_iterator3_free(it->it_attr);
            if (!sc_iterator3_next(it->it_main))
                return SC_FALSE;

            it->it_attr = sc_iterator3_a_a_f_new(it->params[4].type,
                                                 it->params[3].type,
                                                 it->it_main->results[1]);
            if (it->it_attr != nullptr)
                continue;
        }
        it->results[0] = it->it_main->results[0];
        it->results[1] = it->it_main->results[1];
        it->results[3] = it->it_attr->results[1];
        it->results[4] = it->it_attr->results[0];
        return SC_TRUE;
    }

    return SC_FALSE;
}


sc_bool sc_iterator5_next(sc_iterator5 *it)
{
    g_assert(it != 0);


    switch (it->type)
    {
    case sc_iterator5_f_a_a_a_f:
        return _sc_iterator5_f_a_a_a_f_next(it);

    case sc_iterator5_a_a_f_a_f:
        return _sc_iterator5_a_a_f_a_f_next(it);

    case sc_iterator5_f_a_f_a_f:
        return _sc_iterator5_f_a_f_a_f_next(it);

    case sc_iterator5_f_a_f_a_a:
        return _sc_iterator5_f_a_f_a_a_next(it);

    case sc_iterator5_f_a_a_a_a:
        return _sc_iterator5_f_a_a_a_a_next(it);

    case sc_iterator5_a_a_f_a_a:
        return _sc_iterator5_a_a_f_a_a_next(it);
    };

    return SC_FALSE;
}

sc_addr sc_iterator5_value(sc_iterator5 *it, sc_uint vid)
{
    g_assert(it != 0);
    g_assert(it->results != 0);
    g_assert(vid < 5);

    return it->results[vid];
}
