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
#include "logging.h"
#include "error.h"

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#define VPKT_DBG 2

static void hexdump(const char * direction, uint8_t *data, uint32_t size)
{
    if (size > 0) {
#if (VPKT_DBG == 1)
        char str[64];
        uint32_t i;

        hpcalcs_debug("Dumping %s packet with size %" PRIu32, direction, size);
        str[0] = 0;
        if (size <= 12)
        {
            str[0] = ' '; str[1] = ' '; str[2] = ' '; str[3] = ' ';

            for (i = 0; i < size; i++)
            {
                sprintf(&str[3*i+4], "%02X ", data[i]);
            }
        }
        else
        {
            sprintf(str, "    %02X %02X %02X %02X %02X ..... %02X %02X %02X %02X %02X",
                         data[0], data[1], data[2], data[3], data[4],
                         data[size-5], data[size-4], data[size-3], data[size-2], data[size-1]);
        }
        hpcalcs_debug(str);
#elif (VPKT_DBG == 2)
        char *str;

        hpcalcs_debug("Dumping %s packet with size %" PRIu32, direction, size);
        str = (char *)malloc(3 * size + 8 + 10);
        if (str != NULL) {
            uint32_t i, j, k;
            int step = 16;

            for(k = 0; k < 4; k++) {
                str[k] = ' ';
            }

            for (i = j = 0; i < size; i++, j++) {
                if (i && !(i % step)) {
                    hpcalcs_debug(str);
                    j = 0;
                }

                sprintf(&str[3*j+4], "%02X ", data[i]);
            }
            hpcalcs_debug(str);

            free(str);
        }
#endif
    }
}


HPEXPORT int HPCALL prime_send(calc_handle * handle, prime_raw_pkt * pkt) {
    int res;
    if (handle != NULL && pkt != NULL) {
        cable_handle * cable = handle->cable;
        if (cable != NULL) {
            hexdump("OUT", pkt->data, pkt->size);
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

HPEXPORT int HPCALL prime_recv(calc_handle * handle, prime_raw_pkt * pkt) {
    int res;
    if (handle != NULL && pkt != NULL) {
        cable_handle * cable = handle->cable;
        if (cable != NULL) {
            res = hpcables_cable_recv(cable, pkt->data, &pkt->size);
            hexdump("IN", pkt->data, pkt->size);
            if (res == ERR_SUCCESS) {
                //hpcalcs_info("%s: recv succeeded", __FUNCTION__);
            }
            else {
                hpcalcs_warning("%s: recv failed", __FUNCTION__);
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
