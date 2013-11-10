/* libhpcalcs - hand-helds support library
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
 * \file error.c Files / cables / calcs: Error functions.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef _WIN32
// For strdup
#define _BSD_SOURCE
#endif
#include <string.h>

#include <hpcalcs.h>
#include <hpcables.h>
#include <hpcalcs.h>

#include "logging.h"
#include "error.h"

HPEXPORT int HPCALL hpfiles_error_get (int number, char **message) {
    if (message != NULL) {
        if (number >= ERR_FILE_FIRST && number <= ERR_FILE_LAST) {
            *message = strdup("TODO human-readable error message");
            return 0;
        }
        else {
            *message = NULL;
            return number;
        }
    }
    else {
        hpfiles_error("%s: message is NULL", __FUNCTION__);
        return number;
    }
}

HPEXPORT int HPCALL hpcables_error_get (int number, char **message) {
    if (message != NULL) {
        if (number >= ERR_CABLE_FIRST && number <= ERR_CABLE_LAST) {
            *message = strdup("TODO human-readable error message");
            return 0;
        }
        else {
            *message = NULL;
            return number;
        }
    }
    else {
        hpcables_error("%s: message is NULL", __FUNCTION__);
        return number;
    }
}

HPEXPORT int HPCALL hpcalcs_error_get (int number, char **message) {
    if (message != NULL) {
        if (number >= ERR_CALC_FIRST && number <= ERR_CALC_LAST) {
            *message = strdup("TODO human-readable error message");
            return 0;
        }
        else {
            *message = NULL;
            return number;
        }
    }
    else {
        hpcalcs_error("%s: message is NULL", __FUNCTION__);
        return number;
    }
}


HPEXPORT int HPCALL hplibs_error_get(int number, char **message) {
    int err = number;
    char *s = NULL;

    if (message != NULL) {
        // Skip ERR_SUCCESS.
        if (number > ERR_CALC_FIRST && number <= ERR_CALC_LAST) {
            *message = strdup("TODO human-readable error message");
            return 0;
        }
        else {
            // Retrieve the error message.
            err = hpfiles_error_get(err, &s);
            if (err) {
                free(s);
                err = hpcables_error_get(err, &s);
                if (err) {
                    free(s);
                    err = hpcalcs_error_get(err, &s);
                    if (err) {
                        // next level: not a libhp* error.
                        free(s);
                    }
                    else {
                        hpcalcs_info("%s\n", s);
                    }
                }
                else {
                    hpcables_info("%s\n", s);
                }
            }
            else {
                hpfiles_info("%s\n", s);
            }
        }
    }
    else {
        hpcalcs_error("%s: message is NULL", __FUNCTION__);
        return number;
    }

    return number;
}
