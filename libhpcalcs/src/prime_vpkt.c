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
#include "error.h"

#include "prime_cmd.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------
// Calcs - HP Prime virtual packets
// -----------------------------------------------


HPEXPORT prime_vtl_pkt * HPCALL prime_vtl_pkt_new(uint32_t size) {
    prime_vtl_pkt * pkt = (prime_vtl_pkt *)malloc(sizeof(*pkt));

    pkt->size = size;
    if (size != 0) {
        pkt->data = (uint8_t *)calloc(size, sizeof(*pkt->data));

        if (pkt->data == NULL) {
            free(pkt);
            pkt = NULL;
        }
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
            // (but the calculator doesn't do the same).
            pkt_id++;
            if (pkt_id == 0xFF) {
                pkt_id = 0; // Skip 0xFF, which is used for other purposes.
            }
        }

        if (r || !pkt->size) {
            raw.size = r + 1;
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

        memset(&raw, 0, sizeof(raw));

        //size = pkt->size;
        pkt->size = 0;
        pkt->data = NULL;

        for(;;) {
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

                // Exclude those packets from reassembly (at least for screenshotting purposes, they seem to be spurious).
                if (raw.data[0] == 0xFF) {
                    // TODO: investigate whether the second byte could indicate an error code ?
                    hpcalcs_error("%s: skipping packet starting with 0xFF", __FUNCTION__);
                    continue;
                }
                // Sanity check. 0x00 is used for most packets, but dumps show 0x01 in some packets during the sequence for sending a file to the calculator.
                else if (raw.data[0] != 0x00 && raw.data[0] != 0x01) {
                    res = ERR_CALC_PACKET_FORMAT;
                    hpcalcs_error("%s: unknown packet starts neither with 0xFF, neither with 0x00, nor with 0x01", __FUNCTION__);
                    break;
                }

                read_pkts_count++;

                // Over-read prevention (hopefully ^^) code: pre-set the expected size of the reply to the given command.
                if (read_pkts_count == 1) {
                    hpcalcs_error("%s: %02X %02X %02X %02X", __FUNCTION__, raw.data[0], raw.data[1], raw.data[2], raw.data[3]);
                    switch (pkt->cmd) {
                        case CMD_PRIME_CHECK_READY:
                            // Single-packet reply.
                            expected_size = 1;
                            break;
                        case CMD_PRIME_GET_INFOS:
                        case CMD_PRIME_RECV_SCREEN:
                        case CMD_PRIME_RECV_FILE:
                        case CMD_PRIME_RECV_BACKUP:
                            // Expected size is embedded in reply.
                            if (raw.data[2] == 0x01) {
                                if (pkt->cmd != raw.data[1]) {
                                    hpcalcs_warning("%s: command in packet %02X does not match the expected command %02X", __FUNCTION__, raw.data[1], pkt->cmd);
                                }

                                expected_size = (((uint32_t)(raw.data[3])) << 24) | (((uint32_t)(raw.data[4])) << 16) | (((uint32_t)(raw.data[5])) << 8) | ((uint32_t)(raw.data[6]));
                                expected_size += 6; // cmd + 0x01 + size.
                            }
                            else {
                                // Most of the packet returned 
                                if (pkt->cmd != CMD_PRIME_CHECK_READY) {
                                    res = ERR_CALC_PACKET_FORMAT;
                                    hpcalcs_error("%s: expected 0x01 as third byte in the packet", __FUNCTION__);
                                }
                            }
                            break; 
                        default:
                            // Not implemented.
                            expected_size = 0;
                            break;
                    }
                    if (res != 0) {
                        break;
                    }
                }

                pkt->size += raw.size - 1;
                new_data = realloc(pkt->data, pkt->size);
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
                pkt->data = realloc(pkt->data, expected_size);
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
