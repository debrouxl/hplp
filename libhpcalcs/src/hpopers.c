/*
 * libhpopers: hand-helds support libraries.
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
 * \file hpfiles.c Higher-level operations: base part.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <inttypes.h>
#include <string.h>

#include <hpopers.h>
#include "logging.h"
#include "error.h"
#include "gettext.h"

// not static, must be shared between instances
int hpoers_instance_count = 0;

HPEXPORT int HPCALL hpopers_init(void (*log_callback)(const char *format, va_list args)) {
    hpopers_log_set_callback(log_callback);

    // TODO: when (if) libhpopers is split from libhpcalcs, copy and adjust locale setting code from hpfiles.c.

    hpopers_info(_("hpopers library version %s compiled on %s"), hpopers_version_get(), __DATE__ " " __TIME__);
    hpopers_info(_("%s: init succeeded"), __FUNCTION__);

    return ERR_SUCCESS;
}

HPEXPORT int HPCALL hpopers_exit(void) {
    hpopers_info(_("%s: exit succeeded"), __FUNCTION__);
    return ERR_SUCCESS;
}


HPEXPORT const char* HPCALL hpopers_version_get (void) {
    return VERSION;
}

