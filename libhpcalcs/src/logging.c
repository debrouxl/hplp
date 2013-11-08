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
 * \file logging.c Logging primitives
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hpfiles.h>
#include <hpcables.h>
#include <hpcalcs.h>
#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef  __GNUC__
#ifndef alloca
#define alloca(size) __builtin_alloca(size)
#endif
#endif

void (*hpfiles_log_callback)(const char *format, va_list args);
hplibs_logging_level hpfiles_log_level = LOG_LEVEL_ALL;

void (*hpcables_log_callback)(const char *format, va_list args);
hplibs_logging_level hpcables_log_level = LOG_LEVEL_ALL;

void (*hpcalcs_log_callback)(const char *format, va_list args);
hplibs_logging_level hpcalcs_log_level = LOG_LEVEL_ALL;


#define DEBUG_FUNC_BODY(lib, level) \
    if (lib##_log_callback != NULL && lib##_log_level <= LOG_LEVEL_##level) { \
        va_list args; \
        char * format2; \
        va_start (args, format); \
        format2 = (char *)alloca(strlen(format) + sizeof("hpfiles " #level ": ") + 10); \
        sprintf(format2, "hpfiles " #level ": %s\n", format); \
        (*lib##_log_callback)(format2, args); \
    }

HPEXPORT void HPCALL hpfiles_log_set_callback(void (*log_callback)(const char *format, va_list args)) {
    hpfiles_log_callback = log_callback;
}

HPEXPORT hplibs_logging_level HPCALL hpfiles_log_set_level(hplibs_logging_level log_level) {
    hplibs_logging_level ret = hpfiles_log_level;
    hpfiles_log_level = log_level;
    return ret;
}

void hpfiles_debug (const char *format, ...) {
    DEBUG_FUNC_BODY(hpfiles, DEBUG)
}

void hpfiles_info (const char *format, ...) {
    DEBUG_FUNC_BODY(hpfiles, INFO)
}

void hpfiles_warning (const char *format, ...) {
    DEBUG_FUNC_BODY(hpfiles, WARN)
}

void hpfiles_error (const char *format, ...) {
    DEBUG_FUNC_BODY(hpfiles, ERROR)
}



HPEXPORT void HPCALL hpcables_log_set_callback(void (*log_callback)(const char *format, va_list args)) {
    hpcables_log_callback = log_callback;
}

HPEXPORT hplibs_logging_level HPCALL hpcables_log_set_level(hplibs_logging_level log_level) {
    hplibs_logging_level ret = hpcables_log_level;
    hpcables_log_level = log_level;
    return ret;
}

void hpcables_debug (const char *format, ...) {
    DEBUG_FUNC_BODY(hpcables, DEBUG)
}

void hpcables_info (const char *format, ...) {
    DEBUG_FUNC_BODY(hpcables, INFO)
}

void hpcables_warning (const char *format, ...) {
    DEBUG_FUNC_BODY(hpcables, WARN)
}

void hpcables_error (const char *format, ...) {
    DEBUG_FUNC_BODY(hpcables, ERROR)
}



HPEXPORT void HPCALL hpcalcs_log_set_callback(void (*log_callback)(const char *format, va_list args)) {
    hpcalcs_log_callback = log_callback;
}

HPEXPORT hplibs_logging_level HPCALL hpcalcs_log_set_level(hplibs_logging_level log_level) {
    hplibs_logging_level ret = hpcalcs_log_level;
    hpcalcs_log_level = log_level;
    return ret;
}

void hpcalcs_debug (const char *format, ...) {
    DEBUG_FUNC_BODY(hpcalcs, DEBUG)
}

void hpcalcs_info (const char *format, ...) {
    DEBUG_FUNC_BODY(hpcalcs, INFO)
}

void hpcalcs_warning (const char *format, ...) {
    DEBUG_FUNC_BODY(hpcalcs, WARN)
}

void hpcalcs_error (const char *format, ...) {
    DEBUG_FUNC_BODY(hpcalcs, ERROR)
}
