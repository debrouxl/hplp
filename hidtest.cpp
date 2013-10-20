// Derived from the public domain hidtest code by Alan Ott, Signal 11 Software.

/*
 * libhpcables, libhpcalcs: hand-helds support libraries.
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

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "hidapi.h"


// -----------------------------------------------
// Helper functions
// -----------------------------------------------
static void do_print_buf(uint8_t * buf, uint32_t len) {
    // Print out the returned buffer.
    for (uint32_t i = 0; i < len; i++) {
        printf("%02hx ", buf[i]);
    }
    printf("\n");
}


// -----------------------------------------------
// Enums
// -----------------------------------------------
typedef enum {
    CABLE_NONE = 0,
    CABLE_PRIME_HID,
    CABLE_MAX
} cable_model;

typedef enum {
    CALC_NONE = 0,
    CALC_PRIME,
    CALC_MAX
} calc_model;

// -----------------------------------------------
// Cables - base part
// -----------------------------------------------
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
} ;

extern const cable_fncts cable_prime_hid_fncts;

const cable_fncts * hpcables_all_cables[CABLE_MAX] = {
    NULL,
    &cable_prime_hid_fncts
};

void hpcables_debug (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables DEBUG: ") + 4);
    sprintf(format2, "hpcables DEBUG: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcables_info (const char *format, ...)
{
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables INFO: ") + 4);
    sprintf(format2, "hpcables INFO: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcables_warning (const char *format, ...)
{
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables WARN: ") + 4);
    sprintf(format2, "hpcables WARN: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcables_error (const char *format, ...)
{
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables ERROR: ") + 4);
    sprintf(format2, "hpcables ERROR: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

int hpcables_init(void) {
    int res = hid_init();
    if (res == 0) {
        hpcables_info("%s: init succeeded", __FUNCTION__);
    }
    else {
        hpcables_error("%s: init failed", __FUNCTION__);
    }
    return res;
}

int hpcables_exit(void) {
    hid_exit();
    return 0;
}

cable_handle * hpcables_handle_new(cable_model model) {
    cable_handle * handle = NULL;
    if (model > CABLE_NONE && model < CABLE_MAX) {
        handle = (cable_handle *)calloc(1, sizeof(*handle));

        if (handle != NULL) {
            handle->model = model;
            handle->fncts = hpcables_all_cables[model];
            hpcables_info("%s: handle allocation succeeded", __FUNCTION__);
        }
        else {
            hpcables_error("%s: handle allocation failed", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: invalid model", __FUNCTION__);
    }

    return handle;
}

int hpcables_handle_del(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        free(handle->handle);
        handle->handle = NULL;

        free(handle);
        handle = NULL;
        res = 0;
        hpcables_info("%s: handle deletion succeeded", __FUNCTION__);
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }

    return res;
}

int hpcables_cable_open(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*open) (cable_handle *) = fncts->open;
            if (open != NULL) {
                res = (*open)(handle);
                if (res == 0) {
                    handle->open = 1;
                    hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: open failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->open is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcables_cable_close(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*close) (cable_handle *) = fncts->close;
            if (close != NULL) {
                res = (*close)(handle);
                if (res == 0) {
                    handle->open = 0;
                    hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: close failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->close is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcables_cable_send (cable_handle * handle, uint8_t * data, uint32_t len) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*send) (cable_handle *, uint8_t *, uint32_t) = fncts->send;
            if (send != NULL) {
                res = (*send)(handle, data, len);
                if (res == 0) {
                    hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: send failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->send is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcables_cable_recv (cable_handle * handle, uint8_t * data, uint32_t * len) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*recv) (cable_handle *, uint8_t *, uint32_t *) = fncts->recv;
            if (recv != NULL) {
                res = (*recv)(handle, data, len);
                if (res == 0) {
                    hpcables_info("%s: recv succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: recv failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->recv is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

// -----------------------------------------------
// Cables - HP Prime HID cable implementation.
// -----------------------------------------------

#define USB_VID_HP (0x03F0)
#define USB_PID_PRIME (0x0441)

#define PRIME_RAW_DATA_SIZE (64+1)

static int cable_prime_hid_open (cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        hid_device * device_handle = hid_open(USB_VID_HP, USB_PID_PRIME, NULL);
        if (device_handle) {
            handle->model = CABLE_PRIME_HID;
            handle->handle = (void *)device_handle;
            handle->fncts = &cable_prime_hid_fncts;
            handle->read_timeout = 500;
            handle->open = 1;
            handle->busy = 0;
            res = 0;
            hpcables_error("%s: cable open succeeded", __FUNCTION__);
        }
        else {
            hpcables_error("%s: cable open failed", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

static int cable_prime_hid_close (cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        hid_device * device_handle = (hid_device *)handle->handle;
        if (device_handle != NULL) {
            hid_close(device_handle);
            res = 0;
            hpcables_error("%s: cable close succeeded", __FUNCTION__);
        }
        else {
            hpcables_error("%s: device_handle is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

static int cable_prime_hid_send (cable_handle * handle, uint8_t * data, uint32_t len) {
    int res = -1;
    if (handle != NULL && data != NULL) {
        hid_device * device_handle = (hid_device *)handle->handle;
        if (device_handle != NULL) {
            res = hid_write(device_handle, data, len);
            if (res >= 0) {
                hpcables_info("%s: wrote %d bytes", __FUNCTION__, res);
                res = 0; // Indicate success.
            }
            else {
                hpcables_error("%s: write failed %ls", __FUNCTION__, hid_error(device_handle));
            }
        }
        else {
            hpcables_error("%s: device_handle is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

static int cable_prime_hid_recv (cable_handle * handle, uint8_t * data, uint32_t * len) {
    int res = -1;
    if (handle != NULL && data != NULL && len != NULL) {
        hid_device * device_handle = (hid_device *)handle->handle;
        if (device_handle != NULL) {
            res = hid_read_timeout(device_handle, data, PRIME_RAW_DATA_SIZE, handle->read_timeout);
            if (res >= 0) {
                hpcables_info("%s: read %d bytes", __FUNCTION__, res);
                *len = res;
                res = 0; // Indicate success
            }
            else {
                hpcables_error("%s: read failed", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: device_handle is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

const cable_fncts cable_prime_hid_fncts =
{
    CABLE_PRIME_HID,
    "Prime HID cable",
    "Prime HID cable",
    &cable_prime_hid_open,
    &cable_prime_hid_close,
    &cable_prime_hid_send,
    &cable_prime_hid_recv
};


// -----------------------------------------------
// Calcs - base part
// -----------------------------------------------
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

extern const calc_fncts calc_prime_fncts;

const calc_fncts * hpcalcs_all_calcs[CALC_MAX] = {
    NULL,
    &calc_prime_fncts
};

void hpcalcs_debug (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs DEBUG: ") + 4);
    sprintf(format2, "hpcalcs DEBUG: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcalcs_info (const char *format, ...)
{
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs INFO: ") + 4);
    sprintf(format2, "hpcalcs INFO: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcalcs_warning (const char *format, ...)
{
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs WARN: ") + 4);
    sprintf(format2, "hpcalcs WARN: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcalcs_error (const char *format, ...)
{
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs ERROR: ") + 4);
    sprintf(format2, "hpcalcs ERROR: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

int hpcalcs_init(void) {
    return 0;
}

int hpcalcs_exit(void) {
    return 0;
}

calc_handle * hpcalcs_handle_new(calc_model model) {
    calc_handle * handle = NULL;
    if (model > CALC_NONE && model < CALC_MAX) {
        handle = (calc_handle *)calloc(1, sizeof(*handle));

        if (handle != NULL) {
            handle->model = model;
            handle->fncts = hpcalcs_all_calcs[model];
            hpcalcs_info("%s: calc handle allocation succeeded", __FUNCTION__);
        }
        else {
            hpcables_error("%s: calc handle allocation failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: invalid model", __FUNCTION__);
    }
    return handle;
}

int hpcalcs_handle_del(calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        free(handle->handle);
        handle->handle = NULL;

        free(handle);
        handle = NULL;
        res = 0;
        hpcalcs_info("%s: calc handle deletion succeeded", __FUNCTION__);
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcalcs_cable_attach(calc_handle * handle, cable_handle * cable) {
    int res = -1;
    if (handle != NULL && cable != NULL) {
        handle->cable = cable;
        handle->attached = 1;

        if (hpcables_cable_open(cable) == 0) {
            handle->open = 1;
            res = 0;
            hpcalcs_info("%s: cable open succeeded", __FUNCTION__);
        }
        else {
            hpcalcs_error("%s: cable open failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

int hpcalcs_cable_detach(calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        if (hpcables_cable_close(handle->cable) == 0) {
            handle->open = 0;
            handle->attached = 0;
            handle->cable = NULL;
            res = 0;
            hpcalcs_info("%s: cable close succeeded", __FUNCTION__);
        }
        else {
            hpcalcs_error("%s: cable close failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcalcs_calc_get_infos(calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        const calc_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*get_infos) (calc_handle *) = fncts->get_infos;
            if (get_infos != NULL) {
                res = (*get_infos)(handle);
                if (res == 0) {
                    hpcables_info("%s: get_infos succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: get_infos failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->get_infos is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}


// -----------------------------------------------
// Calcs - HP Prime raw packets
// -----------------------------------------------
typedef struct
{
    uint32_t size;
    uint8_t data[PRIME_RAW_DATA_SIZE];
} prime_raw_pkt;

int prime_send(calc_handle * handle, prime_raw_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        cable_handle * cable = handle->cable;
        if (cable != NULL) {
            res = hpcables_cable_send(cable, pkt->data, pkt->size);
            if (res == 0) {
                hpcables_info("%s: send succeeded", __FUNCTION__);
            }
            else {
                hpcables_error("%s: send failed", __FUNCTION__);
            }
        }
        else {
            hpcalcs_error("%s: cable is NULL", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

int prime_recv(calc_handle * handle, prime_raw_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        cable_handle * cable = handle->cable;
        if (cable != NULL) {
            res = hpcables_cable_recv(cable, pkt->data, &pkt->size);
            if (res == 0) {
                hpcables_info("%s: send succeeded", __FUNCTION__);
            }
            else {
                hpcables_error("%s: send failed", __FUNCTION__);
            }
        }
        else {
            hpcalcs_error("%s: cable is NULL", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}


// -----------------------------------------------
// Calcs - HP Prime virtual packets
// -----------------------------------------------
typedef struct
{
    uint32_t size;

    uint8_t * data;
} prime_vtl_pkt;

prime_vtl_pkt * prime_vtl_pkt_new(uint32_t size) {
    prime_vtl_pkt * pkt = (prime_vtl_pkt *)malloc(sizeof(*pkt));

    pkt->size = size;
    pkt->data = (uint8_t *)calloc(size, sizeof(*pkt->data));

    if (pkt->data == NULL) {
        free(pkt);
        pkt = NULL;
    }

    return pkt;
}

void prime_vtl_pkt_del(prime_vtl_pkt * pkt) {
    if (pkt != NULL) {
        free(pkt->data);
        free(pkt);
    }
    else {
        hpcalcs_error("%s: pkt is NULL", __FUNCTION__);
    }
}

int prime_send_data(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        prime_raw_pkt raw;
        // TODO: fragmentation.
        raw.size = (uint8_t)pkt->size;
        memcpy(&raw.data, pkt->data, PRIME_RAW_DATA_SIZE);
        res = prime_send(handle, &raw);
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

int prime_recv_data(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        prime_raw_pkt raw;
        // TODO: reassembly.
        raw.size = PRIME_RAW_DATA_SIZE;
        for (;;) {
            res = prime_recv(handle, &raw);
            if (res == 0) {
                do_print_buf(raw.data, raw.size);
            }
            else {
                break;
            }
        }
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}


// -----------------------------------------------
// Calcs - HP Prime command protocols implementation
// -----------------------------------------------
int calc_prime_s_get_infos (calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(PRIME_RAW_DATA_SIZE);
        if (pkt != NULL) {
            uint8_t * ptr;

            // 1) 00 E7 01 00 00 00 0A 00 00 54 1E 0D 0A 13 (and the rest seems to vary), whatever that means
            ptr = pkt->data;
            *ptr++ = 0x00; // Report number ?
            *ptr++ = 0x00; // ?
            *ptr++ = 0xE7; // Command ?
            *ptr++ = 0x01; // 0x00000001, LE ?
            *ptr++ = 0x00;
            *ptr++ = 0x00;
            *ptr++ = 0x00;
            *ptr++ = 0x0A; // '\n' ?
            *ptr++ = 0x00;
            *ptr++ = 0x00;
            *ptr++ = 0x54; // 'T'
            *ptr++ = 0x1E;
            *ptr++ = 0x0D; // '\r'
            *ptr++ = 0x0A; // '\n'
            *ptr++ = 0x13;
            res = prime_send_data(handle, pkt);
            if (res != 0) {
                goto end;
            }

            // 2) 00 FF (and the rest seems to vary)
            memset(pkt->data, 0, PRIME_RAW_DATA_SIZE);
            ptr = pkt->data;
            *ptr++ = 0x00; // Report number ?
            *ptr++ = 0x00; // ?
            *ptr++ = 0xFF; // Command ?
            res = prime_send_data(handle, pkt);
            if (res != 0) {
                goto end;
            }

            // 3) 00 FA (and the rest seems to vary)
            memset(pkt->data, 0, PRIME_RAW_DATA_SIZE);
            ptr = pkt->data;
            *ptr++ = 0x00; // Report number ?
            *ptr++ = 0x00; // ?
            *ptr++ = 0xFA; // Command ?
            res = prime_send_data(handle, pkt);
            if (res != 0) {
                goto end;
            }

end:
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

int calc_prime_r_get_infos (calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(PRIME_RAW_DATA_SIZE);
        if (pkt != NULL) {
            res = prime_recv_data(handle, pkt);
            if (res != 0) {
                goto end;
            }
end:
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

// -----------------------------------------------
// Calcs - HP Prime operations implementation
// -----------------------------------------------
int calc_prime_get_infos (calc_handle * handle) {
    int res;

    res = calc_prime_s_get_infos(handle);
    if (res == 0) {
        res = calc_prime_r_get_infos(handle);
        if (res != 0) {
            hpcalcs_error("%s: r_get_infos failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_get_infos failed", __FUNCTION__);
    }
    return res;
}

const calc_fncts calc_prime_fncts =
{
    CALC_PRIME,
    &calc_prime_get_infos
};


/*static void do_read_all_packets(calc_handle * handle, unsigned char * buf) {
    printf("Data read:\n   ");
    int res;
    do {
        res = (handle, buf, 64);
        do_print_buf(buf, res);
    } while (res > 0);
}*/


// -----------------------------------------------
// It's a main !
// -----------------------------------------------
int main(int argc, char* argv[]) {
    int res;
//    unsigned char buf[512];
    cable_handle * cable;
    calc_handle * calc;

    (void)argc;
    (void)argv;

    printf("Hello World!\n");

    res = hpcables_init();
    if (res) {
        return 1;
    }
    res = hpcalcs_init();
    if (res) {
        return 1;
    }

#if 0
    {
        struct hid_device_info *devs, *cur_dev;

        devs = hid_enumerate(0x0, 0x0);
        cur_dev = devs;
        while (cur_dev) {
            printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
            printf("\n");
            printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
            printf("  Product:      %ls\n", cur_dev->product_string);
            printf("  Release:      %hx\n", cur_dev->release_number);
            printf("  Interface:    %d\n",  cur_dev->interface_number);
            printf("\n");
            cur_dev = cur_dev->next;
        }
        hid_free_enumeration(devs);
    }
#endif

    cable = hpcables_handle_new(CABLE_PRIME_HID);
    if (cable == NULL) {
        goto final_teardown;
    }
    calc = hpcalcs_handle_new(CALC_PRIME);
    if (calc == NULL) {
        goto del_cable;
    }
    if (hpcalcs_cable_attach(calc, cable) != 0) {
        goto del_calc;
    }

    if (hpcalcs_calc_get_infos(calc) != 0) {
        goto cleanup;
    }

// TODO: more packets.
/*
// Read result.

    do_read_all_packets(handle, buf);

// Next group

    // 4) 00 FC 09 (and the rest seems to vary)
    memset(buf, 0, sizeof(buf));
    ptr = buf;
    *ptr++ = 0x00; // Report number ?
    *ptr++ = 0x00; // ?
    *ptr++ = 0xFC; // Command ?
    *ptr++ = 0x09; // (might not be necessary)
    hpprime_raw_pkt_send(handle, buf, 65);

// Read result.

    do_read_all_packets(handle, buf);

// Next group

    // 6) 00 FC 05 (and the rest seems to vary)
    memset(buf, 0, sizeof(buf));
    ptr = buf;
    *ptr++ = 0x00; // Report number ?
    *ptr++ = 0x00; // ?
    *ptr++ = 0xFC; // Command ?
    *ptr++ = 0x05; // (might not be necessary)
    hpprime_raw_pkt_send(handle, buf, 65);

// Read result.

    do_read_all_packets(handle, buf);

// Next group

    // 6) 00 F9 02 (and the rest seems to vary)
    memset(buf, 0, sizeof(buf));
    ptr = buf;
    *ptr++ = 0x00; // Report number ?
    *ptr++ = 0x00; // ?
    *ptr++ = 0xF9; // Command ?
    *ptr++ = 0x02; // (might not be necessary)
    hpprime_raw_pkt_send(handle, buf, 65);

// Read result.

    do_read_all_packets(handle, buf);
*/

cleanup:

//detach_cable:
    hpcalcs_cable_detach(calc);
del_calc:
    hpcalcs_handle_del(calc);
del_cable:
    hpcables_handle_del(cable);
final_teardown:
    printf("Exiting program\n");
    hpcalcs_exit();
    hpcables_exit();

    printf("Goodbye world!\n");
    return 0;
}
