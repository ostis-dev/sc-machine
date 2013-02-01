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

#include "uiPrecompiled.h"
#include "uiTranslatorFromSc.h"
#include "uiKeynodes.h"

uiTranslateFromSc::uiTranslateFromSc()
{

}

uiTranslateFromSc::~uiTranslateFromSc()
{

}

void uiTranslateFromSc::translate(const sc_addr &input_addr, const sc_addr &format_addr)
{
    mInputConstructionAddr = input_addr;
    mOutputFormatAddr = format_addr;

    collectObjects();

    runImpl();

    // write into sc-link
    sc_stream *result_data_stream = 0;
    result_data_stream = sc_stream_memory_new(mOutputData.c_str(), mOutputData.size(), SC_STREAM_READ, SC_FALSE);

    sc_addr result_addr = sc_memory_link_new();
    sc_memory_set_link_content(result_addr, result_data_stream);

    sc_stream_free(result_data_stream);

    // generate translation
    sc_addr arc_addr = sc_memory_arc_new(sc_type_arc_common | sc_type_const, mInputConstructionAddr, result_addr);
    sc_memory_arc_new(sc_type_arc_pos_const_perm, ui_keynode_nrel_translation, arc_addr);
}

void uiTranslateFromSc::collectObjects()
{
    sc_iterator3 *it = sc_iterator3_f_a_a_new(mInputConstructionAddr, sc_type_arc_pos_const_perm, 0);
    while (sc_iterator3_next(it) == SC_TRUE)
    {
        sc_type el_type = 0;
        sc_addr addr = sc_iterator3_value(it, 2);

        //! TODO add error logging
        if (sc_memory_get_element_type(addr, &el_type) != SC_RESULT_OK)
            continue;

        if (el_type & sc_type_node)
        {
            mNodes[addr] = el_type;
            continue;
        }

        if (el_type & sc_type_arc_mask)
        {
            mArcs[addr] = el_type;
            continue;
        }

        if (el_type & sc_type_link)
        {
            mLinks[addr] = el_type;
            continue;
        }

    }
    sc_iterator3_free(it);
}

bool uiTranslateFromSc::isNeedToTranslate(const sc_addr &addr) const
{
    if (mNodes.find(addr) != mNodes.end())
        return true;

    if (mArcs.find(addr) != mArcs.end())
        return true;

    if (mLinks.find(addr) != mLinks.end())
        return true;

    return false;
}

String uiTranslateFromSc::buildId(const sc_addr &addr) const
{
    StringStream ss;
    ss << addr.seg << "_" << addr.offset;
    return ss.str();
}
