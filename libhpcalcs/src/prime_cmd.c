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
#include "error.h"
#include "utils.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static inline uint16_t crc16_block(const uint8_t * buffer, uint32_t len) {
    static const uint16_t ccitt_crc16_table[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
    };
    uint16_t crc = 0;

    while (len--) {
       crc = ccitt_crc16_table[(crc >> 8) ^ *buffer++] ^ (crc << 8);
    }
    return crc;
}

static int read_vtl_pkt(calc_handle * handle, uint8_t cmd, prime_vtl_pkt ** pkt, int packet_contains_header) {
    int res;
    cable_handle * cable;
    (void)packet_contains_header;

    // Use different code paths for retrieving data, depending on the model.
    cable = hpcalcs_cable_get(handle);
    if (cable != NULL) {
        cable_model model = hpcables_get_model(cable);
        if (model == CABLE_PRIME_HID) {
            // Real Prime calculators need fragmentation.
            *pkt = prime_vtl_pkt_new(0);
            if (*pkt != NULL) {
                (*pkt)->cmd = cmd;
                res = prime_recv_data(handle, *pkt);
                // prime_recv_data takes care of setting (*pkt)->size.
                if (res == ERR_SUCCESS) {
                    if ((*pkt)->size > 0) {
                        if ((*pkt)->data[0] == (*pkt)->cmd) {
                            hpcalcs_debug("%s: command matches returned data", __FUNCTION__);
                        }
                        else {
                            hpcalcs_debug("%s: command does not match returned data", __FUNCTION__);
                            // It's not necessarily an error.
                        }
                    }
                    else {
                        hpcalcs_info("%s: empty packet", __FUNCTION__);
                    }
                }
                else {
                    prime_vtl_pkt_del(*pkt);
                    *pkt = NULL;
                }
            }
            else {
                res = ERR_MALLOC;
                hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
            }
        }
        else if (model == CABLE_PRIME_EMU) {
            // Virtual Prime calculators don't need fragmentation.
            uint32_t size = 10240;
            uint8_t * data = (uint8_t *)calloc(1, size);
            if (data != NULL) {
                // data might be reallocated by hpcables_cable_recv.
                res = hpcables_cable_recv(cable, &data, &size);
                if (res == ERR_SUCCESS) {
                    if (size > 0) {
                        uint32_t expected_size = 0;
                        hpcalcs_info("%s: non-empty packet", __FUNCTION__);
                        res = prime_data_size(cmd, data, &expected_size);
                        if (res != ERR_SUCCESS || size != expected_size) {
                            hpcalcs_error("%s: failed to determine packet size, or it does not match expectations", __FUNCTION__);
                        }
                        else {
                            size = expected_size;
                        }

                        *pkt = prime_vtl_pkt_new_with_data_ptr(size, data);
                        if (*pkt != NULL) {
                            hpcalcs_info("%s: created vpkt", __FUNCTION__);
                        }
                        else {
                            res = ERR_MALLOC;
                            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
                        }
                    }
                    else {
                        hpcalcs_info("%s: empty packet", __FUNCTION__);
                    }
                }
                else {
                    hpcalcs_error("%s: failed to read from cable", __FUNCTION__);
                }
            }
            else {
                res = ERR_MALLOC;
                hpcalcs_error("%s: couldn't create buffer", __FUNCTION__);
            }
        }
        else {
            res = ERR_INVALID_MODEL;
            hpcalcs_error("%s: unhandled model", __FUNCTION__);
        }
    }
    else {
        res = ERR_CALC_NO_CABLE;
        hpcalcs_error("%s: couldn't obtain cable", __FUNCTION__);
    }
    return res;
}

static int write_vtl_pkt(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res;
    // Use different code paths for submitting data, depending on the model.
    cable_handle * cable = hpcalcs_cable_get(handle);
    if (cable != NULL) {
        cable_model model = hpcables_get_model(cable);
        if (model == CABLE_PRIME_HID) {
            // Real Prime calculators need fragmentation.
            res = prime_send_data(handle, pkt);
        }
        else if (model == CABLE_PRIME_EMU) {
            // Virtual Prime calculators don't need fragmentation.
            res = hpcables_cable_send(cable, pkt->data, pkt->size);
        }
        else {
            res = ERR_INVALID_MODEL;
            hpcalcs_error("%s: unhandled model", __FUNCTION__);
        }
    }
    else {
        res = ERR_CALC_NO_CABLE;
        hpcalcs_error("%s: couldn't obtain cable", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_check_ready(calc_handle * handle) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(1);
        if (pkt != NULL) {
            uint8_t * ptr;

            pkt->cmd = CMD_PRIME_CHECK_READY;
            ptr = pkt->data;
            *ptr++ = CMD_PRIME_CHECK_READY;
            res = write_vtl_pkt(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_check_ready(calc_handle * handle, uint8_t ** out_data, uint32_t * out_size) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt;
        res = read_vtl_pkt(handle, CMD_PRIME_CHECK_READY, &pkt, 0);
        if (res == ERR_SUCCESS && pkt != NULL) {
            if (out_data != NULL && out_size != NULL) {
                *out_size = pkt->size;
                *out_data = pkt->data; // Transfer ownership of the memory block to the caller.
                pkt->data = NULL; // Detach it from virtual packet.
            }
            // else do nothing. res is already ERR_SUCCESS.
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: failed to read packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_get_infos (calc_handle * handle) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(1);
        if (pkt != NULL) {
            uint8_t * ptr;

            pkt->cmd = CMD_PRIME_GET_INFOS;
            ptr = pkt->data;
            *ptr++ = CMD_PRIME_GET_INFOS;
            res = write_vtl_pkt(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_get_infos (calc_handle * handle, calc_infos * infos) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt;
        res = read_vtl_pkt(handle, CMD_PRIME_GET_INFOS, &pkt, 1);
        if (res == ERR_SUCCESS && pkt != NULL) {
            if (infos != NULL) {
                infos->size = pkt->size;
                infos->data = pkt->data; // Transfer ownership of the memory block to the caller.
                pkt->data = NULL; // Detach it from virtual packet.
            }
            // else do nothing. res is already ERR_SUCCESS.
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: failed to read packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_set_date_time(calc_handle * handle, time_t timestamp) {
    int res;
    if (handle != NULL) {
        struct tm * brokendowntime = localtime(&timestamp);
        if (brokendowntime != NULL) {
            uint32_t size = 10; // Size of the data after the header.
            prime_vtl_pkt * pkt = prime_vtl_pkt_new(size + 6); // Add size of the header.
            if (pkt != NULL) {
                uint8_t * ptr;

                pkt->cmd = CMD_PRIME_SET_DATE_TIME;
                ptr = pkt->data;
                *ptr++ = CMD_PRIME_SET_DATE_TIME;
                *ptr++ = 0x01;
                *ptr++ = (uint8_t)((size >> 24) & 0xFF);
                *ptr++ = (uint8_t)((size >> 16) & 0xFF);
                *ptr++ = (uint8_t)((size >>  8) & 0xFF);
                *ptr++ = (uint8_t)((size      ) & 0xFF);
                *ptr++ = 0x00; // ?
                *ptr++ = 0x00; // ?
                *ptr++ = 0x54; // 84 decimal, ?
                *ptr++ = 0x1E; // 30 decimal, ?
                *ptr++ = brokendowntime->tm_year - (2000 - 1900); // tm_year: The number of years since 1900.
                *ptr++ = brokendowntime->tm_mon + 1; // tm_mon: the number of months since January, in the range 0 to 11.
                *ptr++ = brokendowntime->tm_mday;
                *ptr++ = brokendowntime->tm_hour;
                *ptr++ = brokendowntime->tm_min;
                *ptr++ = brokendowntime->tm_sec;
                res = write_vtl_pkt(handle, pkt);
                prime_vtl_pkt_del(pkt);
            }
            else {
                res = ERR_MALLOC;
                hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
            }
        }
        else {
            res = ERR_CALC_SPLIT_TIMESTAMP;
            hpcalcs_error("%s: couldn't split timestamp", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_set_date_time(calc_handle * handle) {
    int res = 0;
    if (handle != NULL) {
        // There doesn't seem to be anything to do.
        //res = calc_prime_r_check_ready(handle, NULL, NULL);
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_recv_screen(calc_handle * handle, calc_screenshot_format format) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(2);
        if (pkt != NULL) {
            uint8_t * ptr;

            pkt->cmd = CMD_PRIME_RECV_SCREEN;
            ptr = pkt->data;
            *ptr++ = CMD_PRIME_RECV_SCREEN;
            *ptr++ = (uint8_t)format;
            res = write_vtl_pkt(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_recv_screen(calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt;
        res = read_vtl_pkt(handle, CMD_PRIME_RECV_SCREEN, &pkt, 1);
        if (res == ERR_SUCCESS && pkt != NULL) {
            if (pkt->size > 13) {
                // Packet has CRC
                uint16_t computed_crc; // 0x0000 ?
                uint8_t * ptr = pkt->data;
                // For whatever reason the CRC seems to be encoded the other way around compared to receiving files
                uint16_t embedded_crc = (((uint16_t)(ptr[6])) << 8) | ((uint16_t)(ptr[7]));
                // Reset CRC before computing
                ptr[6] = 0x00;
                ptr[7] = 0x00;
                computed_crc = crc16_block(ptr + 6, pkt->size - 6); // The CRC for *screenshots* skips the header, and includes all data.
                hpcalcs_info("%s: embedded=%" PRIX16 " computed=%" PRIX16, __FUNCTION__, embedded_crc, computed_crc);
                if (computed_crc != embedded_crc) {
                    res = ERR_CALC_PACKET_FORMAT;
                    hpcalcs_error("%s: CRC mismatch", __FUNCTION__);
                }

                // Skip marker.
                if (pkt->data[8] == (uint8_t)format && pkt->data[9] == 0xFF && pkt->data[10] == 0xFF && pkt->data[11] == 0xFF && pkt->data[12] == 0xFF) {
                    if (out_data != NULL && out_size != NULL) {
                        *out_size = pkt->size - 13;
                        memmove(pkt->data, pkt->data + 13, pkt->size - 13);
                        *out_data = pkt->data; // Transfer ownership of the memory block to the caller.
                        pkt->data = NULL; // Detach it from virtual packet.
                    }
                    // else do nothing. res is already ERR_SUCCESS.
                }
                else {
                    res = ERR_CALC_PACKET_FORMAT;
                    hpcalcs_warning("%s: unknown marker at beginning of image", __FUNCTION__);
                }
            }
            else {
                res = ERR_CALC_PACKET_FORMAT;
                hpcalcs_info("%s: packet is too short: %" PRIu32 "bytes", __FUNCTION__, pkt->size);
            }
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: failed to read packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

// Seems to be made of a series of CMD_PRIME_RECV_FILE.
HPEXPORT int HPCALL calc_prime_s_send_file(calc_handle * handle, files_var_entry * file) {
    int res;
    if (handle != NULL && file != NULL) {
        uint8_t namelen = (uint8_t)char16_strlen(file->name) * 2;
        uint32_t size = 10 - 6 + namelen + file->size; // Size of the data after the header.
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(size + 6); // Add size of the header.
        hpcalcs_debug("Virtual packet has size %" PRIu32 " (%" PRIx32 ")\n", size, size);
        if (pkt != NULL) {
            uint8_t * ptr;
            uint16_t crc16;
            uint32_t offset = 0;

            // Mangle the file before sending, if necessary.
            switch (file->type) {
                case PRIME_TYPE_PRGM:
                    break;
                case PRIME_TYPE_NOTE:
                    if (file->data[0] == 0xFF && file->data[1] == 0xFE && (file->data[2] != 0x00 || file->data[3] != 0x00)) {
                        offset = 2;
                        size -= 2;
                    }
                    break;
                default:
                    break;
            }
                    // Some text editors add the UTF-16LE BOM at the beginning of the file, but the SDKV0.30 firmware version chokes on it.
                    // Therefore, skip the BOM, if it's followed by a nonzero character.
            if (   (file->type == PRIME_TYPE_PRGM || file->type == PRIME_TYPE_NOTE)
                && (file->data[0] == 0xFF && file->data[1] == 0xFE && (file->data[2] != 0x00 || file->data[3] != 0x00))
               ) {
                offset = 2;
                size -= 2;
            }
            else if (file->type == PRIME_TYPE_PRGM

            pkt->cmd = CMD_PRIME_RECV_FILE;
            ptr = pkt->data;
            *ptr++ = CMD_PRIME_RECV_FILE;
            *ptr++ = 0x01;
            *ptr++ = (uint8_t)((size >> 24) & 0xFF);
            *ptr++ = (uint8_t)((size >> 16) & 0xFF);
            *ptr++ = (uint8_t)((size >>  8) & 0xFF);
            *ptr++ = (uint8_t)((size      ) & 0xFF);
            *ptr++ = file->type;
            *ptr++ = namelen;
            *ptr++ = 0x00; // CRC16, set it to 0 for now.
            *ptr++ = 0x00;
            memcpy(ptr, file->name, namelen);
            ptr += namelen;
            memcpy(ptr, file->data + offset, file->size - offset);
            crc16 = crc16_block(pkt->data, size); // Yup, the last 6 bytes of the packet are excluded from the CRC.
            pkt->data[8] = crc16 & 0xFF;
            pkt->data[9] = (crc16 >> 8) & 0xFF;
            res = write_vtl_pkt(handle, pkt);

            prime_vtl_pkt_del(pkt);
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_send_file(calc_handle * handle) {
    int res;
    if (handle != NULL) {
        // There doesn't seem to be anything to do, beyond eliminating packets starting with 0xFF.
        res = calc_prime_r_check_ready(handle, NULL, NULL);
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

// Receiving an individual file from the calculator seems to start by a CMD_PRIME_REQ_FILE.
HPEXPORT int HPCALL calc_prime_s_recv_file(calc_handle * handle, files_var_entry * file) {
    int res;
    if (handle != NULL && file != NULL) {
        uint8_t namelen = (uint8_t)char16_strlen(file->name) * 2;
        uint32_t size = 10 - 6 + namelen; // Size of the data after the header.
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(size + 6); // Add size of the header.
        if (pkt != NULL) {
            uint8_t * ptr;
            uint16_t crc16;

            pkt->cmd = CMD_PRIME_RECV_FILE;
            ptr = pkt->data;
            *ptr++ = CMD_PRIME_REQ_FILE;
            *ptr++ = 0x01;
            *ptr++ = (uint8_t)((size >> 24) & 0xFF);
            *ptr++ = (uint8_t)((size >> 16) & 0xFF);
            *ptr++ = (uint8_t)((size >>  8) & 0xFF);
            *ptr++ = (uint8_t)((size      ) & 0xFF);
            *ptr++ = file->type;
            *ptr++ = namelen;
            *ptr++ = 0x00; // CRC16, set it to 0 for now.
            *ptr++ = 0x00;
            memcpy(ptr, file->name, namelen);
            crc16 = crc16_block(pkt->data, size); // Yup, the last 6 bytes are excluded from the CRC.
            pkt->data[8] = crc16 & 0xFF;
            pkt->data[9] = (crc16 >> 8) & 0xFF;
            res = write_vtl_pkt(handle, pkt);
            
            prime_vtl_pkt_del(pkt);
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

static int parse_recv_file(const uint8_t * pkt_data, uint32_t pkt_size, files_var_entry ** out_file) {
    int res;

    // Proper file packets have CRC
    uint16_t computed_crc; // 0x0000 ?
    uint16_t embedded_crc = (((uint16_t)(pkt_data[9])) << 8) | ((uint16_t)(pkt_data[8]));
    // Reset CRC before computing
    pkt_data[8] = 0x00;
    pkt_data[9] = 0x00;
    computed_crc = crc16_block(pkt_data, pkt_size - 6); // The CRC contains the initial 0x00, but not the final 6 bytes (...).
    hpcalcs_info("%s: embedded=%" PRIX16 " computed=%" PRIX16, __FUNCTION__, embedded_crc, computed_crc);
    if (computed_crc != embedded_crc) {
        hpcalcs_error("%s: CRC mismatch", __FUNCTION__);
    }

    if (out_file != NULL) {
        uint8_t namelen;
        uint32_t size;
        uint8_t filetype;

        *out_file = NULL;
        filetype = pkt_data[6];
        namelen = pkt_data[7];
        size = pkt_size - 10 - namelen;

        if (!(size & UINT32_C(0x80000000))) {
            *out_file = hpfiles_ve_create_with_data(&pkt_data[10 + namelen], size);
            if (*out_file != NULL) {
                (*out_file)->type = filetype;
                memcpy((*out_file)->name, &pkt_data[10], namelen);
                (*out_file)->invalid = (computed_crc != embedded_crc);
                hpcalcs_info("%s: created entry for %ls with size %" PRIu32 " and type %02X", __FUNCTION__, (*out_file)->name, (*out_file)->size, filetype);
            }
            else {
                res = ERR_MALLOC;
                hpcalcs_error("%s: couldn't create entry", __FUNCTION__);
            }
        }
        else {
            res = ERR_CALC_PACKET_FORMAT;
            hpcalcs_error("%s: weird size (packet too short ?)", __FUNCTION__);
            // TODO: change res.
        }
    }
}

HPEXPORT int HPCALL calc_prime_r_recv_file(calc_handle * handle, files_var_entry ** out_file) {
    int res;
    prime_vtl_pkt * pkt;
    // TODO: if no file was received, have *out_file = NULL, but res = 0.
    if (handle != NULL) {
        res = read_vtl_pkt(handle, CMD_PRIME_RECV_FILE, &pkt, 1);
        if (res == ERR_SUCCESS && pkt != NULL) {
            if (pkt->size >= 11) {
                res = parse_recv_file(pkt->data, pkt->size, out_file);
            }
            else {
                if (pkt->data[0] != 0xF9) {
                    res = ERR_CALC_PACKET_FORMAT;
                    hpcalcs_info("%s: packet is too short: %" PRIu32 "bytes", __FUNCTION__, pkt->size);
                }
                else {
                    hpcalcs_info("%s: skipping F9 packet", __FUNCTION__);
                }
                if (out_file != NULL) {
                    *out_file = NULL;
                }
            }
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: failed to read packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_recv_backup(calc_handle * handle) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(1);
        if (pkt != NULL) {
            uint8_t * ptr;

            pkt->cmd = CMD_PRIME_RECV_FILE;
            ptr = pkt->data;
            *ptr++ = CMD_PRIME_RECV_BACKUP;
            res = write_vtl_pkt(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_recv_backup(calc_handle * handle, files_var_entry *** out_vars) {
    int res;
    if (handle != NULL) {
        // In order to be more robust against packet losses, try to read as much as possible...
        res = read_vtl_pkt(handle, CMD_PRIME_RECV_FILE, &pkt, 1);
        // ... then attempt to split data according to file headers.
        if (res == ERR_SUCCESS && pkt != NULL) {
            if (pkt->size >= 11) {
                // Packet has CRC
                uint16_t computed_crc; // 0x0000 ?
                uint8_t * ptr = pkt->data;
                uint16_t embedded_crc = (((uint16_t)(ptr[9])) << 8) | ((uint16_t)(ptr[8]));
                // Reset CRC before computing
                ptr[8] = 0x00;
                ptr[9] = 0x00;
                computed_crc = crc16_block(ptr, pkt->size - 6); // The CRC contains the initial 0x00, but not the final 6 bytes (...).
                hpcalcs_info("%s: embedded=%" PRIX16 " computed=%" PRIX16, __FUNCTION__, embedded_crc, computed_crc);
                if (computed_crc != embedded_crc) {
                    hpcalcs_error("%s: CRC mismatch", __FUNCTION__);
                }

                if (out_file != NULL) {
                    uint8_t namelen;
                    uint32_t size;
                    uint8_t filetype;

                    *out_file = NULL;
                    filetype = pkt->data[6];
                    namelen = pkt->data[7];
                    size = pkt->size - 10 - namelen;

                    if (!(size & UINT32_C(0x80000000))) {
                        *out_file = hpfiles_ve_create_with_data(&pkt->data[10 + namelen], size);
                        if (*out_file != NULL) {
                            (*out_file)->type = filetype;
                            memcpy((*out_file)->name, &pkt->data[10], namelen);
                            (*out_file)->invalid = (computed_crc != embedded_crc);
                            hpcalcs_info("%s: created entry for %ls with size %" PRIu32 " and type %02X", __FUNCTION__, (*out_file)->name, (*out_file)->size, filetype);
                        }
                        else {
                            res = ERR_MALLOC;
                            hpcalcs_error("%s: couldn't create entry", __FUNCTION__);
                        }
                    }
                    else {
                        res = ERR_CALC_PACKET_FORMAT;
                        hpcalcs_error("%s: weird size (packet too short ?)", __FUNCTION__);
                        // TODO: change res.
                    }
                }
            }
            else {
                if (pkt->data[0] != 0xF9) {
                    res = ERR_CALC_PACKET_FORMAT;
                    hpcalcs_info("%s: packet is too short: %" PRIu32 "bytes", __FUNCTION__, pkt->size);
                }
                else {
                    hpcalcs_info("%s: skipping F9 packet", __FUNCTION__);
                }
                if (out_file != NULL) {
                    *out_file = NULL;
                }
            }
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: failed to read packet", __FUNCTION__);
        }
        
        uint32_t count = 0;
        files_var_entry ** entries = hpfiles_ve_create_array(count);
        if (entries != NULL) {
            for (;;) {
                if (out_vars != NULL) {
                    *out_vars = entries;
                }
                res = calc_prime_r_recv_file(handle, &entries[count]);
                if (res == ERR_SUCCESS) {
                    if (entries[count] != NULL) {
                        files_var_entry ** new_entries;
                        hpcalcs_info("%s: continuing due to non-empty entry", __FUNCTION__);
                        count++;
                        new_entries = hpfiles_ve_resize_array(entries, count);
                        if (new_entries != NULL) {
                            entries = new_entries;
                            entries[count] = NULL;
                        }
                        else {
                            res = ERR_MALLOC;
                            hpcalcs_error("%s: couldn't resize entries", __FUNCTION__);
                            break;
                        }
                    }
                    else {
                        hpcalcs_info("%s: breaking due to empty file", __FUNCTION__);
                        res = ERR_SUCCESS;
                        break;
                    }
                }
                else {
                    hpcalcs_error("%s: breaking due to reception failure", __FUNCTION__);
                    break;
                }
            }
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create entries", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_send_key(calc_handle * handle, uint32_t code) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(7);
        if (pkt != NULL) {
            uint8_t * ptr;

            pkt->cmd = CMD_PRIME_SEND_KEY;
            ptr = pkt->data;
            *ptr++ = CMD_PRIME_SEND_KEY;
            *ptr++ = 0x01;
            *ptr++ = 0x00;
            *ptr++ = 0x00;
            *ptr++ = 0x00;
            *ptr++ = 0x01;
            *ptr++ = (uint8_t)code;
            res = write_vtl_pkt(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_send_key(calc_handle * handle) {
    int res = 0;
    if (handle != NULL) {
        // There doesn't seem to be anything to do.
        //res = calc_prime_r_check_ready(handle, NULL, NULL);
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_send_keys(calc_handle * handle, const uint8_t * data, uint32_t size) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(6 + size);
        if (pkt != NULL) {
            uint8_t * ptr;

            pkt->cmd = CMD_PRIME_SEND_KEY;
            ptr = pkt->data;
            *ptr++ = CMD_PRIME_SEND_KEY;
            *ptr++ = 0x01;
            *ptr++ = (uint8_t)((size >> 24) & 0xFF);
            *ptr++ = (uint8_t)((size >> 16) & 0xFF);
            *ptr++ = (uint8_t)((size >>  8) & 0xFF);
            *ptr++ = (uint8_t)((size      ) & 0xFF);
            memcpy(ptr, data, size);
            res = write_vtl_pkt(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_send_keys(calc_handle * handle) {
    int res = 0;
    if (handle != NULL) {
        // There doesn't seem to be anything to do.
        //res = calc_prime_r_check_ready(handle, NULL, NULL);
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_s_send_chat(calc_handle * handle, const uint16_t * data, uint32_t size) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(size + 6);
        if (pkt != NULL) {
            uint8_t * ptr;

            pkt->cmd = CMD_PRIME_SEND_CHAT;
            ptr = pkt->data;
            *ptr++ = CMD_PRIME_SEND_CHAT;
            *ptr++ = 0x01;
            *ptr++ = (uint8_t)((size >> 24) & 0xFF);
            *ptr++ = (uint8_t)((size >> 16) & 0xFF);
            *ptr++ = (uint8_t)((size >>  8) & 0xFF);
            *ptr++ = (uint8_t)((size      ) & 0xFF);
            memcpy(ptr, data, size);
            res = write_vtl_pkt(handle, pkt);
            prime_vtl_pkt_del(pkt);
        }
        else {
            res = ERR_MALLOC;
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_send_chat(calc_handle * handle) {
    int res = 0;
    if (handle != NULL) {
        // There doesn't seem to be anything to do.
        //res = calc_prime_r_check_ready(handle, NULL, NULL);
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

HPEXPORT int HPCALL calc_prime_r_recv_chat(calc_handle * handle, uint16_t ** out_data, uint32_t * out_size) {
    int res;
    if (handle != NULL) {
        prime_vtl_pkt * pkt;
        res = read_vtl_pkt(handle, CMD_PRIME_RECV_CHAT, &pkt, 0);
        if (res == ERR_SUCCESS && pkt != NULL) {
            if (pkt->size >= 8) {
                if (out_data != NULL && out_size != NULL) {
                    *out_size = pkt->size - 6;
                    memmove(pkt->data, pkt->data + 6, pkt->size - 6);
                    *out_data = (uint16_t *)pkt->data; // Transfer ownership of the memory block to the caller.
                    pkt->data = NULL; // Detach it from virtual packet.
                }
            }
            else {
                res = ERR_CALC_PACKET_FORMAT;
                hpcalcs_info("%s: packet is too short: %" PRIu32 "bytes", __FUNCTION__, pkt->size);
            }
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: failed to read packet", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}
