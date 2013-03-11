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

#ifndef _ui_keynodes_h_
#define _ui_keynodes_h_

extern "C"
{
#include "sc_memory_headers.h"
}

extern sc_addr keynode_user;
extern sc_addr keynode_command_translate_from_sc;
extern sc_addr keynode_nrel_user_answer_formats;
extern sc_addr keynode_rrel_source_sc_construction;
extern sc_addr keynode_rrel_output_format;

extern sc_addr keynode_question_nrel_answer;
extern sc_addr keynode_question_finished;
extern sc_addr keynode_nrel_authors;
extern sc_addr keynode_nrel_translation;

extern sc_addr keynode_command_generate_instance;
extern sc_addr keynode_command_initiated;
extern sc_addr keynode_command_finished;
extern sc_addr keynode_rrel_command_arguments;
extern sc_addr keynode_rrel_command;
extern sc_addr keynode_nrel_command_template;
extern sc_addr keynode_nrel_command_result;
extern sc_addr keynode_displayed_answer;

extern sc_addr keynode_format_scs_json;
extern sc_addr keynode_format_scg_json;
extern sc_addr keynode_format_scn_json;

extern sc_addr keynode_system_element;

#define RREL_ORDER_COUNT 10
extern sc_addr ui_keynode_rrel_order[RREL_ORDER_COUNT];
#define UI_ARG_COUNT 10
extern sc_addr ui_keynode_arg[UI_ARG_COUNT];

//! Initialize all keynodes, that will be used in extension
sc_bool initialize_keynodes();

#endif
