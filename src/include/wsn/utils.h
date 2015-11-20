/*
 * ws-node - C library to support lightweight websocket based service node
 *
 * Copyright (C) 2015 yquant.com
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#ifndef _WSN_INCL_UTILS_H
#define _WSN_INCL_UTILS_H

#include "yajl/yajl_tree.h"

#include "wsn/defs.h"

#ifdef __cplusplus
extern "C" {
#endif

WSN_EXPORT char* wsn_strcat(const char *str1, const char *str2);
WSN_EXPORT char* wsn_substr(const char *start, const char *end);
WSN_EXPORT char* wsn_strdup(const char *str);
WSN_EXPORT char* wsn_strdupn(const char *str, int len);
WSN_EXPORT char** wsn_string_array_create(const char *str, const char *delimiters);
WSN_EXPORT void wsn_string_array_cleanup(char **array);

WSN_EXPORT const char wsn_path_sep();
WSN_EXPORT char* wsn_path_file_part(const char *path, int with_ext);

WSN_EXPORT char* wsn_read_all(const char *file_name, int *len);

WSN_EXPORT yajl_val wsn_yajl_tree_get(yajl_val node, const char *path, yajl_type type);

#ifdef __cplusplus
}
#endif

#endif // _WSN_INCL_UTILS_H