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
 * \file filetypes.c Files: File type utility functions.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hpfiles.h>
#include "filetypes.h"
#include "logging.h"

HPEXPORT const char * HPCALL hpfiles_vartype2str(calc_model model, uint8_t type) {
    switch (model) {
        case CALC_PRIME:
            return prime_vartype2str(type);
        default:
            hpfiles_error("%s: invalid model argument", __FUNCTION__);
            return "";
    }
}

HPEXPORT uint8_t HPCALL hpfiles_str2vartype(calc_model model, const char * type) {
    if (type == NULL) {
        hpfiles_error("%s: invalid type argument", __FUNCTION__);
        return FILE_TYPE_UNKNOWN;
    }
    switch (model) {
        case CALC_PRIME:
            return prime_str2vartype(type);
        default:
            hpfiles_error("%s: invalid model argument", __FUNCTION__);
            return FILE_TYPE_UNKNOWN;
    }
}

HPEXPORT const char * HPCALL hpfiles_vartype2fext(calc_model model, uint8_t type) {
    switch (model) {
        case CALC_PRIME:
            return prime_byte2fext(type);
        default:
            hpfiles_error("%s: invalid model argument", __FUNCTION__);
            return "";
    }
}

HPEXPORT uint8_t HPCALL hpfiles_fext2vartype(calc_model model, const char * type) {
    if (type == NULL) {
        hpfiles_error("%s: invalid type argument", __FUNCTION__);
        return FILE_TYPE_UNKNOWN;
    }
    switch (model) {
        case CALC_PRIME:
            return prime_fext2byte(type);
        default:
            hpfiles_error("%s: invalid model argument", __FUNCTION__);
            return FILE_TYPE_UNKNOWN;
    }
}

HPEXPORT uint8_t HPCALL hpfiles_filename2vartype(calc_model model, const char * filepath) {
    if (filepath == NULL) {
        hpfiles_error("%s: invalid filepath argument", __FUNCTION__);
        return FILE_TYPE_UNKNOWN;
    }
    switch (model) {
        case CALC_PRIME:
            return prime_filename2byte(filepath);
        default:
            hpfiles_error("%s: invalid model argument", __FUNCTION__);
            return FILE_TYPE_UNKNOWN;
    }
}

HPEXPORT int HPCALL hpfiles_parsefilename(calc_model model, const char * filepath, uint8_t * out_type, char ** out_calcfilename) {
    if (filepath == NULL || out_type == NULL || out_calcfilename == NULL) {
        hpfiles_error("%s: an argument is NULL", __FUNCTION__);
        return -1;
    }
    switch (model) {
        case CALC_PRIME:
            return prime_parsefilename(filepath, out_type, out_calcfilename);
        default:
            hpfiles_error("%s: invalid model argument", __FUNCTION__);
            return FILE_TYPE_UNKNOWN;
    }
}
