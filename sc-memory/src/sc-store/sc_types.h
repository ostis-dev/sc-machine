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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef _sc_types_h_
#define _sc_types_h_

#include "sc_defines.h"
#include <memory.h>
#include <stdlib.h>

#define nullptr ((void*)0)

// base types
typedef signed char sc_int8;
typedef unsigned char sc_uint8;
typedef signed short sc_int16;
typedef unsigned short sc_uint16;
typedef signed int sc_int32;
typedef unsigned int sc_uint32;
typedef long long sc_int64;
typedef unsigned long long sc_uint64;

typedef sc_uint32 sc_uint;
typedef char sc_char;
typedef unsigned char sc_uchar;

// booleans
enum _sc_bool
{
    SC_FALSE = 0,
    SC_TRUE = 1
};

typedef enum _sc_bool sc_bool;

// types limits
#define SC_MININT8	((sc_int8)  0x80)
#define SC_MAXINT8	((sc_int8)  0x7f)
#define SC_MAXUINT8	((sc_uint8) 0xff)

#define SC_MININT16     ((sc_int16)  0x8000)
#define SC_MAXINT16     ((sc_int16)  0x7fff)
#define SC_MAXUINT16	((sc_uint16) 0xffff)

#define SC_MININT32     ((sc_int32)  0x80000000)
#define SC_MAXINT32     ((sc_int32)  0x7fffffff)
#define SC_MAXUINT32	((sc_uint32) 0xffffffff)

#define SC_ADDR_SEG_MAX     SC_MAXUINT16
#define SC_ADDR_OFFSET_MAX  SC_MAXUINT16

#define SEGMENT_SIZE        SC_MAXUINT16    // number of elements in segment

// Types for segment and offset
typedef sc_uint16 sc_addr_seg;
typedef sc_uint16 sc_addr_offset;

//! Structure to store sc-element address
struct _sc_addr
{
#if USE_NETWORK_SCALE
    sc_uint32 net_addr;
#endif
    sc_addr_seg seg;
    sc_addr_offset offset;
};

//! Make sc-addr empty
#define SC_ADDR_MAKE_EMPTY(addr) { (addr).seg = 0; (addr).offset = 0; }
//! Check if specified sc-addr is empty
#define SC_ADDR_IS_EMPTY(addr) (((addr).seg == 0) && ((addr).offset == 0))
#define SC_ADDR_IS_NOT_EMPTY(addr) (!SC_ADDR_IS_EMPTY(addr))
//! Check if two sc-addr's are equivalent
#define SC_ADDR_IS_EQUAL(addr, addr2) (((addr).seg == (addr2).seg) && ((addr).offset == (addr2).offset))
#define SC_ADDR_IS_NOT_EQUAL(addr, addr2) (!SC_ADDR_IS_EQUAL(addr, addr2))

/*! Next defines help to pack local part of sc-addr (segment and offset) into int value
 * and get them back from int
 */
#define SC_ADDR_LOCAL_TO_INT(addr) (sc_uint32)(((addr).seg << 16) | ((addr).offset & 0xffff))
#define SC_ADDR_LOCAL_OFFSET_FROM_INT(v) (sc_uint16)((v) & 0x0000ffff)
#define SC_ADDR_LOCAL_SEG_FROM_INT(v) SC_ADDR_LOCAL_OFFSET_FROM_INT(v >> 16)

typedef sc_uint16 sc_type;

// sc-element types
#define sc_type_node        0x1
#define sc_type_link        0x2
#define sc_type_edge_common 0x4
#define sc_type_arc_common  0x8
#define sc_type_arc_access  0x10

// sc-element constant
#define sc_type_const       0x20
#define sc_type_var         0x40

// sc-element positivity
#define sc_type_arc_pos         0x80
#define sc_type_arc_neg         0x100
#define sc_type_arc_fuz         0x200

// sc-element premanently
#define sc_type_arc_temp        0x400
#define sc_type_arc_perm        0x800

// struct node types
#define sc_type_node_tuple       (0x80)
#define sc_type_node_struct      (0x100)
#define sc_type_node_role        (0x200)
#define sc_type_node_norole      (0x400)
#define sc_type_node_class       (0x800)
#define sc_type_node_abstract    (0x1000)
#define sc_type_node_material    (0x2000)

#define sc_type_arc_pos_const_perm (sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_perm)

// type mask
#define sc_type_element_mask     (sc_type_node | sc_type_link | sc_type_edge_common | sc_type_arc_common | sc_type_arc_access)
#define sc_type_constancy_mask   (sc_type_const | sc_type_var)
#define sc_type_positivity_mask  (sc_type_arc_pos | sc_type_arc_neg | sc_type_arc_fuz)
#define sc_type_permanency_mask  (sc_type_arc_perm | sc_type_arc_temp)
#define sc_type_node_struct_mask (sc_type_node_tuple | sc_type_node_struct | sc_type_node_role | sc_type_node_norole | sc_type_node_class | sc_type_node_abstract | sc_type_node_material)
#define sc_type_arc_mask         (sc_type_arc_access | sc_type_arc_common | sc_type_edge_common)

// results
enum _sc_result
{
    SC_RESULT_ERROR = 0,               // unknown error
    SC_RESULT_OK = 1,                  // no any error
    SC_RESULT_ERROR_INVALID_PARAMS,    // invalid function parameters error
    SC_RESULT_ERROR_INVALID_TYPE,      // invalied type error
    SC_RESULT_ERROR_IO,                // input/output error
    SC_RESULT_ERROR_INVALID_STATE,     // invalid state of processed object
    SC_RESULT_ERROR_NOT_FOUND          // item not found
};

// contents
#define SC_MAX_CHECKSUM_LEN     32
//! Structure to store checksum informaiton
struct _sc_check_sum
{
    char data[SC_MAX_CHECKSUM_LEN];  // maximum checksum length
    sc_uint8 len;    // checksum length
};

// events
enum _sc_event_type
{
    SC_EVENT_ADD_OUTPUT_ARC = 0,
    SC_EVENT_ADD_INPUT_ARC,
    SC_EVENT_REMOVE_OUTPUT_ARC,
    SC_EVENT_REMOVE_INPUT_ARC,
    SC_EVENT_REMOVE_ELEMENT
};

// structure to store statistics info
struct _sc_stat
{
    sc_uint64 node_count; // amount of all sc-nodes stored in memory
    sc_uint64 arc_count; // amount of all sc-arcs stored in memory
    sc_uint64 link_count; // amount of all sc-links stored in memory

    sc_uint64 node_live_count; // amount of sc-nodes, that wasn't deleted
    sc_uint64 arc_live_count; // amount of sc-arcs, that wasn't deleted
    sc_uint64 link_live_count; // amount of sc-links, that wasn't deleted

    sc_uint64 empty_count; // amount of empty sc-element cells
};

typedef struct _sc_check_sum sc_check_sum;
typedef struct _sc_arc  sc_arc;
typedef struct _sc_content sc_content;
typedef struct _sc_arc_info sc_arc_info;
typedef struct _sc_element sc_element;
typedef struct _sc_segment sc_segment;
typedef struct _sc_addr sc_addr;
typedef struct _sc_elements_stat sc_elements_stat;
typedef struct _sc_iterator_param sc_iterator_param;
typedef struct _sc_iterator3 sc_iterator3;
typedef struct _sc_event sc_event;
typedef enum _sc_result sc_result;
typedef enum _sc_event_type sc_event_type;
typedef struct _sc_stat sc_stat;


#endif
