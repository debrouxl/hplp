/*
 * libhpfiles, libhpcables, libhpcalcs - hand-helds support library
 * Copyright (C) 2013 Lionel Debroux
 * Code patterns and snippets borrowed from libticables & libticalcs:
 * Copyright (C) 1999-2009 Romain Liévin
 * Copyright (C) 2009-2013 Lionel Debroux
 * Copyright (C) 1999-2013 libti* contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * \file utils.h Files / Cables / Calcs: utility functions, e.g. UTF-16LE handling.
 */

#ifndef __HPLIBS_UTILS_H__
#define __HPLIBS_UTILS_H__

//! Plain C equivalent of char_traits<char16_t>::length.
uint32_t char16_strlen(char16_t * str);
//! strncpy applied to char16_t.
char16_t * char16_strncpy(char16_t * dst, const char16_t * src, uint32_t n);
//! Hex dumping function.
void hexdump(const char * direction, uint8_t *data, uint32_t size, uint32_t level);

#endif
