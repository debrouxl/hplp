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
 * \file hpcalcs.h Calcs: base part.
 */

#ifndef __HPLIBS_CALCS_H__
#define __HPLIBS_CALCS_H__

#include <stdint.h>

#include "hplibs.h"
#include "hpfiles.h"
#include "hpcables.h"
#include "export.h"

typedef struct _calc_fncts calc_fncts;
typedef struct _calc_handle calc_handle;

typedef enum {
    CALC_FNCT_CHECK_READY = 0,
    CALC_FNCT_GET_INFOS = 1,
    CALC_FNCT_RECV_SCREEN = 2,
    CALC_FNCT_SEND_FILE = 3,
    CALC_FNCT_RECV_FILE = 4,
    CALC_FNCT_RECV_BACKUP = 5,
    CALC_FNCT_LAST // Keep this one last
} calc_fncts_idx;

typedef enum {
    CALC_OPS_NONE = 0,
    CALC_OPS_CHECK_READY = (1 << CALC_FNCT_CHECK_READY),
    CALC_OPS_GET_INFOS = (1 << CALC_FNCT_GET_INFOS),
    CALC_OPS_RECV_SCREEN = (1 << CALC_FNCT_RECV_SCREEN),
    CALC_OPS_SEND_FILE = (1 << CALC_FNCT_SEND_FILE),
    CALC_OPS_RECV_FILE = (1 << CALC_FNCT_RECV_FILE),
    CALC_OPS_RECV_BACKUP = (1 << CALC_FNCT_RECV_BACKUP)
} calc_features_operations;

// Probably incomplete
typedef enum {
    CALC_SCREENSHOT_FORMAT_PRIME_PNG_SOMETHING = 5,
    CALC_SCREENSHOT_FORMAT_PRIME_PNG = 9
} calc_screenshot_format;

// This will change when the returned data is better documented.
typedef struct {
    uint32_t size;
    uint8_t * data;
} calc_infos;

struct _calc_fncts {
    calc_model model;
    const char * name;
    const char * description;
    int features;
    int (*check_ready) (calc_handle * handle, uint8_t ** out_data, uint32_t * out_size);
    int (*get_infos) (calc_handle * handle, calc_infos * infos);
    int (*recv_screen) (calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size);
    int (*send_file) (calc_handle * handle, files_var_entry * file);
    int (*recv_file) (calc_handle * handle, const char * name, files_var_entry ** out_file);
    int (*recv_backup) (calc_handle * handle, files_var_entry *** out_vars);
};

struct _calc_handle {
    calc_model model;
    void * handle;
    const calc_fncts * fncts;
    cable_handle * cable;
    int attached;
    int open;
    int busy;
};


typedef struct
{
    uint32_t size;
    uint8_t data[PRIME_RAW_DATA_SIZE];
} prime_raw_pkt;


typedef struct
{
    uint32_t size;

    uint8_t * data;
} prime_vtl_pkt;


#ifdef __cplusplus
extern "C" {
#endif

HPEXPORT int HPCALL hpcalcs_init(void);
HPEXPORT int HPCALL hpcalcs_exit(void);

HPEXPORT const char* HPCALL hpcalcs_version_get(void);

HPEXPORT int HPCALL hpcalcs_error_get(int number, char **message);


HPEXPORT calc_handle * HPCALL hpcalcs_handle_new(calc_model model);
HPEXPORT int HPCALL hpcalcs_handle_del(calc_handle * handle);

HPEXPORT int HPCALL hpcalcs_cable_attach(calc_handle * handle, cable_handle * cable);
HPEXPORT int HPCALL hpcalcs_cable_detach(calc_handle * handle);


HPEXPORT int HPCALL hpcalcs_calc_check_ready(calc_handle * handle, uint8_t ** out_data, uint32_t * out_size);
HPEXPORT int HPCALL hpcalcs_calc_get_infos(calc_handle * handle, calc_infos * infos);
HPEXPORT int HPCALL hpcalcs_calc_recv_screen(calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size);
HPEXPORT int HPCALL hpcalcs_calc_send_file(calc_handle * handle, files_var_entry * file);
HPEXPORT int HPCALL hpcalcs_calc_recv_file(calc_handle * handle, const char * name, files_var_entry ** out_file);
HPEXPORT int HPCALL hpcalcs_calc_recv_backup(calc_handle * handle, files_var_entry *** out_vars);


HPEXPORT int HPCALL prime_send(calc_handle * handle, prime_raw_pkt * pkt);
HPEXPORT int HPCALL prime_recv(calc_handle * handle, prime_raw_pkt * pkt);


HPEXPORT prime_vtl_pkt * HPCALL prime_vtl_pkt_new(uint32_t size);
HPEXPORT void HPCALL prime_vtl_pkt_del(prime_vtl_pkt * pkt);

HPEXPORT int HPCALL prime_send_data(calc_handle * handle, prime_vtl_pkt * pkt);
HPEXPORT int HPCALL prime_recv_data(calc_handle * handle, prime_vtl_pkt * pkt);

#ifdef __cplusplus
}
#endif

#endif
