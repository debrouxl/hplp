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

#include "logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void hpfiles_debug (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpfiles DEBUG: ") + 4);
    sprintf(format2, "hpfiles DEBUG: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}

void hpfiles_info (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpfiles INFO: ") + 4);
    sprintf(format2, "hpfiles INFO: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}

void hpfiles_warning (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpfiles WARN: ") + 4);
    sprintf(format2, "hpfiles WARN: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}

void hpfiles_error (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpfiles ERROR: ") + 4);
    sprintf(format2, "hpfiles ERROR: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}



void hpcables_debug (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables DEBUG: ") + 4);
    sprintf(format2, "hpcables DEBUG: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}

void hpcables_info (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables INFO: ") + 4);
    sprintf(format2, "hpcables INFO: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}

void hpcables_warning (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables WARN: ") + 4);
    sprintf(format2, "hpcables WARN: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}

void hpcables_error (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables ERROR: ") + 4);
    sprintf(format2, "hpcables ERROR: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}



void hpcalcs_debug (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs DEBUG: ") + 4);
    sprintf(format2, "hpcalcs DEBUG: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}

void hpcalcs_info (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs INFO: ") + 4);
    sprintf(format2, "hpcalcs INFO: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}

void hpcalcs_warning (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs WARN: ") + 4);
    sprintf(format2, "hpcalcs WARN: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}

void hpcalcs_error (const char *format, ...) {
    va_list args;
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs ERROR: ") + 4);
    sprintf(format2, "hpcalcs ERROR: %s\n", format);
    va_start (args, format);
    vprintf(format2, args);
}
