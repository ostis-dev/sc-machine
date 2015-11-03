/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_structure.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search_defines.h"
#include "search.h"

#include <sc_helper.h>
#include <sc_memory_headers.h>

sc_result agent_search_decomposition(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1, *it2, *it3;
    sc_iterator5 *it5, *it_order;
    sc_bool sys_off = SC_TRUE;
    sc_type el_type;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_decomposition, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                 question,
                                 sc_type_arc_pos_const_perm,
                                 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        // iterate decomposition
        it5 = sc_iterator5_a_a_f_a_a_new(s_default_ctx,
                                         sc_type_node | sc_type_const,
                                         sc_type_arc_common | sc_type_const,
                                         sc_iterator3_value(it1, 2),
                                         sc_type_arc_pos_const_perm,
                                         sc_type_node | sc_type_const);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            if (SC_FALSE == sc_helper_check_arc(s_default_ctx, keynode_decomposition_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm))
                continue;
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 0))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))
                                       || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 4))))
                continue;

            appendIntoAnswer(answer, sc_iterator5_value(it5, 0));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
            appendIntoAnswer(answer, sc_iterator5_value(it5, 4));

            // iterate decomposition set elements
            it2 = sc_iterator3_f_a_a_new(s_default_ctx,
                                         sc_iterator5_value(it5, 0),
                                         sc_type_arc_pos_const_perm,
                                         0);
            while (sc_iterator3_next(it2) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))
                                           || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                    continue;

                // iterate order relations between elements
                it_order = sc_iterator5_f_a_a_a_a_new(s_default_ctx,
                                                      sc_iterator3_value(it2, 2),
                                                      sc_type_arc_common | sc_type_const,
                                                      sc_type_node | sc_type_const,
                                                      sc_type_arc_pos_const_perm,
                                                      sc_type_node | sc_type_const);
                while (sc_iterator5_next(it_order) == SC_TRUE)
                {
                    if (SC_FALSE == sc_helper_check_arc(s_default_ctx, keynode_order_relation, sc_iterator5_value(it_order, 4), sc_type_arc_pos_const_perm))
                        continue;
                    if (SC_FALSE == sc_helper_check_arc(s_default_ctx, sc_iterator5_value(it5, 0), sc_iterator5_value(it_order, 2), sc_type_arc_pos_const_perm))
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
                it3 = sc_iterator3_a_a_f_new(s_default_ctx,
                                             sc_type_node | sc_type_const,
                                             sc_type_arc_pos_const_perm,
                                             sc_iterator3_value(it2, 1));
                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it3, 0), &el_type);
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

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

void add_all_atom_commands(sc_addr curr_command, sc_addr answer)
{
    sc_iterator3 *it3;
    sc_iterator5 *it5;
    it5 = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                     sc_type_const,
                                     sc_type_arc_common | sc_type_const,
                                     curr_command,
                                     sc_type_arc_pos_const_perm,
                                     keynode_nrel_ui_commands_decomposition);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
        it3 = sc_iterator3_f_a_a_new(s_default_ctx,
                                     sc_iterator5_value(it5, 0),
                                     sc_type_arc_pos_const_perm,
                                     sc_type_const | sc_type_node);
        while (sc_iterator3_next(it3) == SC_TRUE)
        {
            if (SC_TRUE == sc_helper_check_arc(s_default_ctx, keynode_ui_user_command_class_atom, sc_iterator3_value(it3, 2), sc_type_arc_pos_const_perm))
            {
                appendIntoAnswer(answer, sc_iterator3_value(it3, 2));
            }
            else
            {
                add_all_atom_commands(sc_iterator3_value(it3, 2), answer);
            }
        }
        sc_iterator3_free(it3);
    }
    sc_iterator5_free(it5);
}

sc_bool check_argument_correspondence(sc_addr argument, sc_addr curr_command, sc_addr arg_type, sc_bool *classes_found)
{
    sc_iterator5 *it_classes;
    sc_bool classes_correspond = SC_FALSE;

    it_classes = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                            curr_command,
                                            sc_type_arc_common | sc_type_const,
                                            sc_type_const,
                                            sc_type_arc_pos_const_perm,
                                            arg_type);
    (*classes_found) = SC_FALSE;
    while (sc_iterator5_next(it_classes) == SC_TRUE)
    {
        (*classes_found) = SC_TRUE;
        if (SC_TRUE == sc_helper_check_arc(s_default_ctx, sc_iterator5_value(it_classes, 2), argument, sc_type_arc_pos_const_perm))
        {
            classes_correspond = SC_TRUE;
            break;
        }
    }
    sc_iterator5_free(it_classes);

    return classes_correspond;
}

void process_arguments_rec(sc_addr *arguments, sc_uint8 arg_count, sc_addr curr_command, sc_addr answer)
{
    sc_iterator3 *it3;
    sc_iterator5 *it5;
    sc_bool classes_found = SC_FALSE;
    sc_bool classes_correspond = SC_FALSE;
    sc_bool command_is_atomic = sc_helper_check_arc(s_default_ctx, keynode_ui_user_command_class_atom, curr_command, sc_type_arc_pos_const_perm);

    if (arg_count == 1)
    {
        if (SC_TRUE == sc_helper_check_arc(s_default_ctx, keynode_ui_two_argument_command_class, curr_command, sc_type_arc_pos_const_perm))
            return;
        classes_correspond = check_argument_correspondence(arguments[0], curr_command, keynode_nrel_argument_type, &classes_found);
    }
    if (arg_count == 2)
    {
        sc_bool classes_found_first, classes_found_second = SC_FALSE;
        sc_bool classes_correspond_first, classes_correspond_second = SC_FALSE;

        if (SC_TRUE == sc_helper_check_arc(s_default_ctx, keynode_ui_one_argument_command_class, curr_command, sc_type_arc_pos_const_perm))
            return;

        classes_correspond_first = check_argument_correspondence(arguments[0], curr_command, keynode_nrel_first_argument_type, &classes_found_first);
        classes_correspond_second = check_argument_correspondence(arguments[1], curr_command, keynode_nrel_second_argument_type, &classes_found_second);

        if ((SC_TRUE == classes_found_first && SC_FALSE == classes_correspond_first)
            || (SC_TRUE == classes_found_second && SC_FALSE == classes_correspond_second))
        {
            classes_found = SC_TRUE;
            classes_correspond = SC_FALSE;
        }
        else
        {
            if ((SC_TRUE == classes_found_first && SC_TRUE == classes_correspond_first)
                && (SC_TRUE == classes_found_second && SC_TRUE == classes_correspond_second))
            {
                classes_found = SC_TRUE;
                classes_correspond = SC_TRUE;
            }
            else
            {
                if ((SC_TRUE == classes_found_first && SC_TRUE == classes_correspond_first && SC_FALSE == classes_found_second)
                    || (SC_TRUE == classes_found_second && SC_TRUE == classes_correspond_second && SC_FALSE == classes_found_first))
                {
                    classes_found = SC_FALSE;
                }
            }
        }
    }

    if (SC_FALSE == classes_found)
    {
        if (SC_TRUE == command_is_atomic)
        {
            appendIntoAnswer(answer, curr_command);
        }
        else
        {
            it5 = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                             sc_type_const,
                                             sc_type_arc_common | sc_type_const,
                                             curr_command,
                                             sc_type_arc_pos_const_perm,
                                             keynode_nrel_ui_commands_decomposition);
            while (sc_iterator5_next(it5) == SC_TRUE)
            {
                it3 = sc_iterator3_f_a_a_new(s_default_ctx,
                                             sc_iterator5_value(it5, 0),
                                             sc_type_arc_pos_const_perm,
                                             sc_type_const | sc_type_node);
                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    process_arguments_rec(arguments, arg_count, sc_iterator3_value(it3, 2), answer);
                }
                sc_iterator3_free(it3);
            }
            sc_iterator5_free(it5);
        }
    }
    else
    {
        if (SC_TRUE == classes_correspond)
        {
            if (SC_TRUE == command_is_atomic)
            {
                appendIntoAnswer(answer, curr_command);
            }
            else
            {
                add_all_atom_commands(curr_command, answer);
            }
        }
    }
}

sc_result agent_search_command_decomposition(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer, args[2];
    sc_uint8 arg_count = 0;
    sc_iterator3 *it1, *it2;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_command_decomposition, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                 question,
                                 sc_type_arc_pos_const_perm,
                                 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        it2 = sc_iterator3_f_a_a_new(s_default_ctx,
                                     sc_iterator3_value(it1, 2),
                                     sc_type_arc_pos_const_perm,
                                     0);
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            args[arg_count] = sc_iterator3_value(it2, 2);
            arg_count++;

            if (arg_count > 1 && SC_TRUE == sc_helper_check_arc(s_default_ctx, keynode_rrel_1, sc_iterator3_value(it1, 1), sc_type_arc_pos_const_perm))
            {
                //Swap arguments
                sc_addr elem = args[0];
                args[0] = args[1];
                args[1] = elem;
                break;
            }
        }
        sc_iterator3_free(it2);

        process_arguments_rec(args, arg_count, keynode_ui_main_menu, answer);

    }
    sc_iterator3_free(it1);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

void search_subclasses_rec(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_iterator3 *it2, *it6;
    sc_iterator5 *it5, *it_order;
    sc_type el_type;

    // iterate taxonomy
    it5 = sc_iterator5_f_a_a_a_a_new(s_default_ctx,
                                     elem,
                                     sc_type_arc_common | sc_type_const,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     sc_type_node | sc_type_const);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
        if (SC_FALSE == sc_helper_check_arc(s_default_ctx, keynode_taxonomy_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm))
            continue;
        if (SC_TRUE == sys_off && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 2))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 4))))
            continue;

        appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 2));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 4));

        search_subclasses_rec(sc_iterator5_value(it5, 2), answer, sys_off);
    }
    sc_iterator5_free(it5);

    // iterate decomposition
    it5 = sc_iterator5_a_a_f_a_a_new(s_default_ctx,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_common | sc_type_const,
                                     elem,
                                     sc_type_arc_pos_const_perm,
                                     sc_type_node | sc_type_const);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
        if (SC_FALSE == sc_helper_check_arc(s_default_ctx, keynode_decomposition_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm))
            continue;

        if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 0))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 4))))
            continue;

        appendIntoAnswer(answer, sc_iterator5_value(it5, 0));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 4));

        // iterate decomposition set elements
        it2 = sc_iterator3_f_a_a_new(s_default_ctx,
                                     sc_iterator5_value(it5, 0),
                                     sc_type_arc_pos_const_perm,
                                     0);
        while (sc_iterator3_next(it2) == SC_TRUE)
        {
            if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))
                                       || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 2))))
                continue;

            // iterate order relations between elements
            it_order = sc_iterator5_f_a_a_a_a_new(s_default_ctx,
                                                  sc_iterator3_value(it2, 2),
                                                  sc_type_arc_common | sc_type_const,
                                                  sc_type_node | sc_type_const,
                                                  sc_type_arc_pos_const_perm,
                                                  sc_type_node | sc_type_const);
            while (sc_iterator5_next(it_order) == SC_TRUE)
            {
                if (SC_FALSE == sc_helper_check_arc(s_default_ctx, keynode_order_relation, sc_iterator5_value(it_order, 4), sc_type_arc_pos_const_perm))
                    continue;
                if (SC_FALSE == sc_helper_check_arc(s_default_ctx, sc_iterator5_value(it5, 0), sc_iterator5_value(it_order, 2), sc_type_arc_pos_const_perm))
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
            it6 = sc_iterator3_a_a_f_new(s_default_ctx,
                                         sc_type_node | sc_type_const,
                                         sc_type_arc_pos_const_perm,
                                         sc_iterator3_value(it2, 1));
            while (sc_iterator3_next(it6) == SC_TRUE)
            {
                sc_memory_get_element_type(s_default_ctx, sc_iterator3_value(it6, 0), &el_type);
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
}

sc_result agent_search_all_subclasses_in_quasybinary_relation(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1;
    sc_bool sys_off = SC_TRUE;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_search_all_subclasses_in_quasybinary_relation, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                 question,
                                 sc_type_arc_pos_const_perm,
                                 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        search_subclasses_rec(sc_iterator3_value(it1, 2), answer, sys_off);
    }
    sc_iterator3_free(it1);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}

void search_superclasses_rec(sc_addr elem, sc_addr answer, sc_bool sys_off)
{
    sc_iterator3 *it3;
    sc_iterator5 *it5;

    // search taxonomy
    it5 = sc_iterator5_a_a_f_a_a_new(s_default_ctx,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_common | sc_type_const,
                                     elem,
                                     sc_type_arc_pos_const_perm,
                                     sc_type_node | sc_type_const);
    while (sc_iterator5_next(it5) == SC_TRUE)
    {
        if (SC_FALSE == sc_helper_check_arc(s_default_ctx, keynode_taxonomy_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm))
            continue;
        if (SC_TRUE == sys_off && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 0))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))
                                   || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 4))))
            continue;

        appendIntoAnswer(answer, sc_iterator5_value(it5, 0));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 1));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 3));
        appendIntoAnswer(answer, sc_iterator5_value(it5, 4));

        search_superclasses_rec(sc_iterator5_value(it5, 0), answer, sys_off);
    }
    sc_iterator5_free(it5);

    // iterate input arcs
    it3 = sc_iterator3_a_a_f_new(s_default_ctx,
                                 sc_type_node | sc_type_const,
                                 sc_type_arc_pos_const_perm,
                                 elem);
    while (sc_iterator3_next(it3) == SC_TRUE)
    {
        // search all parents in quasybinary relation
        it5 = sc_iterator5_f_a_a_a_a_new(s_default_ctx,
                                         sc_iterator3_value(it3, 0),
                                         sc_type_arc_common | sc_type_const,
                                         sc_type_node | sc_type_const,
                                         sc_type_arc_pos_const_perm,
                                         sc_type_node | sc_type_const);
        while (sc_iterator5_next(it5) == SC_TRUE)
        {
            // check if it's a quasybinary relation
            if (sc_helper_check_arc(s_default_ctx, keynode_quasybinary_relation, sc_iterator5_value(it5, 4), sc_type_arc_pos_const_perm) == SC_TRUE)
            {
                if (sys_off == SC_TRUE && (IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 1))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 2))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 3))
                                           || IS_SYSTEM_ELEMENT(sc_iterator5_value(it5, 4))
                                           || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0))
                                           || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1))))
                    continue;


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
}

sc_result agent_search_all_superclasses_in_quasybinary_relation(const sc_event *event, sc_addr arg)
{
    sc_addr question, answer;
    sc_iterator3 *it1;
    sc_bool sys_off = SC_TRUE;

    if (!sc_memory_get_arc_end(s_default_ctx, arg, &question))
        return SC_RESULT_ERROR_INVALID_PARAMS;

    // check question type
    if (sc_helper_check_arc(s_default_ctx, keynode_question_search_all_superclasses_in_quasybinary_relation, question, sc_type_arc_pos_const_perm) == SC_FALSE)
        return SC_RESULT_ERROR_INVALID_TYPE;

    answer = create_answer_node();

    // get operation argument
    it1 = sc_iterator3_f_a_a_new(s_default_ctx,
                                 question,
                                 sc_type_arc_pos_const_perm,
                                 0);
    if (sc_iterator3_next(it1) == SC_TRUE)
    {
        if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
            sys_off = SC_FALSE;

        appendIntoAnswer(answer, sc_iterator3_value(it1, 2));

        search_superclasses_rec(sc_iterator3_value(it1, 2), answer, sys_off);
    }
    sc_iterator3_free(it1);

    connect_answer_to_question(question, answer);
    finish_question(question);

    return SC_RESULT_OK;
}
