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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "wsn/errors.h"
#include "wsn/utils.h"

char* wsn_strcat(const char *str1, const char *str2)
{
  int len1 = (int)strlen(str1); 
  int len2 = (int)strlen(str2); 
  char *dst = (char*)malloc(len1 + len2 + 1);
  if (dst) {
    memcpy(dst, str1, len1);
    memcpy(dst + len1, str2, len2 + 1);
  }
  return dst;
}

char* wsn_substr(const char *start, const char *end)
{
  int len = end - start;
  char *dst = (char*)malloc(len + 1);
  if (dst) {
    memcpy(dst, start, len);
    dst[len] = '\0';
  }
  return dst;
}

char* wsn_strdup(const char *str)
{
  char *dst = NULL;
  if (str) {
    int len = (int)strlen(str) + 1;
    dst = (char*)malloc(len);
    if (dst) {
      memcpy(dst, str, len);
    }
  }
  return dst;
}

char* wsn_strdupn(const char *str, int len)
{
  char *dst = NULL;
  if (str) {
    dst = (char*)malloc(len + 1);
    if (dst) {
      memcpy(dst, str, len);
    }
    dst[len] = '\0';
  }
  return dst;
}

char** wsn_string_array_create(const char *str, const char *delimiters)
{
  int count = 0;
  const char **parts = calloc(sizeof(*parts), strlen(str));
  if (parts == NULL) {
    return NULL;
  }
  const char *p = str;
  const char *p1 = str;
  while (*p1) {
    if (strchr(delimiters, *p1)) {
      parts[count] = p;
      count++;
      p = p1 + 1;
    }
    p1++;
  }
  parts[count] = p;
  count++;
  parts[count] = p1 + 1;
  
  char **array = (char**)malloc(sizeof(*array) * (count + 1));
  if (array == NULL) {
    free((void*)parts);
    return NULL;
  }
  for (int i = 0; i < count; i++) {
    array[i] = wsn_strdupn(parts[i], parts[i + 1] - parts[i] - 1);
  }
  array[count] = NULL;
  free((void*)parts);
  return array;
}

void wsn_string_array_cleanup(char **array)
{
  int i = 0;
  while (array[i]) {
    free(array[i]);
    i++;
  }
  free(array);
}

const char wsn_path_sep()
{
#ifdef _WIN32
  return '\\';
#else
  return '/';
#endif
}

char* wsn_path_file_part(const char *path, int with_ext)
{
  const char* f = strrchr(path, wsn_path_sep());
  if (f == NULL) {
    f = path;
  } else {
    f = f + 1;
  }
  const char* e = strrchr(f, '.');
  if (with_ext || e == NULL) {
    e = f + strlen(f);
  }
  return wsn_substr(f, e);
}

char* wsn_read_all(const char *file_name, int *len)
{
  int err = 0;
  FILE *fp = fopen(file_name, "rb");
  if (fp == NULL) {
    return NULL;
  }
  fseek(fp, 0, SEEK_END);
  int size = (int)ftell(fp);
  char* content = malloc(size + 1);
  if (content == NULL) {
    fclose(fp);
    err = WSN_ERR_MALLOC;
    wsn_report_err(err, "wsn_read_all() failed to malloc content buffer");
    return NULL;
  }
  fseek(fp, 0, SEEK_SET);
  if (fread(content, size, 1, fp) <= 0) {
    err = WSN_ERR_FILE_IO;
    wsn_report_err(err, "wsn_read_all() failed to read file");
    free(content);
    fclose(fp);
    return NULL;
  }
  *len = size;
  content[size] = '\0';
  fclose(fp);
  return content;
}

yajl_val wsn_yajl_tree_get(yajl_val node, const char *path, yajl_type type)
{
  char **path_array = wsn_string_array_create(path, ".");
  if (path_array == NULL) {
    wsn_report_err(WSN_ERR_MALLOC, "wsn_yajl_tree_get() failed to create path_array");
    return NULL;
  }
  yajl_val val = yajl_tree_get(node, (const char**)path_array, type);
  wsn_string_array_cleanup(path_array);
  return val;
}
