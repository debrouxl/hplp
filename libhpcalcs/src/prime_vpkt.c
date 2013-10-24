/* libhpcalcs - hand-helds support library
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
 * \file prime_vpkt.c Calcs: Prime virtual packets.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hpcalcs.h>
#include "logging.h"

#include <stdlib.h>
#include <string.h>

// -----------------------------------------------
// Calcs - HP Prime virtual packets
// -----------------------------------------------


HPEXPORT prime_vtl_pkt * HPCALL prime_vtl_pkt_new(uint32_t size) {
    prime_vtl_pkt * pkt = (prime_vtl_pkt *)malloc(sizeof(*pkt));

    pkt->size = size;
    pkt->data = (uint8_t *)calloc(size, sizeof(*pkt->data));

    if (pkt->data == NULL) {
        free(pkt);
        pkt = NULL;
    }

    return pkt;
}

HPEXPORT void HPCALL prime_vtl_pkt_del(prime_vtl_pkt * pkt) {
    if (pkt != NULL) {
        free(pkt->data);
        free(pkt);
    }
    else {
        hpcalcs_error("%s: pkt is NULL", __FUNCTION__);
    }
}

HPEXPORT int HPCALL prime_send_data(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        prime_raw_pkt raw;
        uint32_t i, q, r;
        uint32_t offset = 0;
        // WIP: fragmentation.

        memset((void *)&raw, 0, sizeof(raw));
        q = (pkt->size) / (PRIME_RAW_DATA_SIZE-1);
        r = (pkt->size) % (PRIME_RAW_DATA_SIZE-1);

        for (i = 1; i <= q; i++) {
            raw.size = PRIME_RAW_DATA_SIZE;
            memcpy(raw.data, pkt->data + offset, PRIME_RAW_DATA_SIZE);
            offset += PRIME_RAW_DATA_SIZE;
            /*raw.data[0] = pkt->cmd;
            memcpy(raw.data + 1, pkt->data + offset, PRIME_RAW_DATA_SIZE-1);
            offset += PRIME_RAW_DATA_SIZE-1;*/

            res = prime_send(handle, &raw);
            if (res) {
                r = 0;
                break;
            }
        }

        if (r || !pkt->size) {
            raw.size = r;
            memcpy(raw.data, pkt->data + offset, r);
            /*raw.size = r + 1;
            raw.data[0] = pkt->cmd;
            memcpy(raw.data + 1, pkt->data + offset, r);
            offset += r;*/

            res = prime_send(handle, &raw);
        }

        /*raw.size = (uint8_t)pkt->size;
        memcpy(&raw.data, pkt->data, PRIME_RAW_DATA_SIZE);
        res = prime_send(handle, &raw);*/
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL prime_recv_data(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        prime_raw_pkt raw;
        uint32_t size;
        uint32_t offset = 0;
        // WIP: reassembly.

        memset(&raw, 0, sizeof(raw));

        size = pkt->size;
        pkt->size = 0;
        pkt->data = malloc(PRIME_RAW_DATA_SIZE);

        if (pkt->data) {
            for(;;) {
                res = prime_recv(handle, &raw);
                if (res) {
                        break;
                }
                if (raw.size > 0) {
                    uint8_t * new_data;
                    /*pkt->cmd = raw.data[0];
                    pkt->size += raw.size-1;

                    pkt->data = g_realloc(pkt->data, pkt->size);
                    memcpy(pkt->data + offset, &(raw.data[1]), raw.size-1);
                    offset += raw.size-1;*/
                    pkt->size += raw.size;
                    new_data = realloc(pkt->data, pkt->size);
                    if (new_data != NULL) {
                        pkt->data = new_data;
                    }
                    else {
                        hpcalcs_error("%s: cannot reallocate memory", __FUNCTION__);
                        res = -1;
                        break;
                    }
                    offset += raw.size;
                }

                if (raw.size < PRIME_RAW_DATA_SIZE) {
                    break;
                }
                if (size && pkt->size == size) {
                    break;
                }
            }
        }
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}
