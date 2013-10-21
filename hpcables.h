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
 * \file hpcables.h Cables: base part.
 */

#ifndef __HPCOMM_CABLES_H__
#define __HPCOMM_CABLES_H__

#include "hpcomm.h"

typedef struct _cable_handle cable_handle;
typedef struct _cable_fncts cable_fncts;

struct _cable_fncts {
    cable_model model;
    const char * name;
    const char * description;
    int (*open) (cable_handle * handle);
    int (*close) (cable_handle * handle);
    int (*send) (cable_handle * handle, uint8_t * data, uint32_t len);
    int (*recv) (cable_handle * handle, uint8_t * data, uint32_t * len);
};

struct _cable_handle {
    cable_model model;
    void * handle;
    const cable_fncts * fncts;
    int read_timeout;
    int open;
    int busy;
};


int hpcables_init(void);
int hpcables_exit(void);

cable_handle * hpcables_handle_new(cable_model model);
int hpcables_handle_del(cable_handle * handle);

int hpcables_cable_open(cable_handle * handle);
int hpcables_cable_close(cable_handle * handle);
int hpcables_cable_send (cable_handle * handle, uint8_t * data, uint32_t len);
int hpcables_cable_recv (cable_handle * handle, uint8_t * data, uint32_t * len);

#endif
