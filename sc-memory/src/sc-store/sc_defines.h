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

#ifndef _sc_defines_h_
#define _sc_defines_h_

#include "sc_config.h"

#define SEGMENT_EMPTY_SEARCH_LEN 1024 // number of element in two directions to search next empty slot in segment
#define SEGMENT_EMPTY_BUFFER_SIZE 8 // number of empty slot buffer for segment
#define SEGMENT_EMPTY_MAX_UPDATE_THREADS 8 // number of maximum threads to update empty slots

#define SEGS_QUEUE_SIZE 24
#define MAX_PATH_LENGTH 1024

#define TASK_QUEUE_SIZE 1024 // amount of mutex for locking task
#define TASK_MAXIMUM_AMOUNT 5 // amount of args for task


#endif
