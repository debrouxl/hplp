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
 * \file typesprime.c Files: Prime file types and utility functions.
 */
 
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

// For getting the version of basename which doesn't modify its argument.
#ifndef _WIN32
#define _GNU_SOURCE
#endif

#include <hpfiles.h>
#include "typesprime.h"
#include "logging.h"
#include "error.h"

#include <inttypes.h>
#include <strings.h>
// For path splitting.
#ifndef _WIN32
#include <string.h>
#else
#include <stdlib.h>
#include <string.h>
#endif

const char * PRIME_CONST[][3] = {
    { "SETTINGS", "", "Settings" }, // The extension is embedded in the file name.
    { "", "unknown", "Unknown" },
    { "APP", "hpapp", "Application" },
    { "LIST", "hplist", "List" },
    { "MATRIX", "hpmatrix", "Matrix" },
    { "NOTE", "hpnote", "Note" },
    { "PRGM", "hpprgm", "Program" },
    { "", "unknown", "Unknown" }, // Maybe appnote ?
    { "", "unknown", "Unknown" }, // Maybe appprgm ?
    { "COMPLEX", "hpcomplex", "Complex" }, // Extension synthetized, not found with the connectivity kit or emulator ?
    { "REAL", "hpreal", "Real" }, // Extension synthetized, not found with the connectivity kit or emulator ?
    { "TESTMODECONFIG", "", "Real" }, // The extension is embedded in the file name.
    { NULL, NULL, NULL }
};

const char * prime_vartype2str(uint8_t type) {
    const char * res = "unknown";
    if (type < sizeof(PRIME_CONST)/sizeof(PRIME_CONST[0]) - 1) {
        res = PRIME_CONST[type][0];
    }
    hpfiles_debug("%s: returning %s", __FUNCTION__, res);
    return res;
}

uint8_t prime_str2vartype(const char * type) {
    uint8_t res = PRIME_TYPE_UNKNOWN;
    for (uint32_t i = 0; i < sizeof(PRIME_CONST)/sizeof(PRIME_CONST[0]) - 1; i++) {
        if (!strcasecmp(PRIME_CONST[i][0], type)) {
            res = i;
            break;
        }
    }
    hpfiles_debug("%s: returning %" PRIu8, __FUNCTION__, res);
    return res;
}

const char * prime_byte2fext(uint8_t type) {
    const char * res = "unknown";
    if (type < sizeof(PRIME_CONST)/sizeof(PRIME_CONST[0]) - 1) {
        res = PRIME_CONST[type][1];
    }
    hpfiles_debug("%s: returning %s", __FUNCTION__, res);
    return res;
}

uint8_t prime_fext2byte(const char * type) {
    uint8_t res = PRIME_TYPE_UNKNOWN;
    for (uint32_t i = 0; i < sizeof(PRIME_CONST)/sizeof(PRIME_CONST[0]) - 1; i++) {
        if (!strcasecmp(PRIME_CONST[i][1], type)) {
            res = i;
            break;
        }
    }
    hpfiles_debug("%s: returning %" PRIu8, __FUNCTION__, res);
    return res;
}

const char * prime_byte2desc(uint8_t type) {
    const char * res = "Unknown";
    if (type < sizeof(PRIME_CONST)/sizeof(PRIME_CONST[0]) - 1) {
        res = PRIME_CONST[type][2];
    }
    hpfiles_debug("%s: returning %s", __FUNCTION__, res);
    return res;
}

uint8_t prime_filename2byte(const char * filepath) {
    uint8_t res = PRIME_TYPE_UNKNOWN;
    prime_parsefilename(filepath, &res, NULL);
    hpfiles_debug("%s: returning %" PRIu8, __FUNCTION__, res);
    return res;
}

int prime_parsefilename(const char * filepath, uint8_t * out_type, char ** out_calcfilename) {
    int res = ERR_FILE_FILENAME;
    uint8_t type = PRIME_TYPE_UNKNOWN;
    // The way to get the basename and file extension is platform-dependent, obviously...
#ifndef _WIN32
    char * file = basename(filepath);
    char * extension = strrchr(file, '.');
#else
    char file[_MAX_FNAME + 1];
    char extension[_MAX_EXT + 1];
    _splitpath(filepath, NULL /* drive */, NULL /* dir */, file, extension);
#endif
    hpfiles_info("%s: file:\"%s\" extension:\"%s\"", __FUNCTION__, file, extension);
    if (   !strcmp(file, "calc.settings")
        || !strcmp(file, "settings")
        || !strcmp(file, "cas.settings")
       ) {
        type = PRIME_TYPE_SETTINGS;
    }
    else if (!strcmp(file, "testmodes.hptestmodes")) {
        type = PRIME_TYPE_TESTMODECONFIG;
    }
    else {
        if (extension != NULL && extension[0] == '.') {
            type = prime_fext2byte(extension + 1); // Skip '.'
        }
        else {
            hpfiles_error("%s: extension not found in filepath, unhandled type", __FUNCTION__);
        }
    }
    if (type != PRIME_TYPE_UNKNOWN) {
        res = ERR_SUCCESS;
        *out_type = type;
        if (out_calcfilename != NULL) {
            char * calcfilename = strdup(file);
            if (calcfilename != NULL) {
#ifndef _WIN32
                extension = strrchr(calcfilename, '.');
                if (extension != NULL) {
                    *extension = 0; // Strip extension.
                }
#endif
            }
            *out_calcfilename = calcfilename;
        }
    }
    hpfiles_debug("%s: returning %d", __FUNCTION__, res);
    return res;
}
