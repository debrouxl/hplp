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
 * \file error.h Files / cables / calcs: Error IDs.
 */

#ifndef __HPLIBS_CALCS_ERROR_H__
#define __HPLIBS_CALCS_ERROR_H__

typedef enum {
    ERR_HPLIBS_GENERIC_FIRST = 0,
    ERR_SUCCESS = 0, // Must be equal to ERR_HPLIBS_GENERIC_FIRST
    ERR_MALLOC,
    ERR_INVALID_HANDLE,
    ERR_INVALID_PARAMETER,
    ERR_INVALID_MODEL,
    ERR_LIBRARY_INIT,
    ERR_HPLIBS_GENERIC_LAST = 127,

    ERR_FILE_FIRST = 128,
    ERR_FILE_FILENAME = 128,
    ERR_FILE_LAST = 255,

    ERR_CABLE_FIRST = 256,
    ERR_CABLE_NOT_OPEN = 256,
    ERR_CABLE_OPEN,
    ERR_CABLE_BUSY,
    ERR_CABLE_WRITE_ERROR,
    ERR_CABLE_READ_ERROR,
    ERR_CABLE_INVALID_FNCTS,
    ERR_CABLE_PROBE_FAILED,
    ERR_CABLE_LAST = 383,

    ERR_CALC_FIRST = 384,
    ERR_CALC_NO_CABLE = 384,
    ERR_CALC_CABLE_NOT_OPEN,
    ERR_CALC_BUSY,
    ERR_CALC_INVALID_FNCTS,
    ERR_CALC_PACKET_FORMAT,
    ERR_CALC_SPLIT_TIMESTAMP,
    ERR_CALC_PROBE_FAILED,
    ERR_CALC_LAST = 511,

    ERR_OPER_FIRST = 512,
    ERR_OPER_LAST = 639
} hplibs_error;

#endif
