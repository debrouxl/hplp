/*
 * libhpfiles, libhpcables, libhpcalcs: hand-helds support libraries.
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
 * \file hplibs.h Files, Cables, Calcs: definitions common to libhpfiles, libhpcables, libhpcalcs.
 */

#ifndef __HPLIBS_H__
#define __HPLIBS_H__

#include "export.h"

//! Enumeration of cable types.
typedef enum {
    CABLE_NUL = 0,
    CABLE_PRIME_HID,
    CABLE_PRIME_EMU,
    CABLE_39gII_HID,
    CABLE_39gII_EMU,
    CABLE_MAX
} cable_model;

//! Enumeration of calculator types.
typedef enum {
    CALC_NONE = 0,
    CALC_PRIME,
    CALC_39gII,
    CALC_MAX
} calc_model;

//! Enumeration of logging levels.
typedef enum {
    LOG_LEVEL_ALL = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} hplibs_logging_level;

//! USB Vendor ID of Hewlett-Packard.
#define USB_VID_HP (0x03F0)
//! USB Product ID of the Prime calculator.
#define USB_PID_PRIME (0x0441)
//! USB Product ID of the 39gII calculator.
#define USB_PID_39gII (0xD507)

//! Size of a raw HID packet for the Prime.
#define PRIME_RAW_HID_DATA_SIZE (64)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Gets the error message if the error was produced by a libhp* library
 * \param number the error number (from above)
 * \param message out pointer for a newly allocated text error message, which must be freed by the caller
 *
 * \return the error number.
 **/
HPEXPORT int HPCALL hplibs_error_get(int number, char **message);

#ifdef __cplusplus
}
#endif

#endif
