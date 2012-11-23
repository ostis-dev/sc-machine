/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

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

#include "sc_link_helpers.h"

#include <stdlib.h>
#include <memory.h>
#include <glib.h>

#define SC_DEFAULT_CHECKSUM G_CHECKSUM_SHA256

sc_bool sc_link_calculate_checksum(const sc_stream *stream, sc_check_sum *check_sum)
{
    sc_char buffer[1024];
    sc_uint32 data_read;
    const gchar *result = 0;
    GChecksum *checksum = g_checksum_new(SC_DEFAULT_CHECKSUM);

    g_assert(stream != 0);
    g_assert(check_sum != 0);
    g_checksum_reset(checksum);

    sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

    while (sc_stream_eof(stream) == SC_FALSE)
    {
        if (sc_stream_read_data(stream, buffer, 1024, &data_read) == SC_ERROR)
        {
            g_checksum_free(checksum);
            return SC_FALSE;
        }

        g_checksum_update(checksum, (guchar*)buffer, data_read);
    }

    // store results
    check_sum->len = g_checksum_type_get_length(SC_DEFAULT_CHECKSUM);
    result = g_checksum_get_string(checksum);
    memcpy(&(check_sum->data[0]), result, check_sum->len);

    g_checksum_free(checksum);

    sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

    return SC_TRUE;
}



