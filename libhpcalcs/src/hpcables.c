/*
 * libhpcables: hand-helds support libraries.
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
 * \file hpcables.c Cables: base part.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>

#include <hidapi.h>

#include <hpcables.h>
#include "logging.h"
#include "error.h"
#include "gettext.h"


extern const cable_fncts cable_nul_fncts;
extern const cable_fncts cable_prime_hid_fncts;

const cable_fncts * hpcables_all_cables[CABLE_MAX] = {
    &cable_nul_fncts,
    &cable_prime_hid_fncts
};


// not static, must be shared between instances
int hpcables_instance_count = 0;

HPEXPORT int HPCALL hpcables_init(void (*log_callback)(const char *format, va_list args)) {
    int res;

    // TODO: when (if) libhpcables is split from libhpfiles, copy and adjust locale setting code from hpcalcs.c.

    hpcables_log_set_callback(log_callback);
    hpcables_info(_("hpcables library version %s compiled on %s"), hpcables_version_get(), __DATE__ " " __TIME__);

    res = hid_init();
    if (res == 0) {
        res = ERR_SUCCESS;
        hpcables_info(_("%s: init succeeded"), __FUNCTION__);
    }
    else {
        res = ERR_LIBRARY_INIT;
        hpcables_error(_("%s: init failed"), __FUNCTION__);
    }

    return res;
}

HPEXPORT int HPCALL hpcables_exit(void) {
    int res;
    hid_exit();
    res = ERR_SUCCESS;
    hpcables_info(_("%s: exit succeeded"), __FUNCTION__);
    return res;
}


HPEXPORT const char* HPCALL hpcables_version_get (void) {
    return VERSION;
}


HPEXPORT cable_handle * HPCALL hpcables_handle_new(cable_model model) {
    cable_handle * handle = NULL;
    if (model < CABLE_MAX) {
        handle = (cable_handle *)calloc(1, sizeof(*handle));

        if (handle != NULL) {
            handle->model = model;
            handle->handle = NULL;
            handle->fncts = hpcables_all_cables[model];
            hpcables_info("%s: handle allocation for model %d succeeded", __FUNCTION__, model);
        }
        else {
            hpcables_error("%s: handle allocation for model %d failed", __FUNCTION__, model );
        }
    }
    else {
        hpcables_error("%s: invalid model %d", __FUNCTION__, model);
    }

    return handle;
}

HPEXPORT int HPCALL hpcables_handle_del(cable_handle * handle) {
    int res;
    if (handle != NULL) {
        free(handle->handle);
        handle->handle = NULL;

        free(handle);
        res = ERR_SUCCESS;
        hpcables_info("%s: handle deletion succeeded", __FUNCTION__);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }

    return res;
}

HPEXPORT int HPCALL hpcables_handle_display(cable_handle * handle) {
    int res;
    if (handle != NULL) {
        hpcables_info("Link cable handle details:");
        hpcables_info("\tmodel: %s", hpcables_model_to_string(handle->model));
        hpcables_info("\tread_timeout: %d", handle->read_timeout);
        hpcables_info("\topen: %d", handle->open);
        hpcables_info("\tbusy: %d", handle->busy);
        res = ERR_SUCCESS;
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }

    return res;
}

HPEXPORT cable_model HPCALL hpcables_get_model(cable_handle * handle) {
    cable_model model = CABLE_NUL;
    if (handle != NULL) {
        model = handle->model;
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }

    return model;
}

#define DO_BASIC_HANDLE_CHECKS() \
    if (!handle->open) { \
        res = ERR_CABLE_NOT_OPEN; \
        hpcalcs_error("%s: cable not open", __FUNCTION__); \
        break; \
    } \
    DO_BASIC_HANDLE_CHECKS2()

#define DO_BASIC_HANDLE_CHECKS2() \
    if (handle->busy) { \
        res = ERR_CABLE_BUSY; \
        hpcalcs_error("%s: cable busy", __FUNCTION__); \
        break; \
    } \
    if (handle->fncts == NULL) { \
        res = ERR_CABLE_INVALID_FNCTS; \
        hpcalcs_error("%s: fncts is NULL", __FUNCTION__); \
        break; \
    }

HPEXPORT int HPCALL hpcables_options_get_read_timeout(cable_handle * handle) {
    int timeout = 0;
    if (handle != NULL) {
        timeout = handle->read_timeout;
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return timeout;
}

HPEXPORT int HPCALL hpcables_options_set_read_timeout(cable_handle * handle, int read_timeout) {
    int res;
    if (handle != NULL) {
        do {
            int (*set_read_timeout) (cable_handle *, int);

            DO_BASIC_HANDLE_CHECKS2()

            set_read_timeout = handle->fncts->set_read_timeout;
            if (set_read_timeout != NULL) {
                handle->busy = 1;
                res = (*set_read_timeout)(handle, read_timeout);
                if (res == ERR_SUCCESS) {
                    hpcables_info("%s: set_read_timeout succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: set_read_timeout failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CABLE_INVALID_FNCTS;
                hpcables_error("%s: fncts->set_read_timeout is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_probe(cable_handle * handle) {
    int res;
    if (handle != NULL) {
        do {
            int (*probe) (cable_handle *);

            DO_BASIC_HANDLE_CHECKS2()

            probe = handle->fncts->probe;
            if (probe != NULL) {
                handle->busy = 1;
                res = (*probe)(handle);
                if (res == ERR_SUCCESS) {
                    handle->open = 0;
                    hpcables_info("%s: probe succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: probe failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CABLE_INVALID_FNCTS;
                hpcables_error("%s: fncts->probe is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_open(cable_handle * handle) {
    int res;
    if (handle != NULL) {
        do {
            int (*open) (cable_handle *);

            if (handle->open) {
                res = ERR_CABLE_OPEN;
                hpcables_error("%s: cable already open", __FUNCTION__);
                break;
            }
            DO_BASIC_HANDLE_CHECKS2()

            open = handle->fncts->open;
            if (open != NULL) {
                handle->busy = 1;
                res = (*open)(handle);
                if (res == ERR_SUCCESS) {
                    handle->open = 1;
                    hpcables_info("%s: open succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: open failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CABLE_INVALID_FNCTS;
                hpcables_error("%s: fncts->open is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_close(cable_handle * handle) {
    int res;
    if (handle != NULL) {
        do {
            int (*close) (cable_handle *);

            DO_BASIC_HANDLE_CHECKS()

            close = handle->fncts->close;
            if (close != NULL) {
                handle->busy = 1;
                res = (*close)(handle);
                if (res == ERR_SUCCESS) {
                    handle->open = 0;
                    hpcables_info("%s: close succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: close failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CABLE_INVALID_FNCTS;
                hpcables_error("%s: fncts->close is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_send(cable_handle * handle, uint8_t * data, uint32_t len) {
    int res;
    if (handle != NULL) {
        do {
            int (*send) (cable_handle *, uint8_t *, uint32_t);

            DO_BASIC_HANDLE_CHECKS()

            send = handle->fncts->send;
            if (send != NULL) {
                handle->busy = 1;
                res = (*send)(handle, data, len);
                if (res == ERR_SUCCESS) {
                    //hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_warning("%s: send failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CABLE_INVALID_FNCTS;
                hpcables_error("%s: fncts->send is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_recv(cable_handle * handle, uint8_t ** data, uint32_t * len) {
    int res;
    if (handle != NULL) {
        do {
            int (*recv) (cable_handle *, uint8_t **, uint32_t *);

            DO_BASIC_HANDLE_CHECKS()

            recv = handle->fncts->recv;
            if (recv != NULL) {
                handle->busy = 1;
                res = (*recv)(handle, data, len);
                if (res == ERR_SUCCESS) {
                    //hpcables_info("%s: recv succeeded", __FUNCTION__);
                }
                else {
                    hpcables_warning("%s: recv failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CABLE_INVALID_FNCTS;
                hpcables_error("%s: fncts->recv is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

#undef DO_BASIC_HANDLE_CHECKS2
#undef DO_BASIC_HANDLE_CHECKS

HPEXPORT int HPCALL hpcables_probe_cables(uint8_t ** result) {
    int res = 0;
    if (result != NULL) {
        uint8_t * models = (uint8_t *)calloc(CABLE_MAX, sizeof(uint8_t));
        uint8_t * ptr = models;
        if (models != NULL) {
            for (cable_model model = CABLE_NUL; model < CABLE_MAX; model++) {
                cable_handle * handle = hpcables_handle_new(model);
                if (handle != NULL) {
                    if (hpcables_cable_probe(handle) == ERR_SUCCESS) {
                        res++;
                        *ptr++ = 1;
                        hpcables_info("%s: probing cable %s succeeded", __FUNCTION__, hpcables_model_to_string(model));
                    }
                    else {
                        ptr++;
                        hpcables_error("%s: probing cable %s failed", __FUNCTION__, hpcables_model_to_string(model));
                    }

                    if (hpcables_handle_del(handle) != ERR_SUCCESS) {
                        hpcables_error("%s: failed to destroy handle for model %s", __FUNCTION__, hpcables_model_to_string(model));
                    }
                    else {
                        hpcables_info("%s: destroy handle for model %s success", __FUNCTION__, hpcables_model_to_string(model));
                    }
                }
                else {
                    hpcables_error("%s: failed to create handle for model %s", __FUNCTION__, hpcables_model_to_string(model));
                }
            }
        }
        *result = models;
    }
    else {
        hpcables_error("%s: result is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_probe_free(uint8_t * models) {
    int res;
    if (models != NULL) {
        res = ERR_SUCCESS;
        free(models);
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcables_error("%s: models is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_probe_display(uint8_t * models) {
    int res;
    if (models != NULL) {
        cable_model model;
        res = 0;
        hpcables_info("Probing data details:");
        for (model = CABLE_NUL; model < CABLE_MAX; model++) {
            if (models[model] != 0) {
                hpcables_info("\tfound cable #%d: %s", model, hpcables_model_to_string(model));
            }
        }
        res = ERR_SUCCESS;
        hpcables_info("%s found %d cables", __FUNCTION__, res);
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcables_error("%s: models is NULL", __FUNCTION__);
    }
    return res;
}
