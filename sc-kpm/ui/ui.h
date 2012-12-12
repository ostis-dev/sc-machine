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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef _ui_h_
#define _ui_h_

#include "sc_memory_headers.h"

typedef enum
{
    UI_KEYNODE_COMMAND_INITIATED = 0,
    UI_KEYNODE_COMMAND_TRANSLATE_FROM_SC,
    UI_KEYNODE_COMMAND_TRANSLATE_TO_SC,

    UI_KEYNODE_COUNT

} ui_keynodes_enum;

//! Module initialization function
sc_result initialize();

//! Module shutdown function
sc_result shutdown();

/*! Get sc-addr of keynode by it code
 * @param keynode Keynode code (one of ui_keynodes value)
 * @return Returns sc-addr of specified \p keynode.
 */
sc_addr ui_get_keynode(ui_keynodes_enum keynode);

#endif