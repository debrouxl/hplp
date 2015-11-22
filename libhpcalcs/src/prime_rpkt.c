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
 * \file prime_rpkt.c Calcs: Prime raw packets.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hpcalcs.h>
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "utils.h"

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

HPEXPORT int HPCALL prime_send(calc_handle * handle, prime_raw_hid_pkt * pkt) {
    int res;
    if (handle != NULL && pkt != NULL) {
        cable_handle * cable = handle->cable;
        if (cable != NULL) {
            hexdump("OUT", pkt->data, pkt->size, 2);
            res = hpcables_cable_send(cable, pkt->data, pkt->size);
            if (res == ERR_SUCCESS) {
                hpcalcs_info("%s: send succeeded", __FUNCTION__);
            }
            else {
                hpcalcs_error("%s: send failed", __FUNCTION__);
            }
        }
        else {
            res = ERR_CALC_NO_CABLE;
            hpcalcs_error("%s: cable is NULL", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL prime_recv(calc_handle * handle, prime_raw_hid_pkt * pkt) {
    int res;
    if (handle != NULL && pkt != NULL) {
        cable_handle * cable = handle->cable;
        if (cable != NULL) {
            uint8_t * data;
            data = (hpcalcs_alloc_funcs.malloc)(sizeof(pkt->data));
            if (data != NULL) {
                res = hpcables_cable_recv(cable, &data, &pkt->size);
                hexdump("IN", data, pkt->size, 2);
                if (res == ERR_SUCCESS) {
                    //hpcalcs_info("%s: recv succeeded", __FUNCTION__);
                    memcpy(pkt->data, data, sizeof(pkt->data));
                }
                else {
                    hpcalcs_warning("%s: recv failed", __FUNCTION__);
                }
                (hpcalcs_alloc_funcs.free)(data);
            }
            else {
                res = ERR_MALLOC;
                hpcalcs_error("%s: failed to allocate buffer", __FUNCTION__);
            }
        }
        else {
            res = ERR_CALC_NO_CABLE;
            hpcalcs_error("%s: cable is NULL", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}
