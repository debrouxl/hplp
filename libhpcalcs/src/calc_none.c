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

static int calc_none_check_ready(calc_handle * handle, uint8_t ** out_data, uint32_t * out_size) {
    return 0;
}

static int calc_none_get_infos(calc_handle * handle, calc_infos * infos) {
    return 0;
}

static int calc_none_set_date_time(calc_handle * handle, time_t timestamp) {
    return 0;
}

static int calc_none_recv_screen(calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size) {
    return 0;
}

static int calc_none_send_file(calc_handle * handle, files_var_entry * file) {
    return 0;
}

static int calc_none_recv_file(calc_handle * handle, files_var_entry * request, files_var_entry ** out_file) {
    return 0;
}

static int calc_none_recv_backup(calc_handle * handle, files_var_entry *** out_vars) {
    return 0;
}

static int calc_none_send_key(calc_handle * handle, uint32_t code) {
    return 0;
}

static int calc_none_send_keys(calc_handle * handle, const uint8_t * data, uint32_t size) {
    return 0;
}

static int calc_none_send_chat(calc_handle * handle, const uint16_t * data, uint32_t size) {
    return 0;
}

static int calc_none_recv_chat(calc_handle * handle, uint16_t ** out_data, uint32_t * out_size) {
    return 0;
}

const calc_fncts calc_none_fncts =
{
    CALC_NONE,
    "Dummy calculator",
    "Dummy calculator used when no calculator is set",
    0,
    &calc_none_check_ready,
    &calc_none_get_infos,
    &calc_none_set_date_time,
    &calc_none_recv_screen,
    &calc_none_send_file,
    &calc_none_recv_file,
    &calc_none_recv_backup,
    &calc_none_send_key,
    &calc_none_send_keys,
    &calc_none_send_chat,
    &calc_none_recv_chat
};
