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
#include "gettext.h"

HPEXPORT int HPCALL hpfiles_error_get(int number, char **message) {
    int ret = number;
    //hpfiles_debug("%s: entering", __FUNCTION__);
    if (message != NULL) {
        if (number >= ERR_FILE_FIRST && number <= ERR_FILE_LAST) {
            switch (number) {
                case ERR_FILE_FILENAME:
                    *message = strdup(_("Cannot understand filename"));
                    break;
                default:
                    *message = strdup(_("<Unknown error code>"));
                    break;
            }
            ret = 0;
        }
        else {
            *message = NULL;
        }
    }
    else {
        hpfiles_error("%s: message is NULL", __FUNCTION__);
    }
    //hpfiles_debug("%s: exiting %d", __FUNCTION__, ret);
    return ret;
}

HPEXPORT int HPCALL hpcables_error_get(int number, char **message) {
    int ret = number;
    //hpcables_debug("%s: entering", __FUNCTION__);
    if (message != NULL) {
        if (number >= ERR_CABLE_FIRST && number <= ERR_CABLE_LAST) {
            switch (number) {
                case ERR_CABLE_NOT_OPEN:
                    *message = strdup(_("Cable is not open"));
                    break;
                case ERR_CABLE_OPEN:
                    *message = strdup(_("Cable is already open"));
                    break;
                case ERR_CABLE_BUSY:
                    *message = strdup(_("Cable is busy"));
                    break;
                case ERR_CABLE_WRITE_ERROR:
                    *message = strdup(_("Error writing to cable"));
                    break;
                case ERR_CABLE_READ_ERROR:
                    *message = strdup(_("Error reading from cable"));
                    break;
                case ERR_CABLE_INVALID_FNCTS:
                    *message = strdup(_("Invalid cable functions"));
                    break;
                case ERR_CABLE_PROBE_FAILED:
                    *message = strdup(_("Cable probing failed"));
                    break;
                default:
                    *message = strdup(_("<Unknown error code>"));
                    break;
            }
            ret = 0;
        }
        else {
            *message = NULL;
        }
    }
    else {
        hpcables_error("%s: message is NULL", __FUNCTION__);
    }
    //hpcables_debug("%s: exiting %d", __FUNCTION__, ret);
    return ret;
}

HPEXPORT int HPCALL hpcalcs_error_get(int number, char **message) {
    int ret = number;
    //hpcalcs_debug("%s: entering", __FUNCTION__);
    if (message != NULL) {
        if (number >= ERR_CALC_FIRST && number <= ERR_CALC_LAST) {
            switch (number) {
                case ERR_CALC_NO_CABLE:
                    *message = strdup(_("No cable attached"));
                    break;
                case ERR_CALC_CABLE_NOT_OPEN:
                    *message = strdup(_("Cable is not open"));
                    break;
                case ERR_CALC_BUSY:
                    *message = strdup(_("Calc is busy"));
                    break;
                case ERR_CALC_INVALID_FNCTS:
                    *message = strdup(_("Invalid cable functions"));
                    break;
                case ERR_CALC_PACKET_FORMAT:
                    *message = strdup(_("Unhandled packet format"));
                    break;
                case ERR_CALC_SPLIT_TIMESTAMP:
                    *message = strdup(_("Unable to get time constituents"));
                    break;
                case ERR_CALC_PROBE_FAILED:
                    *message = strdup(_("Calc probing failed"));
                    break;
                default:
                    *message = strdup(_("<Unknown error code>"));
                    break;
            }
            ret = 0;
        }
        else {
            *message = NULL;
        }
    }
    else {
        hpcalcs_error("%s: message is NULL", __FUNCTION__);
    }
    //hpcalcs_debug("%s: exiting %d", __FUNCTION__, ret);
    return ret;
}

HPEXPORT int HPCALL hpopers_error_get(int number, char **message) {
    int ret = number;
    //hpopers_debug("%s: entering", __FUNCTION__);
    if (message != NULL) {
        if (number >= ERR_OPER_FIRST && number <= ERR_OPER_LAST) {
            switch (number) {
                default:
                    *message = strdup(_("<Unknown error code>"));
                    break;
            }
            ret = 0;
        }
        else {
            *message = NULL;
        }
    }
    else {
        hpopers_error("%s: message is NULL", __FUNCTION__);
    }
    //hpopers_debug("%s: exiting %d", __FUNCTION__, ret);
    return ret;
}

HPEXPORT int HPCALL hplibs_error_get(int number, char **message) {
    int err = number;
    char *s = NULL;

    if (message != NULL) {
        // Skip ERR_SUCCESS.
        if (number > ERR_HPLIBS_GENERIC_FIRST && number <= ERR_HPLIBS_GENERIC_LAST) {
            switch (number) {
                case ERR_MALLOC:
                    *message = strdup(_("Failed to allocate memory"));
                    break;
                case ERR_INVALID_HANDLE:
                    *message = strdup(_("Invalid handle pointer"));
                    break;
                case ERR_INVALID_PARAMETER:
                    *message = strdup(_("Invalid function parameter"));
                    break;
                case ERR_INVALID_MODEL:
                    *message = strdup(_("Invalid model"));
                    break;
                case ERR_LIBRARY_INIT:
                    *message = strdup(_("Problem initializing the library"));
                    break;
                default:
                    *message = strdup(_("<Unknown error code>"));
                    break;
            }
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
                        free(s);
                        err = hpopers_error_get(err, &s);
                        if (err) {
                            // next level: not a libhp* error.
                            free(s);
                            s = NULL;
                        }
                        else {
                            hpopers_info("%s\n", s);
                        }
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

            *message = s;
        }
    }
    else {
        hpcalcs_error("%s: message is NULL", __FUNCTION__);
        return number;
    }

    return number;
}
