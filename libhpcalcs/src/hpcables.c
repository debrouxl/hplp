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

#include <hpcables.h>
#include "logging.h"

#include <stdlib.h>
#include <hidapi/hidapi.h>

extern const cable_fncts cable_prime_hid_fncts;

const cable_fncts * hpcables_all_cables[CABLE_MAX] = {
    NULL,
    &cable_prime_hid_fncts
};


// not static, must be shared between instances
int hpcables_instance_count = 0;

HPEXPORT int HPCALL hpcables_init(void (*log_callback)(const char *format, va_list args)) {
    int res;

    hpcables_log_set_callback(log_callback);
    hpcables_info("hpcables library version %s compiled on " __DATE__ " " __TIME__, hpcables_version_get());

    res = hid_init();
    if (res == 0) {
        hpcables_info("%s: init succeeded", __FUNCTION__);
    }
    else {
        hpcables_error("%s: init failed", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_exit(void) {
    hid_exit();
    hpcables_info("%s: exit succeeded", __FUNCTION__);
    return 0;
}


HPEXPORT const char* HPCALL hpcables_version_get (void) {
    return VERSION;
}


HPEXPORT cable_handle * HPCALL hpcables_handle_new(cable_model model) {
    cable_handle * handle = NULL;
    if (model > CABLE_NONE && model < CABLE_MAX) {
        handle = (cable_handle *)calloc(1, sizeof(*handle));

        if (handle != NULL) {
            handle->model = model;
            handle->fncts = hpcables_all_cables[model];
            hpcables_info("%s: handle allocation succeeded", __FUNCTION__);
        }
        else {
            hpcables_error("%s: handle allocation failed", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: invalid model", __FUNCTION__);
    }

    return handle;
}

HPEXPORT int HPCALL hpcables_handle_del(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        free(handle->handle);
        handle->handle = NULL;

        free(handle);
        handle = NULL;
        res = 0;
        hpcables_info("%s: handle deletion succeeded", __FUNCTION__);
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }

    return res;
}

HPEXPORT int HPCALL hpcables_handle_display(cable_handle * handle) {
    if (handle != NULL) {
        hpcables_info("Link cable handle details:");
        hpcables_info("\tmodel: %s", hpcables_model_to_string(handle->model));
        hpcables_info("\tread_timeout: %d", handle->read_timeout);
        hpcables_info("\topen: %d", handle->open);
        hpcables_info("\tbusy: %d", handle->busy);
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }

    return 0;
}

HPEXPORT cable_model HPCALL hpcables_get_model(cable_handle * handle) {
    cable_model model = CABLE_NONE;
    if (handle != NULL) {
        model = handle->model;
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }

    return model;
}

HPEXPORT int HPCALL hpcables_options_set_read_timeout(cable_handle * handle, int timeout) {
    int old_timeout = 0;
    if (handle != NULL) {
        do {
            if (!handle->open) {
                hpcables_error("%s: cable not open", __FUNCTION__);
                break;
            }
            if (handle->busy) {
                hpcables_error("%s: cable busy", __FUNCTION__);
                break;
            }

            handle->busy = 1;
            old_timeout = handle->read_timeout;
            handle->read_timeout = timeout;
            handle->busy = 0;
        } while (0);
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return old_timeout;
}

HPEXPORT int HPCALL hpcables_cable_open(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        do {
            const cable_fncts * fncts;
            if (handle->open) {
                hpcables_error("%s: cable open", __FUNCTION__);
                break;
            }
            if (handle->busy) {
                hpcables_error("%s: cable busy", __FUNCTION__);
                break;
            }

            fncts = handle->fncts;
            if (fncts != NULL) {
                int (*open) (cable_handle *) = fncts->open;
                if (open != NULL) {
                    handle->busy = 1;
                    res = (*open)(handle);
                    if (res == 0) {
                        handle->open = 1;
                        hpcables_info("%s: send succeeded", __FUNCTION__);
                    }
                    else {
                        hpcables_error("%s: open failed", __FUNCTION__);
                    }
                    handle->busy = 0;
                }
                else {
                    hpcables_error("%s: fncts->open is NULL", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_close(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        do {
            const cable_fncts * fncts;
            if (!handle->open) {
                hpcables_error("%s: cable not open", __FUNCTION__);
                break;
            }
            if (handle->busy) {
                hpcables_error("%s: cable busy", __FUNCTION__);
                break;
            }

            fncts = handle->fncts;
            if (fncts != NULL) {
                int (*close) (cable_handle *) = fncts->close;
                if (close != NULL) {
                    handle->busy = 1;
                    res = (*close)(handle);
                    if (res == 0) {
                        handle->open = 0;
                        hpcables_info("%s: send succeeded", __FUNCTION__);
                    }
                    else {
                        hpcables_error("%s: close failed", __FUNCTION__);
                    }
                    handle->busy = 0;
                }
                else {
                    hpcables_error("%s: fncts->close is NULL", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_send (cable_handle * handle, uint8_t * data, uint32_t len) {
    int res = -1;
    if (handle != NULL) {
        do {
            const cable_fncts * fncts;
            if (!handle->open) {
                hpcables_error("%s: cable not open", __FUNCTION__);
                break;
            }
            if (handle->busy) {
                hpcables_error("%s: cable busy", __FUNCTION__);
                break;
            }

            fncts = handle->fncts;
            if (fncts != NULL) {
                int (*send) (cable_handle *, uint8_t *, uint32_t) = fncts->send;
                if (send != NULL) {
                    handle->busy = 1;
                    res = (*send)(handle, data, len);
                    if (res == 0) {
                        //hpcables_info("%s: send succeeded", __FUNCTION__);
                    }
                    else {
                        hpcables_warning("%s: send failed", __FUNCTION__);
                    }
                    handle->busy = 0;
                }
                else {
                    hpcables_error("%s: fncts->send is NULL", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_recv (cable_handle * handle, uint8_t * data, uint32_t * len) {
    int res = -1;
    if (handle != NULL) {
        do {
            const cable_fncts * fncts;
            if (!handle->open) {
                hpcables_error("%s: cable not open", __FUNCTION__);
                break;
            }
            if (handle->busy) {
                hpcables_error("%s: cable busy", __FUNCTION__);
                break;
            }

            fncts = handle->fncts;
            if (fncts != NULL) {
                int (*recv) (cable_handle *, uint8_t *, uint32_t *) = fncts->recv;
                if (recv != NULL) {
                    handle->busy = 1;
                    res = (*recv)(handle, data, len);
                    if (res == 0) {
                        //hpcables_info("%s: recv succeeded", __FUNCTION__);
                    }
                    else {
                        hpcables_warning("%s: recv failed", __FUNCTION__);
                    }
                    handle->busy = 0;
                }
                else {
                    hpcables_error("%s: fncts->recv is NULL", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}
