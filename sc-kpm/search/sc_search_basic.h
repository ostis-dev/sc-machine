/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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

#ifndef SC_SEARCH_BASIC_H
#define SC_SEARCH_BASIC_H

#include "sc_memory.h"

extern sc_addr class_question_all_output_const_pos_arc;
extern sc_addr class_question_all_input_const_pos_arc;
extern sc_addr class_question_all_output_const_pos_arc_with_rel;
extern sc_addr class_question_all_input_const_pos_arc_with_rel;
extern sc_addr class_question_full_semantic_neighbourhood;
extern sc_addr nrel_answer;
extern sc_addr question_finished;
extern sc_addr question_initiated;

//! Module initialization function
sc_result initialize();

//! Module shutdown function
sc_result shutdown();

#endif // SC_SEARCH_BASIC_H
