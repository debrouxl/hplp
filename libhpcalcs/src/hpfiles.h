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
 * \file hpfiles.h Files: base part.
 */

#ifndef __HPLIBS_FILES_H__
#define __HPLIBS_FILES_H__

#include <stdint.h>
#include <stdlib.h>

#include "hplibs.h"
#include "export.h"
#include "filetypes.h"

// Not sure the target calculators support such long filenames...
#define FILES_VARNAME_MAXLEN (256)

typedef struct 
{
    char name[FILES_VARNAME_MAXLEN];

    uint8_t type;
    uint32_t size;
    uint8_t* data;
} files_var_entry;

#ifdef __cplusplus
extern "C" {
#endif

HPEXPORT int HPCALL hpfiles_init(void);
HPEXPORT int HPCALL hpfiles_exit(void);

HPEXPORT const char* HPCALL hpfiles_version_get(void);

HPEXPORT int HPCALL hpfiles_error_get(int number, char **message);


HPEXPORT files_var_entry * HPCALL hpfiles_ve_create(void);
HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_size(uint32_t size);
HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_data(uint8_t * data, uint32_t size);
HPEXPORT void HPCALL hpfiles_ve_delete(files_var_entry * entry);

HPEXPORT void * HPCALL hpfiles_ve_alloc_data(size_t size);
HPEXPORT files_var_entry * HPCALL hpfiles_ve_copy(files_var_entry * dst, files_var_entry * src);
HPEXPORT files_var_entry * HPCALL hpfiles_ve_dup(files_var_entry * src);

HPEXPORT files_var_entry ** HPCALL hpfiles_ve_create_array(int element_count);
HPEXPORT files_var_entry ** HPCALL hpfiles_ve_resize_array(files_var_entry ** entries, int element_count);
HPEXPORT void HPCALL hpfiles_ve_delete_array(files_var_entry ** entries);

#ifdef __cplusplus
}
#endif

#endif
