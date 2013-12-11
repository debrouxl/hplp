/*
 * libhpcables: hand-helds support libraries.
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
 * \file link_prime_hid.h Cables: Prime HID cable.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hplibs.h>
#include <hpcalcs.h>

static int cable_nul_open (cable_handle * handle) {
    return 0;
}

static int cable_nul_close (cable_handle * handle) {
    return 0;
}

static int cable_nul_set_read_timeout (cable_handle * handle, int read_timeout) {
    return 0;
}


static int cable_nul_send (cable_handle * handle, uint8_t * data, uint32_t len) {
    return 0;
}

static int cable_nul_recv (cable_handle * handle, uint8_t * data, uint32_t * len) {
    return 0;
}

const cable_fncts cable_nul_fncts =
{
    CABLE_NUL,
    "Dummy cable",
    "Dummy cable used when no cable is set",
    &cable_nul_open,
    &cable_nul_close,
    &cable_nul_set_read_timeout,
    &cable_nul_send,
    &cable_nul_recv
};
