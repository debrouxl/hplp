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
 * \file logging.h Logging primitives.
 */

#ifndef __HPLIBS_LOGGING_H__
#define __HPLIBS_LOGGING_H__

void hpfiles_debug (const char *format, ...);
void hpfiles_info (const char *format, ...);
void hpfiles_warning (const char *format, ...);
void hpfiles_error (const char *format, ...);


void hpcables_debug (const char *format, ...);
void hpcables_info (const char *format, ...);
void hpcables_warning (const char *format, ...);
void hpcables_error (const char *format, ...);


void hpcalcs_debug (const char *format, ...);
void hpcalcs_info (const char *format, ...);
void hpcalcs_warning (const char *format, ...);
void hpcalcs_error (const char *format, ...);

#endif
