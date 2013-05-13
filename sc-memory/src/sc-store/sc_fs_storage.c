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

#include "sc_fs_storage.h"
#include "sc_segment.h"
#include "sc_stream_file.h"
#include "sc_config.h"

#include <stdlib.h>
#include <memory.h>
#include <glib.h>

gchar *repo_path = 0;

const gchar *seg_dir = "segments";
const gchar *content_dir = "contents";
const gchar *addr_key_group = "addrs";

gchar segments_path[MAX_PATH_LENGTH + 1];
gchar contents_path[MAX_PATH_LENGTH + 1];

#define SC_DIR_PERMISSIONS -1

sc_bool sc_fs_storage_initialize(const gchar *path)
{
    g_message("Initialize sc-storage from path: %s\n", path);

    g_snprintf(segments_path, MAX_PATH_LENGTH, "%s/%s", path, seg_dir);
    g_snprintf(contents_path, MAX_PATH_LENGTH, "%s/%s", path, content_dir);

    if (!g_file_test(contents_path, G_FILE_TEST_IS_DIR))
    {
        if (g_mkdir_with_parents(contents_path, SC_DIR_PERMISSIONS) < 0)
            g_error("Can't create '%s' directory.", contents_path);
    }

    repo_path = g_malloc0(sizeof(gchar) * (strlen(path) + 1));
    g_stpcpy(repo_path, path);

    return SC_TRUE;
}

sc_bool sc_fs_storage_shutdown(sc_segment **segments)
{
    g_message("Shutdown sc-storage\n");
    g_message("Write storage into %s", repo_path);
    sc_fs_storage_write_to_path(segments);

    return SC_TRUE;
}

void _get_segment_path(const gchar *path, 
                       sc_uint id,
                       sc_uint max_path_len,
                       gchar *res)
{
    g_snprintf(res, max_path_len, "%s/%.10d", path, id);
}

sc_segment* sc_fs_storage_load_segment(sc_uint id)
{
    sc_segment *segment = sc_segment_new(id);
    gchar file_name[MAX_PATH_LENGTH + 1];
    sc_uint length;

    _get_segment_path(segments_path, id, MAX_PATH_LENGTH, file_name);
    g_assert( g_file_get_contents(file_name, (gchar**)(&segment), &length, 0) );
    g_assert( length == sizeof(sc_segment) );

    return segment;
}

sc_bool sc_fs_storage_read_from_path(sc_segment **segments, sc_uint16 *segments_num)
{
    const gchar *fname = 0;
    sc_uint files_count = 0, idx, to_load;
    GDir *dir = 0;

    if (!g_file_test(repo_path, G_FILE_TEST_IS_DIR))
    {
        g_error("%s isn't a directory.", repo_path);
        return SC_FALSE;
    }

    if (!g_file_test(segments_path, G_FILE_TEST_IS_DIR))
    {
        g_message("There are no segments directory in %s", repo_path);
        return SC_FALSE;
    }

    dir = g_dir_open(segments_path, 0, 0);
    g_assert( dir != (GDir*)0 );

    // calculate files
    fname = g_dir_read_name(dir);
    while (fname != 0)
    {
        if (!g_file_test(fname, G_FILE_TEST_IS_DIR))
            files_count++;
        fname = g_dir_read_name(dir);
    }

    g_message("Segments found: %u", files_count);
    *segments_num = files_count;

    to_load = MIN(files_count, sc_config_get_max_loaded_segments());
    // load segments
    for (idx = 0; idx < to_load; idx++)
        segments[idx] = (gpointer)sc_fs_storage_load_segment(idx);

    g_message("Segments loaded: %u", idx);

    g_dir_close(dir);
    return SC_TRUE;
}

sc_bool sc_fs_storage_write_to_path(sc_segment **segments)
{
    sc_uint idx = 0;
    const sc_segment *segment = 0;
    gchar file_name[MAX_PATH_LENGTH + 1];
    gchar segments_path[MAX_PATH_LENGTH + 1];

    if (!g_file_test(repo_path, G_FILE_TEST_IS_DIR))
    {
        g_error("%s isn't a directory.", repo_path);
        return SC_FALSE;
    }

    // check if segments directory exists, if it doesn't, then create one
    g_snprintf(segments_path, MAX_PATH_LENGTH, "%s/%s", repo_path, seg_dir);
    if (!g_file_test(segments_path, G_FILE_TEST_IS_DIR))
    {
        if (g_mkdir_with_parents(segments_path, SC_DIR_PERMISSIONS) < 0)
            return SC_FALSE;
    }

    for (idx = 0; idx < SC_ADDR_SEG_MAX; idx++)
    {
        segment = segments[idx];
        if (segment == nullptr) continue; // skip null segments

        _get_segment_path(segments_path, idx, MAX_PATH_LENGTH, file_name);
        g_file_set_contents(file_name, (gchar*)segment, sizeof(sc_segment), 0);
    }

    _get_segment_path(segments_path, idx, MAX_PATH_LENGTH, file_name);
    while (g_file_test(file_name, G_FILE_TEST_EXISTS))
    {
        g_remove(file_name);
        _get_segment_path(segments_path, ++idx, MAX_PATH_LENGTH, file_name);
    }


    return SC_TRUE;
}


sc_result sc_fs_storage_write_content(sc_addr addr, const sc_check_sum *check_sum, const sc_stream *stream)
{
    sc_uint8 *path = sc_fs_storage_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar data_path[MAX_PATH_LENGTH];
    sc_char buffer[1024];
    sc_uint32 data_read, data_write;
    sc_stream *out_stream = 0;

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(data_path, MAX_PATH_LENGTH, "%sdata", abs_path);

    // check if specified path exist
    if (g_file_test(data_path, G_FILE_TEST_EXISTS))
    {
        //g_message("Content data file '%s' already exist", data_path);
        sc_fs_storage_add_content_addr(addr, check_sum);
        free(path);
        return SC_RESULT_OK; // do nothing, file saved
    }

    // file doesn't exist, so we need to save it
    if (g_mkdir_with_parents(abs_path, SC_DIR_PERMISSIONS) < 0)
    {
        g_message("Error while creating '%s' directory", abs_path);
        free(path);
        return SC_RESULT_ERROR_IO;
    }

    // write content into file
    out_stream = sc_stream_file_new(data_path, SC_STREAM_WRITE);
    if (out_stream != 0)
    {
        // reset input stream positon to begin
        sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

        while (sc_stream_eof(stream) == SC_FALSE)
        {
            if (sc_stream_read_data(stream, buffer, 1024, &data_read) == SC_RESULT_ERROR)
            {
                sc_stream_free(out_stream);
                return SC_RESULT_ERROR;
            }

            if (sc_stream_write_data(out_stream, buffer, data_read, &data_write) == SC_RESULT_ERROR)
            {
                sc_stream_free(out_stream);
                return SC_RESULT_ERROR;
            }

            if (data_read != data_write)
            {
                sc_stream_free(out_stream);
                return SC_RESULT_ERROR;
            }
        }
        sc_stream_free(out_stream);

        return sc_fs_storage_add_content_addr(addr, check_sum);
    }

    return SC_RESULT_ERROR_IO;
}

sc_result sc_fs_storage_add_content_addr(sc_addr addr, const sc_check_sum *check_sum)
{
    sc_uint8 *path = sc_fs_storage_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar addr_path[MAX_PATH_LENGTH];
    gchar *content = 0;
    gchar *content2 = 0;
    gsize content_len = 0;
    sc_uint32 addrs_num = 0;

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(addr_path, MAX_PATH_LENGTH, "%saddrs", abs_path);

    // try to load existing file
    if (g_file_test(addr_path, G_FILE_TEST_EXISTS))
    {
        if (g_file_get_contents(addr_path, &content, &content_len, 0) == FALSE)
        {
            if (content != 0)
                free(content);
            free(path);
            return SC_RESULT_ERROR_IO;
        }
    }

    // append addr into content
    if (content == 0)
    {
        content_len = sizeof(sc_addr) + sizeof(sc_uint32);
        content = g_new0(gchar, content_len);

        addrs_num = 1;
        memcpy(content, &addrs_num, sizeof(addrs_num));
        memcpy(content + sizeof(addrs_num), &addr, sizeof(addr));
    }else
    {
        content2 = content;
        (*(sc_uint32*)content2)++;

        content = g_new0(gchar, content_len + sizeof(addr));
        memcpy(content, content2, content_len);
        memcpy(content + content_len, &addr, sizeof(addr));
        content_len += sizeof(addr);

        // old content doesn't need
        g_free(content2);
    }

    // write content to file
    if (g_file_set_contents(addr_path, content, content_len, 0) == TRUE)
    {
        g_free(content);
        free(path);

        return SC_RESULT_OK;
    }

    g_free(content);
    free(path);

    return SC_RESULT_ERROR;
}

sc_result sc_fs_storage_find_links_with_content(const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count)
{

    sc_uint8 *path = sc_fs_storage_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar addr_path[MAX_PATH_LENGTH];
    gchar *content = 0, *content2 = 0;
    gsize content_len = 0;

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(addr_path, MAX_PATH_LENGTH, "%saddrs", abs_path);

    // must be a null pointer
    g_assert(*result == 0);

    *result_count = 0;

    // try to load existing file
    if (g_file_test(addr_path, G_FILE_TEST_EXISTS))
    {
        if (g_file_get_contents(addr_path, &content, &content_len, 0) == FALSE)
        {
            if (content != 0)
                free(content);
            free(path);
            return SC_RESULT_ERROR_IO;
        }

    }

    // store result
    if (content == 0)
    {
        *result = 0;
        *result_count = 0;
    }else
    {
        *result_count = *((sc_uint32*)content);
        content2 = content;
        content2 += sizeof(sc_uint32);

        *result = g_new0(sc_addr, *result_count);
        memcpy(*result, content2, sizeof(sc_addr) * (*result_count));
    }

    if (content != 0)
        g_free(content);
    free(path);

    return SC_RESULT_OK;
}

sc_result sc_fs_storage_get_checksum_content(const sc_check_sum *check_sum, sc_stream **stream)
{
    sc_uint8 *path = sc_fs_storage_make_checksum_path(check_sum);
    gchar abs_path[MAX_PATH_LENGTH];
    gchar data_path[MAX_PATH_LENGTH];

    // make absolute path to content directory
    g_snprintf(abs_path, MAX_PATH_LENGTH, "%s/%s", contents_path, path);
    g_snprintf(data_path, MAX_PATH_LENGTH, "%sdata", abs_path);

    // check if specified path exist
    if (g_file_test(data_path, G_FILE_TEST_EXISTS))
    {
        *stream = sc_stream_file_new(data_path, SC_STREAM_READ);
        g_assert(*stream != nullptr);
        free(path);
        return SC_RESULT_OK; // do nothing, file saved
    }

    return SC_RESULT_ERROR;
}

sc_uint8* sc_fs_storage_make_checksum_path(const sc_check_sum *check_sum)
{
    // calculate output string length and create it
    sc_uint div = (check_sum->len % 8 == 0) ? 8 : 4;
    sc_uint len = check_sum->len + check_sum->len / div + 1;
    sc_uint8 *result = malloc(sizeof(sc_uint8) * len);
    sc_uint idx = 0;
    sc_uint j = 0;

    g_assert(check_sum->len == 32);

    g_assert(check_sum != 0);
    g_assert(check_sum->len != 0);
    g_assert(check_sum->len % 4 == 0);

    for (idx = 0; idx < check_sum->len; idx++)
    {
        result[j++] = check_sum->data[idx];
        if ((idx + 1 ) % div == 0)
            result[j++] = '/';
    }

    g_assert(j == (len - 1));

    result[len - 1] = 0;

    return result;
}


