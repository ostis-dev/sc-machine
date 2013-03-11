/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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
#include "search_input_arcs.h"
#include "search_keynodes.h"
#include "search_utils.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>



sc_result agent_search_all_const_pos_input_arc(sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2;

    if (!sc_memory_get_arc_end(arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(keynode_question_all_input_const_pos_arc, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // find argument
    it1 = sc_iterator3_f_a_a_new(question, sc_type_arc_pos_const_perm, 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        // iterate input arcs
        it2 = sc_iterator3_a_a_f_new(0, sc_type_arc_pos_const_perm, sc_iterator3_value(it1, 2));
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            appendIntoAnswer(answer, sc_iterator3_value(it2, 0));
            appendIntoAnswer(answer, sc_iterator3_value(it2, 1));
        }
        sc_iterator3_free(it2);
    }
    sc_iterator3_free(it1);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

// ---------------------------------------------------

sc_result agent_search_all_const_pos_input_arc_with_rel(sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2, *it3;

    if (!sc_memory_get_arc_end(arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(keynode_question_all_input_const_pos_arc_with_rel, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get question argument
    it1 = sc_iterator3_f_a_a_new(question, sc_type_arc_pos_const_perm, 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        // iterate input arcs
        it2 = sc_iterator3_a_a_f_new(0, sc_type_arc_pos_const_perm, sc_iterator3_value(it1, 2));
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            appendIntoAnswer( answer, sc_iterator3_value(it2, 0));
            appendIntoAnswer(answer, sc_iterator3_value(it2, 1));

            // iterate relations
            it3 = sc_iterator3_a_a_f_new(0, sc_type_arc_pos_const_perm, sc_iterator3_value(it2, 1));
            while (sc_iterator3_next(it3) == SC_TRUE)
            {
                appendIntoAnswer(answer, sc_iterator3_value(it3, 0));
                appendIntoAnswer(answer, sc_iterator3_value(it3, 1));
            }
            sc_iterator3_free(it3);
        }
        sc_iterator3_free(it2);
    }
    sc_iterator3_free(it1);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

