/*
 * libhpcalcs: hand-helds support libraries.
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
 * \file hpcalcs.c Calcs: base part.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <hpcables.h>
#include <hpcalcs.h>
#include "logging.h"
#include "error.h"
#include "gettext.h"

extern const calc_fncts calc_none_fncts;
extern const calc_fncts calc_prime_fncts;

const calc_fncts * hpcalcs_all_calcs[CALC_MAX] = {
    &calc_none_fncts,
    &calc_prime_fncts
};


// not static, must be shared between instances
int hpcalcs_instance_count = 0;

HPEXPORT int HPCALL hpcalcs_init(void (*log_callback)(const char *format, va_list args)) {
    hpcalcs_log_set_callback(log_callback);

    // TODO: when (if) libhpfiles is split from libhpcalcs, copy and adjust locale setting code from hpfiles.c.

    hpcalcs_info(_("hpcalcs library version %s compiled on %s"), hpcalcs_version_get(), __DATE__ " " __TIME__);
    hpcalcs_info(_("%s: init succeeded"), __FUNCTION__);

    return ERR_SUCCESS;
}

HPEXPORT int HPCALL hpcalcs_exit(void) {
    hpcalcs_info(_("%s: exit succeeded"), __FUNCTION__);
    return ERR_SUCCESS;
}


HPEXPORT const char* HPCALL hpcalcs_version_get (void) {
    return VERSION;
}


HPEXPORT calc_handle * HPCALL hpcalcs_handle_new(calc_model model) {
    calc_handle * handle = NULL;
    if (model < CALC_MAX) {
        handle = (calc_handle *)calloc(1, sizeof(*handle));

        if (handle != NULL) {
            handle->model = model;
            handle->fncts = hpcalcs_all_calcs[model];
            hpcalcs_info("%s: calc handle allocation succeeded", __FUNCTION__);
        }
        else {
            hpcalcs_error("%s: calc handle allocation failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: invalid model", __FUNCTION__);
    }
    return handle;
}

HPEXPORT int HPCALL hpcalcs_handle_del(calc_handle * handle) {
    int res;
    if (handle != NULL) {
        if (handle->attached) {
            res = hpcalcs_cable_detach(handle);
        }
        else {
            res = ERR_SUCCESS;
        }

        free(handle->handle);
        handle->handle = NULL;

        free(handle);
        hpcalcs_info("%s: calc handle deletion succeeded", __FUNCTION__);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_cable_attach(calc_handle * handle, cable_handle * cable) {
    int res;
    if (handle != NULL && cable != NULL) {
        res = hpcables_cable_open(cable);
        if (res == ERR_SUCCESS) {
            handle->cable = cable;
            handle->attached = 1;
            handle->open = 1;
            hpcalcs_info("%s: cable open and attach succeeded", __FUNCTION__);
        }
        else {
            hpcalcs_error("%s: cable open failed", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_cable_detach(calc_handle * handle) {
    int res;
    if (handle != NULL) {
        res = hpcables_cable_close(handle->cable);
        if (res == ERR_SUCCESS) {
            handle->open = 0;
            handle->attached = 0;
            handle->cable = NULL;
            hpcalcs_info("%s: cable close and detach succeeded", __FUNCTION__);
        }
        else {
            hpcalcs_error("%s: cable close and detach failed", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT cable_handle * HPCALL hpcalcs_cable_get(calc_handle * handle) {
    cable_handle * res = NULL;
    if (handle != NULL) {
        res = handle->cable;
        hpcalcs_info("%s: cable get succeeded", __FUNCTION__);
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_handle_display(calc_handle * handle) {
    int res;
    if (handle != NULL) {
        hpcalcs_info("Link calc handle details:");
        hpcalcs_info("\tmodel: %s", hpcalcs_model_to_string(handle->model));
        hpcalcs_info("\tattached: %d", handle->attached);
        hpcalcs_info("\topen: %d", handle->open);
        hpcalcs_info("\tbusy: %d", handle->busy);
        res = ERR_SUCCESS;
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }

    return res;
}

HPEXPORT calc_model HPCALL hpcalcs_get_model(calc_handle * handle) {
    calc_model model = CALC_NONE;
    if (handle != NULL) {
        model = handle->model;
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }

    return model;
}


#define DO_BASIC_HANDLE_CHECKS() \
    if (!handle->attached) { \
        res = ERR_CALC_NO_CABLE; \
        hpcalcs_error("%s: no cable attached", __FUNCTION__); \
        break; \
    } \
    if (!handle->open) { \
        res = ERR_CALC_CABLE_NOT_OPEN; \
        hpcalcs_error("%s: cable not open", __FUNCTION__); \
        break; \
    } \
    if (handle->busy) { \
        res = ERR_CALC_BUSY; \
        hpcalcs_error("%s: cable busy", __FUNCTION__); \
        break; \
    } \
    if (handle->fncts == NULL) { \
        res = ERR_CALC_INVALID_FNCTS; \
        hpcalcs_error("%s: fncts is NULL", __FUNCTION__); \
        break; \
    }

HPEXPORT int HPCALL hpcalcs_calc_check_ready(calc_handle * handle, uint8_t ** out_data, uint32_t * out_size) {
    int res;
    if (handle != NULL) {
        do {
            int (*check_ready) (calc_handle *, uint8_t **, uint32_t *);

            DO_BASIC_HANDLE_CHECKS()

            check_ready = handle->fncts->check_ready;
            if (check_ready != NULL) {
                handle->busy = 1;
                res = (*check_ready)(handle, out_data, out_size);
                if (res == ERR_SUCCESS) {
                    hpcalcs_info("%s: check_ready succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: check_ready failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->check_ready is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_get_infos(calc_handle * handle, calc_infos * infos) {
    int res;
    if (handle != NULL) {
        do {
            int (*get_infos) (calc_handle *, calc_infos *);

            DO_BASIC_HANDLE_CHECKS()

            get_infos = handle->fncts->get_infos;
            if (get_infos != NULL) {
                handle->busy = 1;
                res = (*get_infos)(handle, infos);
                if (res == 0) {
                    hpcalcs_info("%s: get_infos succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: get_infos failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->get_infos is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_set_date_time(calc_handle * handle, time_t timestamp) {
    int res;
    if (handle != NULL) {
        do {
            int (*set_date_time) (calc_handle *, time_t);

            DO_BASIC_HANDLE_CHECKS()

            set_date_time = handle->fncts->set_date_time;
            if (set_date_time != NULL) {
                handle->busy = 1;
                res = (*set_date_time)(handle, timestamp);
                if (res == 0) {
                    hpcalcs_info("%s: set_date_time succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: set_date_time failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->set_date_time is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_recv_screen(calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size) {
    int res;
    // TODO: some checking on format, but for now, it would hamper documentation efforts.
    if (handle != NULL) {
        do {
            int (*recv_screen) (calc_handle *, calc_screenshot_format, uint8_t **, uint32_t *);

            DO_BASIC_HANDLE_CHECKS()

            recv_screen = handle->fncts->recv_screen;
            if (recv_screen != NULL) {
                handle->busy = 1;
                res = (*recv_screen)(handle, format, out_data, out_size);
                if (res == 0) {
                    hpcalcs_info("%s: recv_screen succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: recv_screen failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->get_infos is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_send_file(calc_handle * handle, files_var_entry * file) {
    int res;
    if (handle != NULL) {
        do {
            int (*send_file) (calc_handle *, files_var_entry *);

            DO_BASIC_HANDLE_CHECKS()

            send_file = handle->fncts->send_file;
            if (send_file != NULL) {
                handle->busy = 1;
                res = (*send_file)(handle, file);
                if (res == 0) {
                    hpcalcs_info("%s: send_file succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: send_file failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->send_file is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_recv_file(calc_handle * handle, files_var_entry * name, files_var_entry ** out_file) {
    int res;
    if (handle != NULL) {
        do {
            int (*recv_file) (calc_handle *, files_var_entry *, files_var_entry **);

            DO_BASIC_HANDLE_CHECKS()

            recv_file = handle->fncts->recv_file;
            if (recv_file != NULL) {
                handle->busy = 1;
                res = (*recv_file)(handle, name, out_file);
                if (res == 0) {
                    hpcalcs_info("%s: recv_file succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: recv_file failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->recv_file is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_recv_backup(calc_handle * handle, files_var_entry *** out_vars) {
    int res;
    if (handle != NULL) {
        do {
            int (*recv_backup) (calc_handle *, files_var_entry ***);

            DO_BASIC_HANDLE_CHECKS()

            recv_backup = handle->fncts->recv_backup;
            if (recv_backup != NULL) {
                handle->busy = 1;
                res = (*recv_backup)(handle, out_vars);
                if (res == 0) {
                    hpcalcs_info("%s: recv_backup succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: recv_backup failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->recv_backup is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_send_key(calc_handle * handle, uint32_t code) {
    int res = -1;
    if (handle != NULL) {
        do {
            int (*send_key) (calc_handle *, uint32_t);

            DO_BASIC_HANDLE_CHECKS()

            send_key = handle->fncts->send_key;
            if (send_key != NULL) {
                handle->busy = 1;
                res = (*send_key)(handle, code);
                if (res == 0) {
                    hpcalcs_info("%s: send_key succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: send_key failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->send_key is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_send_keys(calc_handle * handle, const uint8_t * data, uint32_t size) {
    int res = -1;
    if (handle != NULL) {
        do {
            int (*send_keys) (calc_handle *, const uint8_t *, uint32_t);

            DO_BASIC_HANDLE_CHECKS()

            send_keys = handle->fncts->send_keys;
            if (send_keys != NULL) {
                handle->busy = 1;
                res = (*send_keys)(handle, data, size);
                if (res == 0) {
                    hpcalcs_info("%s: send_keys succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: send_keys failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->send_keys is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_send_chat(calc_handle * handle, const uint16_t * data, uint32_t size) {
    int res = -1;
    if (handle != NULL) {
        do {
            int (*send_chat) (calc_handle *, const uint16_t *, uint32_t);

            DO_BASIC_HANDLE_CHECKS()

            send_chat = handle->fncts->send_chat;
            if (send_chat != NULL) {
                handle->busy = 1;
                res = (*send_chat)(handle, data, size);
                if (res == 0) {
                    hpcalcs_info("%s: send_chat succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: send_chat failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->send_chat is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL hpcalcs_calc_recv_chat(calc_handle * handle, uint16_t ** data, uint32_t *size) {
    int res = -1;
    if (handle != NULL) {
        do {
            int (*recv_chat) (calc_handle *, uint16_t **, uint32_t *);

            DO_BASIC_HANDLE_CHECKS()

            recv_chat = handle->fncts->recv_chat;
            if (recv_chat != NULL) {
                handle->busy = 1;
                res = (*recv_chat)(handle, data, size);
                if (res == 0) {
                    hpcalcs_info("%s: recv_chat succeeded", __FUNCTION__);
                }
                else {
                    hpcalcs_error("%s: recv_chat failed", __FUNCTION__);
                }
                handle->busy = 0;
            }
            else {
                res = ERR_CALC_INVALID_FNCTS;
                hpcalcs_error("%s: fncts->recv_chat is NULL", __FUNCTION__);
            }
        } while (0);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

#undef DO_BASIC_HANDLE_CHECKS
