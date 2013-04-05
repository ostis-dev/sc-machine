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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "uiPrecompiled.h"
#include "uiCommands.h"
#include "uiKeynodes.h"
#include "uiDefines.h"


// -------------------- Events ----------------------
sc_event *event_ui_start_answer_translation = 0;
sc_event *event_ui_command_generate_instance = 0;
sc_event *event_ui_remove_displayed_answer = 0;

struct sArcInfo
{
    sc_addr self_addr;
    sc_addr begin_addr;
    sc_addr end_addr;
    sc_type self_type;

    sArcInfo(sc_addr _self, sc_addr _beg, sc_addr _end, sc_type _type)
        : self_addr(_self)
        , begin_addr(_beg)
        , end_addr(_end)
        , self_type(_type)
    {

    }

};

// -------------------- Event handlers --------------
sc_result ui_command_generate_instance(sc_event *event, sc_addr arg)
{
    sc_addr command_addr;
    sc_addr args_addr;
    sc_iterator5 *it5 = (sc_iterator5*)nullptr;
    sc_iterator3 *it3 = (sc_iterator3*)nullptr;

    if (sc_memory_get_arc_end(arg, &command_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // first of all we need to find command arguments
    it5 = sc_iterator5_f_a_a_a_f_new(command_addr,
                                     sc_type_arc_pos_const_perm,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     keynode_rrel_command_arguments);

    if (sc_iterator5_next(it5) != SC_TRUE)
    {
        sc_iterator5_free(it5);
        return SC_RESULT_ERROR;
    }

    args_addr = sc_iterator5_value(it5, 2);
    sc_iterator5_free(it5);


    int idx = 0;
    bool found = true;
    tScAddrVector arguments;
    while (found)
    {
        found = false;
        // iterate arguments and append them into vector
        it5 = sc_iterator5_f_a_a_a_f_new(args_addr,
                                         sc_type_arc_pos_const_perm,
                                         0,
                                         sc_type_arc_pos_const_perm,
                                         ui_keynode_rrel_order[idx]);

        if (sc_iterator5_next(it5) == SC_TRUE)
        {
            arguments.push_back(sc_iterator5_value(it5, 2));
            found = true;
        }
        sc_iterator5_free(it5);
        ++idx;
    }

    // get command class
    it5 = sc_iterator5_f_a_a_a_f_new(command_addr,
                                     sc_type_arc_pos_const_perm,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     keynode_rrel_command);

    if (sc_iterator5_next(it5) != SC_TRUE)
    {
        sc_iterator5_free(it5);
        return SC_RESULT_ERROR;
    }

    sc_addr new_command_class_addr;
    new_command_class_addr = sc_iterator5_value(it5, 2);
    sc_iterator5_free(it5);

    // get command template
    it5 = sc_iterator5_f_a_a_a_f_new(new_command_class_addr,
                                     sc_type_arc_common | sc_type_const,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     keynode_nrel_command_template);

    if (sc_iterator5_next(it5) != SC_TRUE)
    {
        sc_iterator5_free(it5);
        return SC_RESULT_ERROR;
    }

    sc_addr new_command_templ_addr;
    new_command_templ_addr = sc_iterator5_value(it5, 2);
    sc_iterator5_free(it5);

    // collect all elements in command template
    sc_addr templ_item_addr;
    sc_type templ_item_type;
    tScAddrToScAddrMap templ_to_inst;

    typedef std::list < sArcInfo > tTemplArcsList;
    tTemplArcsList templ_arcs;
    it3 = sc_iterator3_f_a_a_new(new_command_templ_addr,
                                 sc_type_arc_pos_const_perm,
                                 0);
    while (sc_iterator3_next(it3) == SC_TRUE)
    {
        templ_item_addr = sc_iterator3_value(it3, 2);
        sc_memory_get_element_type(templ_item_addr, &templ_item_type);

        if (templ_item_type & sc_type_var)
        {
            if (templ_item_type & sc_type_arc_mask)
            {
                // arcs will be processed later
                sc_addr beg_addr, end_addr;
                // todo checks
                sc_memory_get_arc_begin(templ_item_addr, &beg_addr);
                sc_memory_get_arc_end(templ_item_addr, &end_addr);
                templ_arcs.push_back(sArcInfo(templ_item_addr, beg_addr, end_addr, templ_item_type));
            }
            else
            {
                if (templ_item_type & sc_type_node)
                {
                    templ_to_inst[templ_item_addr] = sc_memory_node_new((templ_item_type & ~sc_type_var) | sc_type_const);
                }else
                {
                    if (templ_item_type & sc_type_link)
                        assert("Not supported yet");
                }
            }
        }else
        {
            // check arguments
            bool is_argument = false;
            for (sc_uint32 i = 0; i < UI_ARG_COUNT; ++i)
            {
                if (SC_ADDR_IS_EQUAL(templ_item_addr, ui_keynode_arg[i]))
                {
                    is_argument = true;
                    templ_to_inst[templ_item_addr] = arguments[i];
                    break;
                }
            }

            if (!is_argument)
                templ_to_inst[templ_item_addr] = templ_item_addr;
        }
    }

    // now process arcs
    bool created = true;
    tScAddrToScAddrMap::iterator it_arc_beg, it_arc_end;
    sc_addr arc_addr, arc_beg_addr, arc_end_addr, new_arc_addr;
    sc_type arc_type;
    while (created)
    {
        created = false;
        tTemplArcsList::iterator it, it_end = templ_arcs.end();
        for (it = templ_arcs.begin(); it != it_end; ++it)
        {
            arc_addr = (*it).self_addr;
            arc_beg_addr = (*it).begin_addr;
            arc_end_addr = (*it).end_addr;
            arc_type = (*it).self_type;


            it_arc_beg = templ_to_inst.find(arc_beg_addr);
            it_arc_end = templ_to_inst.find(arc_end_addr);

            // check if arc can be created
            if (it_arc_beg != templ_to_inst.end() && it_arc_end != templ_to_inst.end())
            {
                created = true;
                new_arc_addr = sc_memory_arc_new((arc_type & ~sc_type_var) | sc_type_const, (*it_arc_beg).second, (*it_arc_end).second);
                templ_to_inst[arc_addr] = new_arc_addr;

                it = templ_arcs.erase(it);
            }
        }
    }

    // need to be empty
    assert(templ_arcs.empty());

    // create contour, that contains instance of command
    sc_addr created_instance_addr = sc_memory_node_new(sc_type_node_struct | sc_type_const);
    SYSTEM_ELEMENT(created_instance_addr);
    tScAddrToScAddrMap::iterator it_res, it_res_end = templ_to_inst.end();
    for (it_res = templ_to_inst.begin(); it_res != it_res_end; ++it_res)
    {
        arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, created_instance_addr, (*it_res).second);
        SYSTEM_ELEMENT(arc_addr);
    }

    // generate result for command
    arc_addr = sc_memory_arc_new(sc_type_arc_common | sc_type_const, command_addr, created_instance_addr);
    SYSTEM_ELEMENT(arc_addr);
    arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, keynode_nrel_command_result, arc_addr);
    SYSTEM_ELEMENT(arc_addr);

    // change command state
    sc_memory_element_free(arg);
    arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, keynode_command_finished, command_addr);
    SYSTEM_ELEMENT(arc_addr);

    return SC_RESULT_OK;
}

sc_result ui_start_answer_translation(sc_event *event, sc_addr arg)
{
    sc_addr question_addr;
    sc_addr answer_addr;
    sc_addr author_addr;
    sc_addr output_formats_addr;
    sc_addr format_addr;
    sc_addr trans_command_addr;
    sc_addr arc_addr;
    sc_iterator5 *it5 = (sc_iterator5*)nullptr;
    sc_iterator3 *it3 = (sc_iterator3*)nullptr;


    if (sc_memory_get_arc_end(arg, &question_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    it5 = sc_iterator5_f_a_a_a_f_new(question_addr,
                                     sc_type_arc_common | sc_type_const,
                                     sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     keynode_question_nrel_answer);
    if (sc_iterator5_next(it5) == SC_FALSE)
    {
        sc_iterator5_free(it5);
        return SC_RESULT_ERROR;
    }

    answer_addr = sc_iterator5_value(it5, 2);

    // find author of this question
    it5 = sc_iterator5_f_a_a_a_f_new(question_addr,
                                    sc_type_arc_common | sc_type_const,
                                    sc_type_node | sc_type_const,
                                    sc_type_arc_pos_const_perm,
                                    keynode_nrel_authors);
    if (it5 == nullptr)
        return SC_RESULT_ERROR;

    if (sc_iterator5_next(it5) == SC_TRUE)
    {
        author_addr = sc_iterator5_value(it5, 2);
        sc_iterator5_free(it5);

        // check if author is an user
        if (sc_helper_check_arc(keynode_user, author_addr, sc_type_arc_pos_const_perm) == SC_TRUE)
        {
            // get answer output formats
            it5 = sc_iterator5_f_a_a_a_f_new(question_addr,
                                            sc_type_arc_common | sc_type_const,
                                            sc_type_node | sc_type_const,
                                            sc_type_arc_pos_const_perm,
                                            keynode_nrel_user_answer_formats);
            if (it5 == nullptr)
                return SC_RESULT_ERROR;

            if (sc_iterator5_next(it5) == SC_TRUE)
            {
                output_formats_addr = sc_iterator5_value(it5, 2);
                sc_iterator5_free(it5);

                // list all output formats and initialize translation
                it3 = sc_iterator3_f_a_a_new(output_formats_addr,
                                             sc_type_arc_pos_const_perm,
                                             sc_type_node | sc_type_const);
                if (it3 == nullptr)
                    return SC_RESULT_ERROR;

                while (sc_iterator3_next(it3) == SC_TRUE)
                {
                    format_addr = sc_iterator3_value(it3, 2);

                    // init translation command
                    trans_command_addr = sc_memory_node_new(sc_type_const);
                    SYSTEM_ELEMENT(trans_command_addr);

                    arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, trans_command_addr, answer_addr);
                    SYSTEM_ELEMENT(arc_addr);
                    arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, keynode_rrel_source_sc_construction, arc_addr);
                    SYSTEM_ELEMENT(arc_addr);

                    arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, trans_command_addr, format_addr);
                    SYSTEM_ELEMENT(arc_addr);
                    arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, keynode_rrel_output_format, arc_addr);
                    SYSTEM_ELEMENT(arc_addr);

                    // add into translation command set
                    arc_addr = sc_memory_arc_new(sc_type_arc_pos_const_perm, keynode_command_translate_from_sc, trans_command_addr);
                    SYSTEM_ELEMENT(arc_addr);
                }
                sc_iterator3_free(it3);

            }else
                sc_iterator5_free(it5);

        }
    }else
        sc_iterator5_free(it5);


    // get answer node

    return SC_RESULT_OK;
}

sc_result ui_remove_displayed_answer(sc_event *event, sc_addr arg)
{
    sc_addr answer_addr;
    sc_iterator5 *it5 = 0;
    sc_iterator5 *it5Res = 0;
    sc_iterator5 *it5Args = 0;

    if (sc_memory_get_arc_end(arg, &answer_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    // first of all delete translation command
    it5 = sc_iterator5_a_a_f_a_f_new(sc_type_node | sc_type_const,
                                     sc_type_arc_pos_const_perm,
                                     answer_addr,
                                     sc_type_arc_pos_const_perm,
                                     keynode_rrel_source_sc_construction);
    if (sc_iterator5_next(it5) == SC_TRUE)
        sc_memory_element_free(sc_iterator5_value(it5, 0));
    sc_iterator5_free(it5);

    // remove translation result
    it5 = sc_iterator5_f_a_a_a_f_new(answer_addr,
                                     sc_type_arc_common | sc_type_const,
                                     sc_type_link,
                                     sc_type_arc_pos_const_perm,
                                     keynode_nrel_translation);
    if (sc_iterator5_next(it5) == SC_TRUE)
        sc_memory_element_free(sc_iterator5_value(it5, 2));
    sc_iterator5_free(it5);

    // find question, and remove all connected information
    it5 = sc_iterator5_a_a_f_a_f_new(sc_type_node | sc_type_const,
                                     sc_type_arc_common | sc_type_const,
                                     answer_addr,
                                     sc_type_arc_pos_const_perm,
                                     keynode_question_nrel_answer);
    if (sc_iterator5_next(it5) == SC_TRUE)
    {
        it5Res = sc_iterator5_a_a_f_a_f_new(sc_type_node | sc_type_const,
                                            sc_type_arc_common | sc_type_const,
                                            sc_iterator5_value(it5, 0),
                                            sc_type_arc_pos_const_perm,
                                            keynode_nrel_command_result);

        if (sc_iterator5_next(it5Res) == SC_TRUE)
        {
            it5Args = sc_iterator5_f_a_a_a_f_new(sc_iterator5_value(it5Res, 0),
                                                 sc_type_arc_pos_const_perm,
                                                 sc_type_node | sc_type_const,
                                                 sc_type_arc_pos_const_perm,
                                                 keynode_rrel_command_arguments);
            if (sc_iterator5_next(it5Args) == SC_TRUE)
                sc_memory_element_free(sc_iterator5_value(it5Args, 2));
            sc_iterator5_free(it5Args);

            sc_memory_element_free(sc_iterator5_value(it5Res, 0));
        }
        sc_iterator5_free(it5Res);

        sc_memory_element_free(sc_iterator5_value(it5, 0));
    }
    sc_iterator5_free(it5);

    // now we can remove answer node
    sc_memory_element_free(answer_addr);

    return SC_RESULT_OK;
}

// -------------------- Module ----------------------
sc_result ui_initialize_commands()
{
    event_ui_start_answer_translation = sc_event_new(keynode_question_finished, SC_EVENT_ADD_OUTPUT_ARC, 0, ui_start_answer_translation, 0);
    if (event_ui_start_answer_translation == nullptr)
        return SC_RESULT_ERROR;

    event_ui_command_generate_instance = sc_event_new(keynode_command_initiated, SC_EVENT_ADD_OUTPUT_ARC, 0, ui_command_generate_instance, 0);
    if (event_ui_command_generate_instance == nullptr)
        return SC_RESULT_ERROR;

    event_ui_remove_displayed_answer = sc_event_new(keynode_displayed_answer, SC_EVENT_ADD_OUTPUT_ARC, 0, ui_remove_displayed_answer, 0);
    if (event_ui_remove_displayed_answer == nullptr)
        return SC_RESULT_ERROR;

    return SC_RESULT_OK;
}

void ui_shutdown_commands()
{
    sc_event_destroy(event_ui_start_answer_translation);
    event_ui_start_answer_translation = (sc_event*)nullptr;

    sc_event_destroy(event_ui_command_generate_instance);
    event_ui_command_generate_instance = (sc_event*)nullptr;

    sc_event_destroy(event_ui_remove_displayed_answer);
    event_ui_remove_displayed_answer = (sc_event*)nullptr;
}
