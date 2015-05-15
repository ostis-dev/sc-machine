/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _merge_h_
#define _merge_h_

#include "sc_memory.h"

extern sc_memory_context * s_default_ctx;


//! Module initialization function
sc_result initialize();

//! Module shutdown function
sc_result shutdown();

#endif
