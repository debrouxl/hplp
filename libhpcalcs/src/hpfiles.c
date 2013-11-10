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
 * \file hpfiles.c Files: base part.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hpfiles.h>
#include "logging.h"
#include "error.h"

#include <inttypes.h>
#include <string.h>

// not static, must be shared between instances
int hpfiles_instance_count = 0;

HPEXPORT int HPCALL hpfiles_init(void (*log_callback)(const char *format, va_list args)) {
    hpfiles_log_set_callback(log_callback);
    hpfiles_info("hpfiles library version %s compiled on " __DATE__ " " __TIME__, hpfiles_version_get());
    hpfiles_info("%s: init succeeded", __FUNCTION__);
    return ERR_SUCCESS;
}

HPEXPORT int HPCALL hpfiles_exit(void) {
    hpfiles_info("%s: exit succeeded", __FUNCTION__);
    return ERR_SUCCESS;
}


HPEXPORT const char* HPCALL hpfiles_version_get (void) {
    return VERSION;
}


HPEXPORT files_var_entry * HPCALL hpfiles_ve_create(void) {
    return calloc(1, sizeof(files_var_entry));
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_size(uint32_t size) {
    files_var_entry * ve = hpfiles_ve_create();
    if (ve != NULL) {
        ve->data = (uint8_t *)calloc(sizeof(uint8_t), size);
        if (ve->data != NULL) {
            ve->size = size;
        }
        else {
            free(ve);
            ve = NULL;
        }
    }

    return ve;
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_data(uint8_t * data, uint32_t size) {
    files_var_entry * ve = hpfiles_ve_create();
    if (ve != NULL) {
        ve->data = (uint8_t *)malloc(size);
        if (ve->data != NULL) {
            memcpy(ve->data, data, size);
            ve->size = size;
        }
        else {
            free(ve);
            ve = NULL;
        }
    }

    return ve;
}

HPEXPORT void HPCALL hpfiles_ve_delete(files_var_entry * ve) {
    if (ve != NULL) {
        free(ve->data);
        free(ve);
    }
    else {
        hpfiles_error("%s: ve is NULL", __FUNCTION__);
    }
}


HPEXPORT void *hpfiles_ve_alloc_data(uint32_t size) {
    return calloc(sizeof(uint8_t), size + 1);
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_copy(files_var_entry * dst, files_var_entry * src) {
    if (src != NULL && dst != NULL) {
        memcpy(dst, src, sizeof(files_var_entry));
        if (src->data != NULL) {
            dst->data = (uint8_t *)malloc(src->size);
            if (dst->data != NULL) {
                memcpy(dst->data, src->data, src->size);
            }
            else {
                dst = NULL;
            }
        }
    }
    else {
        hpfiles_error("%s: an argument is NULL", __FUNCTION__);
        dst = NULL;
    }

    return dst;
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_dup(files_var_entry * src) {
    files_var_entry * dst = NULL;

    if (src != NULL) {
        dst = malloc(sizeof(files_var_entry));
        if (dst != NULL) {
            memcpy(dst, src, sizeof(files_var_entry));
            if (src->data != NULL) {
                dst->data = (uint8_t *)malloc(src->size);
                if (dst->data != NULL) {
                    memcpy(dst->data, src->data, src->size);
                }
                else {
                    free(dst);
                    dst = NULL;
                }
            }
        }
    }
    else {
        hpfiles_error("%s: src is NULL", __FUNCTION__);
    }

    return dst;
}


HPEXPORT int hpfiles_ve_display(files_var_entry * ve) {
    int res;
    if (ve != NULL) {
        hpfiles_info("Displaying var entry %p", ve);
        hpfiles_info("Name: %ls", ve->name);
        hpfiles_info("Type: %u (%02X)", ve->type, ve->type);
        hpfiles_info("Size: %" PRIu32 " (%02" PRIX32 ")", ve->size, ve->size);
        hpfiles_info("Data: %p", ve->data);
        res = ERR_SUCCESS;
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpfiles_error("%s: ve is NULL", __FUNCTION__);
    }
    return res;
}


HPEXPORT files_var_entry ** HPCALL hpfiles_ve_create_array(uint32_t element_count) {
    return (files_var_entry **)calloc(element_count + 1, sizeof(files_var_entry *));
}

HPEXPORT files_var_entry ** HPCALL hpfiles_ve_resize_array(files_var_entry ** array, uint32_t element_count) {
    return (files_var_entry **)realloc(array, (element_count + 1) * sizeof(files_var_entry *));
}

HPEXPORT void HPCALL hpfiles_ve_delete_array(files_var_entry ** array) {
    files_var_entry ** ptr;

    if (array != NULL) {
        for(ptr = array; *ptr; ptr++) {
            hpfiles_ve_delete(*ptr);
        }
        free(array);
    }
    else {
        hpfiles_error("%s: array is NULL", __FUNCTION__);
    }
}
