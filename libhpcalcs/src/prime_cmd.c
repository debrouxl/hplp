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
 * \file prime_cmd.c Calcs: Prime commands
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hpcalcs.h>
#include "prime_cmd.h"
#include "logging.h"

#include <stdlib.h>
#include <string.h>

/*static int send_vtl_pkt(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res = -1;
    if (handle != NULL) {
        *pkt = prime_vtl_pkt_new(PRIME_RAW_DATA_SIZE);
        if (*pkt != NULL) {
            res = prime_recv_data(handle, *pkt);
            if (res != 0) {
                goto end;
            }
end:
            prime_vtl_pkt_del(*pkt);
        }
        else {
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}*/

static int read_vtl_pkt(calc_handle * handle, prime_vtl_pkt ** pkt) {
    int res = -1;
    if (handle != NULL) {
        *pkt = prime_vtl_pkt_new(PRIME_RAW_DATA_SIZE);
        if (*pkt != NULL) {
            res = prime_recv_data(handle, *pkt);
            if (res != 0) {
                prime_vtl_pkt_del(*pkt);
                *pkt = NULL;
            }
        }
        else {
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_check_ready(calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(PRIME_RAW_DATA_SIZE);
        if (pkt != NULL) {
            uint8_t * ptr;

            ptr = pkt->data;
            *ptr++ = 0x00; // Report number ?
            *ptr++ = 0x00; // ?
            *ptr++ = CMD_PRIME_CHECK_READY;
            res = prime_send_data(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_check_ready(calc_handle * handle, uint8_t ** out_data, uint32_t * out_size) {
    prime_vtl_pkt * pkt;
    int res = read_vtl_pkt(handle, &pkt);
    if (res == 0 && pkt != NULL) {
        if (out_data != NULL && out_size != NULL) {
            *out_size = pkt->size;
            *out_data = malloc(pkt->size);
            if (*out_data != NULL) {
                memcpy(*out_data, pkt->data, pkt->size);
                res = 0;
            }
            else {
                hpcalcs_error("%s: couldn't allocate memory", __FUNCTION__);
                res = -1;
            }
        }
        // else do nothing. res is already 0.
        prime_vtl_pkt_del(pkt);
    }
    else {
        hpcalcs_error("%s: failed to read packet", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_get_infos (calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(PRIME_RAW_DATA_SIZE);
        if (pkt != NULL) {
            uint8_t * ptr;

            ptr = pkt->data;
            *ptr++ = 0x00; // Report number ?
            *ptr++ = 0x00; // ?
            *ptr++ = CMD_PRIME_GET_INFOS;
            res = prime_send_data(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_get_infos (calc_handle * handle, calc_infos * infos) {
    prime_vtl_pkt * pkt;
    int res = read_vtl_pkt(handle, &pkt);
    if (res == 0 && pkt != NULL) {
        if (infos != NULL) {
            infos->size = pkt->size;
            infos->data = malloc(pkt->size);
            if (infos->data != NULL) {
                memcpy(infos->data, pkt->data, pkt->size);
                res = 0;
            }
            else {
                hpcalcs_error("%s: couldn't allocate memory", __FUNCTION__);
                res = -1;
            }
        }
        // else do nothing. res is already 0.
        prime_vtl_pkt_del(pkt);
    }
    else {
        hpcalcs_error("%s: failed to read packet", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_recv_screen(calc_handle * handle, calc_screenshot_format format) {
    int res = -1;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(PRIME_RAW_DATA_SIZE);
        if (pkt != NULL) {
            uint8_t * ptr;

            ptr = pkt->data;
            *ptr++ = 0x00; // Report number ?
            *ptr++ = 0x00; // ?
            *ptr++ = CMD_PRIME_RECV_SCREEN;
            *ptr++ = (uint8_t)format;
            res = prime_send_data(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_recv_screen(calc_handle * handle, uint8_t ** out_data, uint32_t * out_size) {
    prime_vtl_pkt * pkt;
    int res = read_vtl_pkt(handle, &pkt);
    if (res == 0 && pkt != NULL) {
        if (out_data != NULL && out_size != NULL) {
            *out_size = pkt->size;
            *out_data = malloc(pkt->size);
            if (*out_data != NULL) {
                memcpy(*out_data, pkt->data, pkt->size);
                res = 0;
            }
            else {
                hpcalcs_error("%s: couldn't allocate memory", __FUNCTION__);
                res = -1;
            }
        }
        // else do nothing. res is already 0.
        prime_vtl_pkt_del(pkt);
    }
    else {
        hpcalcs_error("%s: failed to read packet", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_send_file(calc_handle * handle, files_var_entry * file) {
    int res = -1;
    return res;
}

HPEXPORT int HPCALL calc_prime_r_send_file(calc_handle * handle) {
    int res = -1;
    return res;
}

HPEXPORT int HPCALL calc_prime_s_recv_file(calc_handle * handle, const char * name) {
    int res = -1;
    return res;
}

HPEXPORT int HPCALL calc_prime_r_recv_file(calc_handle * handle, files_var_entry ** out_file) {
    int res = -1;
    return res;
}

HPEXPORT int HPCALL calc_prime_s_recv_backup(calc_handle * handle) {
    int res = -1;
    return res;
}

HPEXPORT int HPCALL calc_prime_r_recv_backup(calc_handle * handle, files_var_entry *** out_vars) {
    int res = -1;
    return res;
}
