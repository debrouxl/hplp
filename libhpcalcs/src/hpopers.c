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
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "gettext.h"

hplibs_malloc_funcs hpopers_alloc_funcs = {
    .malloc = malloc,
    .calloc = calloc,
    .realloc = realloc,
    .free = free
};


// not static, must be shared between instances
int hpopers_instance_count = 0;

HPEXPORT int HPCALL hpopers_init(hpopers_config * config) {
    int res = ERR_SUCCESS;
    void (*log_callback)(const char *format, va_list args);
    hplibs_malloc_funcs * alloc_funcs;

    if (config == NULL) {
        log_callback = NULL;
        alloc_funcs = NULL;
    }
    else {
        if (config->version <= HPOPERS_CONFIG_VERSION) {
            if (config->version == 1) {
                log_callback = config->log_callback;
                alloc_funcs = config->alloc_funcs;
            }
            else {
                hpcables_error(_("%s: unsupported config version %u"), __FUNCTION__, config->version);
                res = ERR_LIBRARY_CONFIG_VERSION;
            }
        }
        else {
            hpcables_error(_("%s: unsupported config version %u"), __FUNCTION__, config->version);
            res = ERR_LIBRARY_CONFIG_VERSION;
        }
    }

    if (!res) {
        // TODO: when (if) libhpopers is split from libhpcalcs, copy and adjust locale setting code from hpfiles.c.

        if (!hpopers_instance_count) {
            hpopers_log_set_callback(log_callback);
            if (alloc_funcs != NULL) {
                hpopers_alloc_funcs = *alloc_funcs;
            }
            hpopers_info(_("hpopers library version %s"), hpopers_version_get());

            hpopers_info(_("%s: init succeeded"), __FUNCTION__);
            hpopers_instance_count++;
        }
        else {
            hpopers_info(_("%s: re-init skipped"), __FUNCTION__);
            hpopers_instance_count++;
        }
    }

    return res;
}

HPEXPORT int HPCALL hpopers_exit(void) {
    int res;

    if (hpopers_instance_count <= 0) {
        hpopers_error(_("%s: more exits than inits"), __FUNCTION__);
        res = ERR_LIBRARY_EXIT;
    }
    else {
        hpopers_instance_count--;

        hpopers_info(_("%s: exit succeeded"), __FUNCTION__);
        res = ERR_SUCCESS;
    }

    return res;
}


HPEXPORT const char* HPCALL hpopers_version_get (void) {
    return VERSION;
}

