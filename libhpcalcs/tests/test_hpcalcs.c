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
 * \file test_hpcalcs.c Calcs: Example / testing code, which can be considered authoritative.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../src/hpfiles.h"
#include "../src/hpcables.h"
#include "../src/hpcalcs.h"
#include "../src/prime_cmd.h"

#undef VERSION
#define VERSION "Test program"

static int is_ready(calc_handle* h) {
    int res;
    uint8_t * data;
    uint32_t size;

    res = hpcalcs_calc_check_ready(h, &data, &size);
    // TODO: do something with data & size.

    return res;
}

static int get_infos(calc_handle *h) {
    int res;
    calc_infos infos;

    res = hpcalcs_calc_get_infos(h, &infos);
    // TODO: do something with infos.

    return res;
}

static int recv_screen(calc_handle *h) {
    int res;
    uint8_t * data;
    uint32_t size;

    res = hpcalcs_calc_recv_screen(h, CALC_SCREENSHOT_FORMAT_PRIME_PNG, &data, &size);
    // TODO: do something with data & size.

    return res;
}

static int send_file(calc_handle *h) {
    int res;
    files_var_entry entry;

    res = hpcalcs_calc_send_file(h, &entry);

    return res;
}

static int recv_file(calc_handle *h) {
    int res;
    const char * name;
    files_var_entry * entry;

    name = "foo";
    res = hpcalcs_calc_recv_file(h, name, &entry);
    // TODO: do something with entry.

    return res;
}

static int recv_backup(calc_handle *h) {
    int res;
    files_var_entry ** entries;

    res = hpcalcs_calc_recv_backup(h, &entries);
    // TODO: do something with entries.

    return res;
}

#define NITEMS	7

static const char *str_menu[NITEMS] = 
{
    "Exit",
    "Check whether calc is ready",
    "Get calculator information",
    "Get screenshot",
    "Send file",
    "Receive file",
    "Receive backup"
};

typedef int (*FNCT_MENU) (calc_handle*);

static const FNCT_MENU fnct_menu[NITEMS] = 
{
    NULL,
    is_ready,
    get_infos,
    recv_screen,
    send_file,
    recv_file,
    recv_backup
};

int main(int argc, char **argv)
{
    cable_model model1 = CABLE_PRIME_HID;
    calc_model model2 = CALC_PRIME;
    cable_handle * cable;
    calc_handle * calc;
    int res;
    int do_exit=0;
    unsigned int choice;

    // init libs
    res = hpfiles_init();
    if (res) {
        return 1;
    }
    res = hpcables_init();
    if (res) {
        return 1;
    }
    res = hpcalcs_init();
    if (res) {
        return 1;
    }

    cable = hpcables_handle_new(model1);
    if (cable == NULL) {
        fprintf(stderr, "hpcables_handle_new failed\n");
        goto final_teardown;
    }

    calc = hpcalcs_handle_new(model2);
    if (calc == NULL) {
        fprintf(stderr, "hpcalcs_handle_new failed\n");
        goto del_cable;
    }

    // attach cable to calc (and open cable)
    res = hpcalcs_cable_attach(calc, cable);
    if (res) {
        goto del_calc;
    }

    do
    {
        int i;
        int err;
restart:
        // Display menu
        printf("Choose an action:\n");
        for(i = 0; i < NITEMS; i++) {
            printf("%2i. %s\n", i, str_menu[i]);
        }
        printf("Your choice: ");

        err = scanf("%u", &choice);
        if (err < 1) {
            goto restart;
        }
        printf("\n");

        if (choice == 0) {
            do_exit = 1;
        }

        // Process choice
        if (choice < (int)(sizeof(fnct_menu)/sizeof(fnct_menu[0])) && fnct_menu[choice]) {
            fnct_menu[choice](calc);
        }
        printf("\n");

    } while(!do_exit);

    // detach cable
    res = hpcalcs_cable_detach(calc);

    // remove calc & cable
del_calc:
    hpcalcs_handle_del(calc);
del_cable:
    hpcables_handle_del(cable);

final_teardown:
    printf("Exiting program\n");
    hpcalcs_exit();
    hpcables_exit();
    hpfiles_exit();

    printf("Goodbye world!\n");
    return 0;
}
