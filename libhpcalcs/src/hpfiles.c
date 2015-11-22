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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <inttypes.h>
#include <string.h>

#include <hpfiles.h>
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "utils.h"
#include "gettext.h"

hplibs_malloc_funcs hpfiles_alloc_funcs = {
    .malloc = malloc,
    .calloc = calloc,
    .realloc = realloc,
    .free = free
};


// not static, must be shared between instances
int hpfiles_instance_count = 0;

HPEXPORT int HPCALL hpfiles_init(hpfiles_config * config) {
    int res = ERR_SUCCESS;
    void (*log_callback)(const char *format, va_list args);
    hplibs_malloc_funcs * alloc_funcs;

    if (config == NULL) {
        log_callback = NULL;
        alloc_funcs = NULL;
    }
    else {
        if (config->version <= HPFILES_CONFIG_VERSION) {
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
        // Set up locale info, if NLS support is enabled.
#ifdef ENABLE_NLS
        {
            char locale_dir[65536];

#ifdef __WIN32__
            HANDLE hDll;
            int i;

            hDll = GetModuleHandle("libhpcalcs-0.dll");
            GetModuleFileName(hDll, locale_dir, 65515);

            for (i = (int)strlen(locale_dir); i >= 0; i--) {
                if (locale_dir[i] == '\\') {
                    break;
                }
            }
            locale_dir[i] = '\0';

            strcat(locale_dir, "\\locale");
#else
            strncpy(locale_dir, LOCALEDIR, sizeof(locale_dir) - 21);
#endif

            hpfiles_info("setlocale: %s", setlocale(LC_ALL, ""));
            hpfiles_info("bindtextdomain: %s", bindtextdomain(PACKAGE, locale_dir));
            bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
            hpfiles_info("textdomain: %s", textdomain(NULL));
        }
#endif

        if (!hpfiles_instance_count) {
            hpfiles_log_set_callback(log_callback);
            if (alloc_funcs != NULL) {
                hpfiles_alloc_funcs = *alloc_funcs;
            }
            hpfiles_info(_("hpfiles library version %s"), hpfiles_version_get());

            hpfiles_info(_("%s: init succeeded"), __FUNCTION__);
            hpfiles_instance_count++;
        }
        else {
            hpfiles_info(_("%s: re-init skipped"), __FUNCTION__);
            hpfiles_instance_count++;
        }
    }

    return res;
}

HPEXPORT int HPCALL hpfiles_exit(void) {
    int res;
    if (hpfiles_instance_count <= 0) {
        hpfiles_error(_("%s: more exits than inits"), __FUNCTION__);
        res = ERR_LIBRARY_EXIT;
    }
    else {
        hpfiles_instance_count--;

        hpfiles_info(_("%s: exit succeeded"), __FUNCTION__);
        res = ERR_SUCCESS;
    }
    return res;
}


HPEXPORT const char* HPCALL hpfiles_version_get (void) {
    return VERSION;
}


HPEXPORT files_var_entry * HPCALL hpfiles_ve_create(void) {
    return (hpfiles_alloc_funcs.calloc)(1, sizeof(files_var_entry));
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_size(uint32_t size) {
    files_var_entry * ve = hpfiles_ve_create();
    if (ve != NULL) {
        ve->data = (uint8_t *)(hpfiles_alloc_funcs.calloc)(sizeof(uint8_t), size);
        if (ve->data != NULL) {
            ve->size = size;
        }
        else {
            (hpfiles_alloc_funcs.free)(ve);
            ve = NULL;
        }
    }

    if (ve == NULL) {
        hpfiles_error("%s: failed to create ve", __FUNCTION__);
    }

    return ve;
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_data(uint8_t * data, uint32_t size) {
    files_var_entry * ve = hpfiles_ve_create();
    if (ve != NULL) {
        ve->data = (uint8_t *)(hpfiles_alloc_funcs.malloc)(size);
        if (ve->data != NULL) {
            if (data != NULL) {
                memcpy(ve->data, data, size);
            }
            ve->size = size;
        }
        else {
            (hpfiles_alloc_funcs.free)(ve);
            ve = NULL;
        }
    }

    if (ve == NULL) {
        hpfiles_error("%s: failed to create ve", __FUNCTION__);
    }

    return ve;
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_data_ptr(uint8_t * data, uint32_t size) {
    files_var_entry * ve = hpfiles_ve_create();
    if (ve != NULL) {
        ve->data = data;
        ve->size = size;
    }
    else {
        hpfiles_error("%s: failed to create ve", __FUNCTION__);
    }

    return ve;
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_data_and_name(uint8_t * data, uint32_t size, const char16_t * name) {
    files_var_entry * ve = hpfiles_ve_create_with_data(data, size);
    if (ve != NULL) {
        char16_strncpy(ve->name, name, FILES_VARNAME_MAXLEN);
    }

    if (ve == NULL) {
        hpfiles_error("%s: failed to create ve", __FUNCTION__);
    }

    return ve;
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_from_file(FILE * file, const char16_t * filename) {
    files_var_entry * ve = NULL;
    if (file != NULL) {
        if (!fseek(file, 0, SEEK_END)) {
            long size = ftell(file);
            if (size != -1) {
                if (!fseek(file, 0, SEEK_END)) {
                    // No calculator has 4 GB memory, let alone handle 4 GB variables, so let's (potentially) truncate long to uint32_t.
                    ve = hpfiles_ve_create_with_size((uint32_t)size);
                    if (ve != NULL) {
                        if (fread(ve->data, 1, (uint32_t)size, file) == (uint32_t)size) {
                            char16_strncpy(ve->name, filename, FILES_VARNAME_MAXLEN);
                        }
                        else {
                            hpfiles_error("%s: couldn't read from file", __FUNCTION__);
                        }
                    }
                    // else fall through
                }
                else {
                    hpfiles_error("%s: couldn't seek to BOF", __FUNCTION__);
                }
            }
            else {
                hpfiles_error("%s: couldn't obtain file size", __FUNCTION__);
            }
        }
        else {
            hpfiles_error("%s: couldn't seek to EOF", __FUNCTION__);
        }
    }
    else {
        hpfiles_error("%s: file is NULL", __FUNCTION__);
    }

    if (ve == NULL) {
        hpfiles_error("%s: failed to create ve", __FUNCTION__);
    }

    return ve;
}

HPEXPORT void HPCALL hpfiles_ve_delete(files_var_entry * ve) {
    if (ve != NULL) {
        (hpfiles_alloc_funcs.free)(ve->data);
        (hpfiles_alloc_funcs.free)(ve);
    }
    else {
        hpfiles_error("%s: ve is NULL", __FUNCTION__);
    }
}


HPEXPORT void *hpfiles_ve_alloc_data(uint32_t size) {
    return (hpfiles_alloc_funcs.calloc)(sizeof(uint8_t), size + 1);
}

HPEXPORT files_var_entry * HPCALL hpfiles_ve_copy(files_var_entry * dst, files_var_entry * src) {
    if (src != NULL && dst != NULL) {
        memcpy(dst, src, sizeof(files_var_entry));
        if (src->data != NULL) {
            dst->data = (uint8_t *)(hpfiles_alloc_funcs.malloc)(src->size);
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
        dst = (hpfiles_alloc_funcs.malloc)(sizeof(files_var_entry));
        if (dst != NULL) {
            memcpy(dst, src, sizeof(files_var_entry));
            if (src->data != NULL) {
                dst->data = (uint8_t *)(hpfiles_alloc_funcs.malloc)(src->size);
                if (dst->data != NULL) {
                    memcpy(dst->data, src->data, src->size);
                }
                else {
                    (hpfiles_alloc_funcs.free)(dst);
                    dst = NULL;
                }
            }
        }
    }
    else {
        hpfiles_error("%s: src is NULL", __FUNCTION__);
    }

    if (dst == NULL) {
        hpfiles_error("%s: failed to create ve", __FUNCTION__);
    }

    return dst;
}


HPEXPORT int hpfiles_ve_display(files_var_entry * ve) {
    int res;
    if (ve != NULL) {
        hpfiles_info("Displaying var entry %p", ve);
        hpfiles_info("Name: %ls", ve->name);
        hpfiles_info("Model: %u (%02X)", ve->model, ve->model);
        hpfiles_info("Type: %u (%02X)", ve->type, ve->type);
        hpfiles_info("Invalid: %u", ve->invalid);
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
    return (files_var_entry **)(hpfiles_alloc_funcs.calloc)(element_count + 1, sizeof(files_var_entry *));
}

HPEXPORT files_var_entry ** HPCALL hpfiles_ve_resize_array(files_var_entry ** array, uint32_t element_count) {
    return (files_var_entry **)(hpfiles_alloc_funcs.realloc)(array, (element_count + 1) * sizeof(files_var_entry *));
}

HPEXPORT void HPCALL hpfiles_ve_delete_array(files_var_entry ** array) {

    if (array != NULL) {
        files_var_entry ** ptr;
        for (ptr = array; *ptr; ptr++) {
            hpfiles_ve_delete(*ptr);
        }
        (hpfiles_alloc_funcs.free)(array);
    }
    else {
        hpfiles_error("%s: array is NULL", __FUNCTION__);
    }
}
