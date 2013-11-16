/*
 * libhpfiles: hand-helds support libraries.
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
 * \file typesprime.h Files: Prime file types and utility functions.
 */

#ifndef __HPLIBS_FILES_TYPESPRIME_H__
#define __HPLIBS_FILES_TYPESPRIME_H__

#define PRIME_TYPE_SETTINGS (0x00)
// 0x01 ?
#define PRIME_TYPE_APP (0x02)
#define PRIME_TYPE_LIST (0x03) // variables L0-L9, for instance
#define PRIME_TYPE_MATRIX (0x04) // variables M0-M9, for instance
#define PRIME_TYPE_NOTE (0x05)
#define PRIME_TYPE_PRGM (0x06) // programs with identical data are sent twice during backup ??
#define PRIME_TYPE_APPNOTE (0x07) // XXX Tentative
#define PRIME_TYPE_APPPRGM (0x08) // XXX Tentative
#define PRIME_TYPE_COMPLEX (0x09) // variables Z0-Z9, for instance
#define PRIME_TYPE_REAL (0x0A) // variables A-Z, 0x3B8 (theta), for instance
#define PRIME_TYPE_TESTMODECONFIG (0x0B)
#define PRIME_TYPE_UNKNOWN (0xFF)

//! Return the string corresponding to the file type ID
const char * prime_vartype2str(uint8_t type);

//! Return the type ID corresponding to the string
uint8_t prime_str2vartype(const char * type);

//! Return the file extension corresponding to the value
const char * prime_byte2fext(uint8_t type);

//! Return the type value corresponding to the file extension
uint8_t prime_fext2byte(const char * type);

//! Return the type value corresponding to the file path
uint8_t prime_filename2byte(const char * filepath);

//! Parse the file path and determines the type value and calculator-side filename
int prime_parsefilename(const char * filepath, uint8_t * out_type, char ** out_calcfilename);

#endif
