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
 * \file calc_prime.c Calcs: Prime top_level functions and structures.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hpcalcs.h>
#include "logging.h"

#include "prime_cmd.h"

static int calc_prime_check_ready(calc_handle * handle, uint8_t ** out_data, uint32_t * out_size) {
    int res;

    res = calc_prime_s_check_ready(handle);
    if (res == 0) {
        res = calc_prime_r_check_ready(handle, out_data, out_size);
        if (res != 0) {
            hpcalcs_error("%s: r_check_ready failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_check_ready failed", __FUNCTION__);
    }
    return res;
}

static int calc_prime_get_infos(calc_handle * handle, calc_infos * infos) {
    int res;

    res = calc_prime_s_get_infos(handle);
    if (res == 0) {
        res = calc_prime_r_get_infos(handle, infos);
        if (res != 0) {
            hpcalcs_error("%s: r_get_infos failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_get_infos failed", __FUNCTION__);
    }
    return res;
}

static int calc_prime_set_date_time(calc_handle * handle, time_t timestamp) {
    int res;

    res = calc_prime_s_set_date_time(handle, timestamp);
    if (res == 0) {
        res = calc_prime_r_set_date_time(handle);
        if (res != 0) {
            hpcalcs_error("%s: r_set_date_time failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_set_date_time failed", __FUNCTION__);
    }
    return res;
}

static int calc_prime_recv_screen(calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size) {
    int res;

    res = calc_prime_s_recv_screen(handle, format);
    if (res == 0) {
        res = calc_prime_r_recv_screen(handle, format, out_data, out_size);
        if (res != 0) {
            hpcalcs_error("%s: r_recv_screen failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_recv_screen failed", __FUNCTION__);
    }
    return res;
}

static int calc_prime_send_file(calc_handle * handle, files_var_entry * file) {
    int res;

    res = calc_prime_s_send_file(handle, file);
    if (res == 0) {
        res = calc_prime_r_send_file(handle);
        if (res != 0) {
            hpcalcs_error("%s: r_send_file failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_send_file failed", __FUNCTION__);
    }
    return res;
}

static int calc_prime_recv_file(calc_handle * handle, files_var_entry * request, files_var_entry ** out_file) {
    int res;

    res = calc_prime_s_recv_file(handle, request);
    if (res == 0) {
        res = calc_prime_r_recv_file(handle, out_file);
        if (res != 0) {
            hpcalcs_error("%s: r_recv_file failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_recv_file failed", __FUNCTION__);
    }
    return res;
}

static int calc_prime_recv_backup(calc_handle * handle, files_var_entry *** out_vars) {
    int res;

    res = calc_prime_s_recv_backup(handle);
    if (res == 0) {
        res = calc_prime_r_recv_backup(handle, out_vars);
        if (res != 0) {
            hpcalcs_error("%s: r_recv_backup failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_recv_backup failed", __FUNCTION__);
    }
    return res;
}

static int calc_prime_send_key(calc_handle * handle, uint32_t code) {
    int res;

    res = calc_prime_s_send_key(handle, code);
    if (res == 0) {
        res = calc_prime_r_send_key(handle);
        if (res != 0) {
            hpcalcs_error("%s: r_send_key failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_send_key failed", __FUNCTION__);
    }
    return res;
}

static int calc_prime_send_chat(calc_handle * handle, uint16_t * data, uint32_t size) {
    int res;

    res = calc_prime_s_send_chat(handle, data, size);
    if (res == 0) {
        res = calc_prime_r_send_chat(handle);
        if (res != 0) {
            hpcalcs_error("%s: r_send_chat failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_send_chat failed", __FUNCTION__);
    }
    return res;
}

const calc_fncts calc_prime_fncts =
{
    CALC_PRIME,
    "HP Prime",
    "HP Prime Graphing Calculator",
      CALC_OPS_CHECK_READY | CALC_OPS_GET_INFOS | CALC_OPS_SET_DATE_TIME | CALC_OPS_RECV_SCREEN
    | CALC_OPS_SEND_FILE | CALC_OPS_RECV_FILE | CALC_OPS_RECV_BACKUP | CALC_OPS_SEND_KEY
    | CALC_OPS_SEND_CHAT,
    &calc_prime_check_ready,
    &calc_prime_get_infos,
    &calc_prime_set_date_time,
    &calc_prime_recv_screen,
    &calc_prime_send_file,
    &calc_prime_recv_file,
    &calc_prime_recv_backup,
    &calc_prime_send_key,
    &calc_prime_send_chat,
};
