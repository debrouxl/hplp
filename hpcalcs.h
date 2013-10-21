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

#ifndef __HPCOMM_CALCS_H__
#define __HPCOMM_CALCS_H__

#include "hpcomm.h"

typedef struct _calc_fncts calc_fncts;
typedef struct _calc_handle calc_handle;

struct _calc_fncts {
    calc_model model;
    int (*get_infos) (calc_handle * handle);
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

int hpcalcs_init(void);
int hpcalcs_exit(void);

calc_handle * hpcalcs_handle_new(calc_model model);
int hpcalcs_handle_del(calc_handle * handle);

int hpcalcs_cable_attach(calc_handle * handle, cable_handle * cable);
int hpcalcs_cable_detach(calc_handle * handle);


int hpcalcs_calc_get_infos(calc_handle * handle);


typedef struct
{
    uint32_t size;
    uint8_t data[PRIME_RAW_DATA_SIZE];
} prime_raw_pkt;

int prime_send(calc_handle * handle, prime_raw_pkt * pkt);
int prime_recv(calc_handle * handle, prime_raw_pkt * pkt);


typedef struct
{
    uint32_t size;

    uint8_t * data;
} prime_vtl_pkt;

prime_vtl_pkt * prime_vtl_pkt_new(uint32_t size);
void prime_vtl_pkt_del(prime_vtl_pkt * pkt);

int prime_send_data(calc_handle * handle, prime_vtl_pkt * pkt);
int prime_recv_data(calc_handle * handle, prime_vtl_pkt * pkt);

#endif
