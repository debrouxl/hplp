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
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "prime_cmd.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------
// Calcs - HP Prime virtual packets
// -----------------------------------------------


HPEXPORT prime_vtl_pkt * HPCALL prime_vtl_pkt_new(uint32_t size) {
    prime_vtl_pkt * pkt = (prime_vtl_pkt *)(hpcalcs_alloc_funcs.malloc)(sizeof(*pkt));

    if (pkt != NULL) {
        pkt->size = size;
        if (size != 0) {
            pkt->data = (uint8_t *)(hpcalcs_alloc_funcs.calloc)(size, sizeof(*pkt->data));

            if (pkt->data == NULL) {
                (hpcalcs_alloc_funcs.free)(pkt);
                pkt = NULL;
            }
        }
    }

    return pkt;
}

HPEXPORT prime_vtl_pkt * HPCALL prime_vtl_pkt_new_with_data_ptr(uint32_t size, uint8_t * data) {
    prime_vtl_pkt * pkt = (prime_vtl_pkt *)(hpcalcs_alloc_funcs.malloc)(sizeof(*pkt));

    if (pkt != NULL) {
        pkt->size = size;
        pkt->data = data;
    }

    return pkt;
}

HPEXPORT void HPCALL prime_vtl_pkt_del(prime_vtl_pkt * pkt) {
    if (pkt != NULL) {
        (hpcalcs_alloc_funcs.free)(pkt->data);
        (hpcalcs_alloc_funcs.free)(pkt);
    }
    else {
        hpcalcs_error("%s: pkt is NULL", __FUNCTION__);
    }
}

HPEXPORT int HPCALL prime_send_new_protocol_init(calc_handle * handle) {
    int res;

    // This is a special package that appears to switch the
    // HP prime into some new protocol mode.
    // It looks like a SEND_KEY, but the packet ID must be 0xFF I think.
    prime_raw_hid_pkt raw;
    memset((void *)&raw, 0, sizeof(raw));
    raw.size = 8;
    raw.data[1] = 0xFF;
    raw.data[2] = 0xEC;
    // Remaining bytes are 0
    
    res = prime_send(handle, &raw);
    if (res) {
        hpcalcs_info("%s: send init failed", __FUNCTION__);
        return res;
    }
    else {
        hpcalcs_info("%s: send init succeeded", __FUNCTION__);
    }
    
    return res;
}

HPEXPORT int HPCALL prime_send_data_new(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res;
    if (handle != NULL && pkt != NULL) {
        prime_raw_hid_pkt raw;
        uint32_t i, q, r;
        uint32_t offset = 0;
        uint8_t pkt_id = 0x01;

        memset((void *)&raw, 0, sizeof(raw));
        q = (pkt->size) / (PRIME_RAW_HID_DATA_SIZE - 1);
        r = (pkt->size) % (PRIME_RAW_HID_DATA_SIZE - 1);

        hpcalcs_info("%s: q:%" PRIu32 "\tr:%" PRIu32, __FUNCTION__, q, r);

        for (i = 1; i <= q; i++) {
            raw.size = PRIME_RAW_HID_DATA_SIZE + 1;
            raw.data[1] = pkt_id;
            memcpy(raw.data + 2, pkt->data + offset, PRIME_RAW_HID_DATA_SIZE - 1);
            offset += PRIME_RAW_HID_DATA_SIZE - 1;

            res = prime_send(handle, &raw);
            if (res) {
                hpcalcs_info("%s: send %" PRIu32 " failed", __FUNCTION__, i);
                r = 0;
                break;
            }
            else {
                hpcalcs_info("%s: send %" PRIu32 " succeeded", __FUNCTION__, i);
            }

            // Increment packet ID, which seems to be necessary for computer -> calc packets
            pkt_id++;
            // Skip 0xFE to 0x01
            if (pkt_id == 0xFE) {
                pkt_id = 0x02;
            }
        }

        if (r || !pkt->size) {
            raw.size = r + 2;
            raw.data[1] = pkt_id;
            memcpy(raw.data + 2, pkt->data + offset, r);

            res = prime_send(handle, &raw);
            if (res) {
                hpcalcs_info("%s: send remaining failed", __FUNCTION__);
            }
            else {
                hpcalcs_info("%s: send remaining succeeded", __FUNCTION__);
            }
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL prime_send_data(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res;
    if (handle != NULL && pkt != NULL) {
        prime_raw_hid_pkt raw;
        uint32_t i, q, r;
        uint32_t offset = 0;
        uint8_t pkt_id = 0;

        memset((void *)&raw, 0, sizeof(raw));
        q = (pkt->size) / (PRIME_RAW_HID_DATA_SIZE - 1);
        r = (pkt->size) % (PRIME_RAW_HID_DATA_SIZE - 1);

        hpcalcs_info("%s: q:%" PRIu32 "\tr:%" PRIu32, __FUNCTION__, q, r);

        for (i = 1; i <= q; i++) {
            raw.size = PRIME_RAW_HID_DATA_SIZE + 1;
            raw.data[1] = pkt_id;
            memcpy(raw.data + 2, pkt->data + offset, PRIME_RAW_HID_DATA_SIZE - 1);
            offset += PRIME_RAW_HID_DATA_SIZE - 1;

            res = prime_send(handle, &raw);
            if (res) {
                hpcalcs_info("%s: send %" PRIu32 " failed", __FUNCTION__, i);
                r = 0;
                break;
            }
            else {
                hpcalcs_info("%s: send %" PRIu32 " succeeded", __FUNCTION__, i);
            }

            // Increment packet ID, which seems to be necessary for computer -> calc packets
            pkt_id++;
            if (pkt_id == 0xFF) {
                pkt_id = 0; // Skip 0xFF, which is used for other purposes.
            }
        }

        if (r || !pkt->size) {
            raw.size = r + 2;
            raw.data[1] = pkt_id;
            memcpy(raw.data + 2, pkt->data + offset, r);

            res = prime_send(handle, &raw);
            if (res) {
                hpcalcs_info("%s: send remaining failed", __FUNCTION__);
            }
            else {
                hpcalcs_info("%s: send remaining succeeded", __FUNCTION__);
            }
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL prime_recv_data(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res;
    if (handle != NULL && pkt != NULL) {
        prime_raw_hid_pkt raw;
        uint32_t expected_size = 0;
        uint32_t offset = 0;
        uint32_t read_pkts_count = 0;
        // WIP: reassembly.

        //size = pkt->size;
        pkt->size = 0;
        pkt->data = NULL;

        for(;;) {
            memset(&raw, 0, sizeof(raw));
            res = prime_recv(handle, &raw);
            if (res) {
                hpcalcs_warning("%s: recv failed", __FUNCTION__);
                break;
            }
            else {
                //hpcalcs_info("%s: recv succeeded", __FUNCTION__);
            }
            //hpcalcs_info("%s: raw.size=%" PRIu32, __FUNCTION__, raw.size);
            if (raw.size > 0) {
                uint8_t * new_data;

                uint8_t expected_seq = ((read_pkts_count + (read_pkts_count / 0xFF)) & 0xFF);
                // Exclude those packets from reassembly (at least for screenshotting purposes, they seem to be spurious).
                if (raw.data[0] == 0xFF) {
                    // TODO: investigate whether the second byte could indicate an error code ?
                    hpcalcs_error("%s: skipping packet starting with 0xFF", __FUNCTION__);
                    continue;
                }
                // Once we enable the new protocol, we get these periodically.
                else if (handle->protocol_version > 0 && raw.data[0] == 0xFE) {
                    // TODO: investigate whether the second byte could indicate an error code ?
                    hpcalcs_error("%s: skipping packet starting with 0xFE", __FUNCTION__);
                    continue;
                }
                // Sanity check. The first byte is the sequence number. After reaching 0xFE. it wraps back to 0 (skipping 0xFF).
                // TODO: This is probably going to be different if we're in the new_protocol mode.
                else if (raw.data[0] != expected_seq) {
                    res = ERR_CALC_PACKET_FORMAT;
                    hpcalcs_error("%s: packet out of sequence, got %d, expected %d", __FUNCTION__, (int)raw.data[0], (int)expected_seq);
                    break;
                }

                read_pkts_count++;

                // Over-read prevention (hopefully ^^) code: pre-set the expected size of the reply to the given command.
                if (read_pkts_count == 1) {
                    res = prime_data_size(pkt->cmd, raw.data + 1, &expected_size); // +1: skip leading byte.
                    if (res != ERR_SUCCESS) {
                        break;
                    }
                }

                pkt->size += raw.size - 1;
                new_data = (hpcalcs_alloc_funcs.realloc)(pkt->data, pkt->size);
                if (new_data != NULL) {
                    pkt->data = new_data;
                    // Skip first byte, which is usually 0x00.
                    memcpy(pkt->data + offset, &(raw.data[1]), raw.size - 1);
                    offset += raw.size - 1;
                }
                else {
                    res = ERR_MALLOC;
                    hpcalcs_error("%s: cannot reallocate memory", __FUNCTION__);
                    break;
                }
            }

            if (raw.size < PRIME_RAW_HID_DATA_SIZE) {
                hpcalcs_info("%s: breaking due to short packet (1)", __FUNCTION__);
                goto shorten_packet;
            }
            if (offset >= expected_size) {
                hpcalcs_info("%s: breaking because the expected size was reached (2)", __FUNCTION__);
shorten_packet:
                // Shorten packet.
                if (expected_size <= pkt->size) {
                    hpcalcs_info("%s: shortening packet from %" PRIu32 " to %" PRIu32, __FUNCTION__, pkt->size, expected_size);
                }
                else {
                    hpcalcs_warning("%s: expected %" PRIu32 " bytes but only got %" PRIu32 " bytes, output corrupted", __FUNCTION__, expected_size, pkt->size);
                }
                pkt->data = (hpcalcs_alloc_funcs.realloc)(pkt->data, expected_size);
                pkt->size = expected_size;
                break;
            }
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL prime_data_size(uint8_t cmd, uint8_t * data, uint32_t * out_size) {
    int res = ERR_SUCCESS;
    if (data != NULL && out_size != NULL) {
        switch (cmd) {
            case CMD_PRIME_CHECK_READY:
                // Single-packet reply.
                *out_size = 1;
                break;
            case CMD_PRIME_GET_INFOS:
            case CMD_PRIME_RECV_SCREEN:
            case CMD_PRIME_RECV_BACKUP:
            // Not supposed to receive REQ_FILE
            case CMD_PRIME_RECV_FILE:
            case CMD_PRIME_RECV_CHAT:
            // Not supposed to receive SEND_KEY
            // Not supposed to receive SET_DATE_TIME
                // Expected size is embedded in reply.
                if (data[1] == 0x01) {
                    if (cmd != data[0]) {
                        hpcalcs_warning("%s: command in packet %02X does not match the expected command %02X", __FUNCTION__, data[0], cmd);
                    }

                    *out_size = (((uint32_t)(data[2])) << 24) | (((uint32_t)(data[3])) << 16) | (((uint32_t)(data[4])) << 8) | ((uint32_t)(data[5]));
                    *out_size += 6; // cmd + 0x01 + size.
                }
                else {
                    res = ERR_CALC_PACKET_FORMAT;
                    hpcalcs_error("%s: expected 0x01 as second data byte", __FUNCTION__);
                }
                break;
            default:
                // Not implemented.
                *out_size = 0;
                hpcalcs_error("%s: received unknown command %u, size undetermined, please report", __FUNCTION__, cmd);
                break;
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}
