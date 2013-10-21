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

#include "logging.h"
#include "hpcables.h"
#include "hpcalcs.h"

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


#include "logging.c"
#include "hpcables.c"
#include "hpcalcs.c"
#include "link_prime_hid.c"
#include "prime_rpkt.c"
#include "prime_vpkt.c"
#include "prime_cmd.c"
#include "calc_prime.c"


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
