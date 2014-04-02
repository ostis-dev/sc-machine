/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */
#include "search_structure.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search_defines.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

sc_result agent_search_decomposition(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2, *it3;
    sc_iterator5 *it5, *it_order;
    sc_bool sys_off = SC_TRUE, found = SC_FALSE;
    sc_type el_type;
    const char keynode_sc_agent_of_search_of_decomposition_agent_c_str[] = "sc_agent_of_search_of_decomposition_agent_c";

    if (!sc_memory_get_arc_end(arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(keynode_question_decomposition, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    log_agent_started(keynode_sc_agent_of_search_of_decomposition_agent_c_str);

    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(question, sc_type_arc_pos_const_perm, 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        // iterate decomposition
        it5 = sc_iterator5_a_a_f_a_a_new(sc_type_node | sc_type_const,
                                         sc_type_arc_common | sc_type_const,
                                         sc_iterator3_value(it1, 2),
                                         sc_type_arc_pos_const_perm,
                                         sc_type_node | sc_type_const);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            if (SC_FALSE == sc_helper_check_arc(keynode_decomposition_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm))
                continue;
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 0))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 4))))
                continue;

            found = SC_TRUE;

            appendIntoAnswer(answer, sc_iterator5_value(it5, 0));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 4));

            // iterate decomposition set elements
            it2 = sc_iterator3_f_a_a_new(sc_iterator5_value(it5, 0), sc_type_arc_pos_const_perm, 0);
            while (sc_iterator3_next(it2) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))
                                           || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                    continue;

                // iterate order relations between elements
                it_order = sc_iterator5_f_a_a_a_a_new(sc_iterator3_value(it2, 2),
                                                      sc_type_arc_common | sc_type_const,
                                                      sc_type_node | sc_type_const,
                                                      sc_type_arc_pos_const_perm,
                                                      sc_type_node | sc_type_const);
                while (sc_iterator5_next(it_order) == SC_TRUE)
                {
                    if (SC_FALSE == sc_helper_check_arc(keynode_order_relation, sc_iterator5_value(it_order, 4), sc_type_arc_pos_const_perm))
                        continue;
                    if (SC_FALSE == sc_helper_check_arc(sc_iterator5_value(it5, 0), sc_iterator5_value(it_order, 2), sc_type_arc_pos_const_perm))
                        continue;

                    if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 1))
                                               || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 2))
                                               || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 3))
                                               || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 4))))
                        continue;

                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 1));
                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 2));
                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 3));
                    appendIntoAnswer(answer, sc_iterator5_value(it_order, 4));

                }
                sc_iterator5_free(it_order);

                // iterate roles of element in link
                it3 = sc_iterator3_a_a_f_new(sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_iterator3_value(it2, 1));
                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    sc_memory_get_element_type(sc_iterator3_value(it3, 0), &el_type);
                    if (!(el_type & sc_type_node_role))
                        continue;

                    if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0))
                                               || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1))))
                        continue;

                    appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                    appendIntoAnswer(answer, sc_iterator3_value(it3, 1));
                }
                sc_iterator3_free(it3);

                appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
                appendIntoAnswer(answer, sc_iterator3_value(it2, 2));
            }
            sc_iterator3_free(it2);
        }
        sc_iterator5_free(it5);
    }
    sc_iterator3_free(it1);

    log_agent_finished(keynode_sc_agent_of_search_of_decomposition_agent_c_str, found);
    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

sc_bool search_subclasses_rec(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_iterator3 *it2, *it6;
    sc_iterator5 *it5, *it_order;
    sc_type el_type;
    sc_bool found = SC_FALSE;

    // iterate decomposition
    it5 = sc_iterator5_a_a_f_a_a_new(sc_type_node | sc_type_const,
                                     sc_type_arc_common | sc_type_const,
                                     elem,
                                     sc_type_arc_pos_const_perm,
                                     sc_type_node | sc_type_const);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
        if (SC_FALSE == sc_helper_check_arc(keynode_decomposition_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm))
            continue;

        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 0))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 4))))
            continue;

        found = SC_TRUE;

        appendIntoAnswer(answer, sc_iterator5_value(it5, 0));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 4));

        // iterate decomposition set elements
        it2 = sc_iterator3_f_a_a_new(sc_iterator5_value(it5, 0), sc_type_arc_pos_const_perm, 0);
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))
                                       || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                continue;

            // iterate order relations between elements
            it_order = sc_iterator5_f_a_a_a_a_new(sc_iterator3_value(it2, 2),
                                                  sc_type_arc_common | sc_type_const,
                                                  sc_type_node | sc_type_const,
                                                  sc_type_arc_pos_const_perm,
                                                  sc_type_node | sc_type_const);
            while (sc_iterator5_next(it_order) == SC_TRUE)
            {
                if (SC_FALSE == sc_helper_check_arc(keynode_order_relation, sc_iterator5_value(it_order, 4), sc_type_arc_pos_const_perm))
                    continue;
                if (SC_FALSE == sc_helper_check_arc(sc_iterator5_value(it5, 0), sc_iterator5_value(it_order, 2), sc_type_arc_pos_const_perm))
                    continue;

                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 1))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 2))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 3))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it_order, 4))))
                    continue;

                appendIntoAnswer(answer, sc_iterator5_value(it_order, 1));
                appendIntoAnswer(answer, sc_iterator5_value(it_order, 2));
                appendIntoAnswer(answer, sc_iterator5_value(it_order, 3));
                appendIntoAnswer(answer, sc_iterator5_value(it_order, 4));

            }
            sc_iterator5_free(it_order);

            // iterate roles of element in link
            it6 = sc_iterator3_a_a_f_new(sc_type_node | sc_type_const,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it2, 1));
            while (sc_iterator3_next(it6) == SC_TRUE)
            {
                sc_memory_get_element_type(sc_iterator3_value(it6, 0), &el_type);
                if (!(el_type & sc_type_node_role))
                    continue;

                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it6, 0))
                                           || IS_SYSTEM_ELEMENT(sc_iterator3_value(it6, 1))))
                    continue;

                appendIntoAnswer(answer, sc_iterator3_value(it6, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it6, 1));
            }
            sc_iterator3_free(it6);

            appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
            appendIntoAnswer(answer, sc_iterator3_value(it2, 2));

            search_subclasses_rec(sc_iterator3_value(it2, 2), answer, sys_off);
        }
        sc_iterator3_free(it2);
    }
    sc_iterator5_free(it5);
    return found;
}

sc_result agent_search_all_subclasses_in_quasybinary_relation(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1;
    sc_bool sys_off = SC_TRUE, found = SC_FALSE;
    const char keynode_sc_agent_of_search_of_all_subclasses_in_quasybinary_relation_c_str[] = "sc_agent_of_search_of_all_subclasses_in_quasybinary_relation_c";

    if (!sc_memory_get_arc_end(arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(keynode_question_search_all_subclasses_in_quasybinary_relation, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    log_agent_started(keynode_sc_agent_of_search_of_all_subclasses_in_quasybinary_relation_c_str);
    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(question, sc_type_arc_pos_const_perm, 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        found = search_subclasses_rec(sc_iterator3_value(it1, 2), answer, sys_off);
    }
    sc_iterator3_free(it1);

    log_agent_finished(keynode_sc_agent_of_search_of_all_subclasses_in_quasybinary_relation_c_str, found);
    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

sc_bool search_superclasses_rec(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_iterator3 *it3;
    sc_iterator5 *it5;
    sc_bool found = SC_FALSE;

    // iterate input arcs
    it3 = sc_iterator3_a_a_f_new(sc_type_node | sc_type_const,
                                 sc_type_arc_pos_const_perm,
                                 elem);
    while (sc_iterator3_next(it3) == SC_TRUE)
    {
        // search all parents in quasybinary relation
        it5 = sc_iterator5_f_a_a_a_a_new(sc_iterator3_value(it3, 0),
                                         sc_type_arc_common | sc_type_const,
                                         sc_type_node | sc_type_const,
                                         sc_type_arc_pos_const_perm,
                                         sc_type_node | sc_type_const);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            // check if it's a quasybinary relation
            if (sc_helper_check_arc(keynode_quasybinary_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 2))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 4))
                                           || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0))
                                           || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1))))
                    continue;

                found = SC_TRUE;

                appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
                appendIntoAnswer(answer, sc_iterator5_value(it5, 2));
                appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
                appendIntoAnswer(answer, sc_iterator5_value(it5, 4));

                appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it3, 1));

                search_superclasses_rec(sc_iterator5_value(it5, 2), answer, sys_off);
            }
        }
        sc_iterator5_free(it5);

    }
    sc_iterator3_free(it3);
    return found;
}

sc_result agent_search_all_superclasses_in_quasybinary_relation(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1;
    sc_bool sys_off = SC_TRUE, found = SC_FALSE;
    const char keynode_sc_agent_of_search_of_all_superclasses_in_quasybinary_relation_c_str[] = "sc_agent_of_search_of_all_superclasses_in_quasybinary_relation_c";

    if (!sc_memory_get_arc_end(arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(keynode_question_search_all_superclasses_in_quasybinary_relation, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    log_agent_started(keynode_sc_agent_of_search_of_all_superclasses_in_quasybinary_relation_c_str);
    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(question, sc_type_arc_pos_const_perm, 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        found = search_superclasses_rec(sc_iterator3_value(it1, 2), answer, sys_off);
    }
    sc_iterator3_free(it1);

    log_agent_finished(keynode_sc_agent_of_search_of_all_superclasses_in_quasybinary_relation_c_str, found);
    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}
