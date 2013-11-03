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

//! Enumeration of cable types.
typedef enum {
    CABLE_NONE = 0,
    CABLE_PRIME_HID,
    CABLE_MAX
} cable_model;

//! Enumeration of calculator types.
typedef enum {
    CALC_NONE = 0,
    CALC_PRIME,
    CALC_MAX
} calc_model;

//! USB Vendor ID of Hewlett-Packard.
#define USB_VID_HP (0x03F0)
//! USB Product ID of the Prime calculator.
#define USB_PID_PRIME (0x0441)

//! Size of a raw packet for the Prime.
#define PRIME_RAW_DATA_SIZE (64)


#endif
