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

HPEXPORT int HPCALL hpcables_init(void) {
    int res;

    hpcables_info("hpcables library version %s", hpcables_version_get());

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

HPEXPORT int HPCALL hpcables_error_get (int number, char **message) {
    if (message != NULL) {
        *message = (char *)"";
        return number;
    }
    else {
        hpcables_error("%s: message is NULL", __FUNCTION__);
        return number;
    }
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


HPEXPORT int HPCALL hpcables_cable_open(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*open) (cable_handle *) = fncts->open;
            if (open != NULL) {
                res = (*open)(handle);
                if (res == 0) {
                    handle->open = 1;
                    hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: open failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->open is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_close(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*close) (cable_handle *) = fncts->close;
            if (close != NULL) {
                res = (*close)(handle);
                if (res == 0) {
                    handle->open = 0;
                    hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: close failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->close is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_send (cable_handle * handle, uint8_t * data, uint32_t len) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*send) (cable_handle *, uint8_t *, uint32_t) = fncts->send;
            if (send != NULL) {
                res = (*send)(handle, data, len);
                if (res == 0) {
                    hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: send failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->send is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcables_cable_recv (cable_handle * handle, uint8_t * data, uint32_t * len) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*recv) (cable_handle *, uint8_t *, uint32_t *) = fncts->recv;
            if (recv != NULL) {
                res = (*recv)(handle, data, len);
                if (res == 0) {
                    hpcables_info("%s: recv succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: recv failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->recv is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}
