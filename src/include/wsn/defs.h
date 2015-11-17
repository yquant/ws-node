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

#ifndef _WSN_INCL_DEFS_H
#define _WSN_INCL_DEFS_H

#if defined(USING_WSN_SHARED)
  #if defined(WIN32)

   #if defined(WSN_IMPLEMENTATION)
    #define WSN_EXPORT __declspec(dllexport)
    #define WSN_EXPORT_PRIVATE __declspec(dllexport)
   #else
    #define WSN_EXPORT __declspec(dllimport)
    #define WSN_EXPORT_PRIVATE __declspec(dllimport)
   #endif  // defined(WSN_IMPLEMENTATION)
   
  #else  // defined(WIN32)

   #if defined(WSN_IMPLEMENTATION)
    #define WSN_EXPORT __attribute__((visibility("default")))
    #define WSN_EXPORT_PRIVATE __attribute__((visibility("default")))
   #else
    #define WSN_EXPORT
    #define WSN_EXPORT_PRIVATE
   #endif  // defined(WSN_IMPLEMENTATION)
   
  #endif  // !defined(WIN32)
#else // defined(USING_WSN_SHARED)
  #define WSN_EXPORT
  #define WSN_EXPORT_PRIVATE
#endif

#define CONTAINER_OF(ptr, type, field)                                        \
  ((type *) ((char *) (ptr) - ((char *) &((type *) 0)->field)))

#endif // _WSN_INCL_DEFS_H