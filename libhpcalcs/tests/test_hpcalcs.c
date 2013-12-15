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

#include <inttypes.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../src/hpfiles.h"
#include "../src/hpcables.h"
#include "../src/hpcalcs.h"
#include "../src/hpopers.h"
#include "../src/prime_cmd.h"

#undef VERSION
#define VERSION "Test program"

#ifdef _WIN32
static FILE * log_file = NULL;
#endif

static void output_log_callback(const char *format, va_list args) {
    // Windows' terminal is extremely slow, cannot print the traces there.
#ifndef _WIN32
    // Using args twice, once for printing to stdout and once for printing to a file, triggers crashes.
    // Therefore, let's have this function print only to stdout.
    // Should there be a need to print to a file, on non-Windows, people can rely on "tee".
    vprintf(format, args);
#else
    if (log_file == NULL) {
        log_file = fopen("trace.txt", "w+b");
        if (log_file != NULL) {
            time_t curtime = time(NULL);
            fprintf(log_file, "Opening log file at %s", ctime(&curtime));
        }
    }
    if (log_file != NULL) {
        vfprintf(log_file, format, args);
        fflush(log_file);
    }
#endif
}

// NOTE: this triplet of crude routines is just for demo and testing purposes !!
// In the general case, a proper i18n library (not offered by the C standard library) should be used !
static void crude_convert_UTF16LE_to_8bit(const char16_t * input, char * output) {
    const char * in_ptr = (const char *)input;
    char16_t chr = 0;
    do {
        chr = ((char16_t)*in_ptr) | (((char16_t)*(in_ptr + 1)) << 8);
        in_ptr += 2;
        if (chr & 0xFF00) {
            *output++ = '_'; // Mangle characters with high byte set...
        }
        else {
            *output++ = (char)(chr & 0xFF);
        }
    } while (chr != 0);
}

static void crude_convert_8bit_to_UTF16LE(const char * input, char16_t * output) {
    char chr = 0;
    do {
        chr = *input++;
        *((char *)output) = chr;
        *(((char *)output) + 1) = 0x00;
        output++;
    } while (chr != 0);
}

static void crude_convert_wchar_t_to_UTF16LE(const wchar_t * input, char16_t * output) {
    wchar_t chr = 0;
    do {
        chr = *input++;
        *output++ = (char16_t)chr;
    } while (chr != 0);
}

static void produce_output_file(calc_handle * handle, files_var_entry * entry) {
    char filename[FILES_VARNAME_MAXLEN + 13];
    FILE * f;
    const char * extension;

    printf("Receive file success\n");
    hpfiles_ve_display(entry);
    crude_convert_UTF16LE_to_8bit(entry->name, filename);
    if (entry->invalid) {
        printf("NOTE: the data for file %s is corrupted (packet loss in transfer) !\n", filename);
    }
    extension = hpfiles_vartype2fext(hpcalcs_get_model(handle), entry->type);
    if (extension[0] != 0) {
        strcat(filename, ".");
        strcat(filename, extension);
    }
    f = fopen(filename, "w+b");
    if (f != NULL) {
        fwrite(entry->data, 1, entry->size, f);
        fclose(f);
    }
    else {
        printf("Cannot open file for writing !\n");
    }
}

static int is_ready(calc_handle * handle) {
    int res;
    uint8_t * data;
    uint32_t size;

    res = hpcalcs_calc_check_ready(handle, &data, &size);
    if (res == 0) {
        printf("Check ready success\n");
        // TODO: do something with data & size.
    }
    else {
        printf("hpcalcs_calc_check_ready failed\n");
    }

    return res;
}

static int get_infos(calc_handle * handle) {
    int res;
    calc_infos infos;

    res = hpcalcs_calc_get_infos(handle, &infos);
    if (res == 0) {
        printf("Get infos success\n");
        // TODO: do something with infos.
    }
    else {
        printf("hpcalcs_calc_get_infos failed\n");
    }

    return res;
}

static int set_date_time(calc_handle * handle) {
    int res;

    res = hpcalcs_calc_set_date_time(handle, time(NULL));
    if (res == 0) {
        printf("Set date time success\n");
    }
    else {
        printf("hpcalcs_calc_set_date_time failed\n");
    }

    return res;
}

static int recv_screen(calc_handle * handle) {
    int res = 0;
    uint8_t * data;
    uint32_t size;
    unsigned int format;
    int err;

    printf("Choose a format (for Prime, usually 8 to 11):");

    err = scanf("%u", &format);
    if (err >= 1) {
        char filename[1024];

        filename[0] = 0;

        printf("\nEnter output filename: ");
        err = scanf("%1023s", filename);
        if (err >= 1) {
            puts("\n");
            res = hpcalcs_calc_recv_screen(handle, format, &data, &size);
            if (res == 0 && data != NULL) {
                FILE * f;
                printf("Receive screenshot success\n");
                f = fopen(filename, "w+b");
                if (f != NULL) {
                    fwrite(data, 1, size, f);
                    fclose(f);
                }
                else {
                    printf("Cannot open file for writing !\n");
                }
            }
            else {
                printf("hpcalcs_calc_recv_screen failed\n");
            }
        }
        else {
            fflush(stdin);
            printf("Canceled\n");
        }
    }
    else {
        fflush(stdin);
        printf("Canceled\n");
    }

    return res;
}

/*// On GCC 4.7+ and Clang, we can use the standard u"FOO" construct; otherwise, use L"x".
#if (defined(__GNUC__) && !defined(__clang__) && (__GNUC__ <= 4) && (__GNUC_MINOR__ < 7))
#define UTF16(x) L##x
#else
#define UTF16(x) u##x
#endif*/

#define xstr(s) str(s)
#define str(s) #s

static int send_file(calc_handle * handle) {
    int res = 0;
    int err;
    files_var_entry * entry = NULL;
    char filename[FILES_VARNAME_MAXLEN + 1];

    printf("\nEnter input filename: ");
    err = scanf("%" xstr(FILES_VARNAME_MAXLEN) "s", filename);
    if (err >= 1) {
        FILE * f = fopen(filename, "rb");
        if (f != NULL) {
            uint32_t size;
            uint8_t type;
            fseek(f, 0, SEEK_END);
            size = (uint32_t)ftell(f);
            fseek(f, 0, SEEK_SET);
            printf("Input file has size %" PRIu32 " (%" PRIx32 ")\n", size, size);
            entry = hpfiles_ve_create_with_size(size);
            if (entry != NULL) {
                char * calcfilename = NULL;
                if (!hpfiles_parsefilename(hpcalcs_get_model(handle), filename, &type, &calcfilename)) {
                    if (type != HPLIBS_FILE_TYPE_UNKNOWN && calcfilename != NULL) {
                        entry->type = type;
                        if (fread(entry->data, 1, size, f) == size) {
                            crude_convert_8bit_to_UTF16LE(calcfilename, entry->name);
                            // We can at last send the file !
                            res = hpcalcs_calc_send_file(handle, entry);
                            if (res == 0 && entry != NULL) {
                                printf("hpcalcs_calc_send_file succeeded\n");
                            }
                            else {
                                printf("hpcalcs_calc_send_file failed\n");
                            }
                        }
                        else {
                            printf("Reading input file failed, aborted\n");
                        }
                        free(calcfilename);
                    }
                    else {
                        printf("Unable to determine file type or calc filename, aborted (please report the bug !)\n");
                    }
                }
                else {
                    printf("Unable to parse filename, aborted (please report the bug !)\n");
                }
                hpfiles_ve_delete(entry);
            }
            else {
                printf("Cannot create entry, aborted\n");
            }
        }
        else {
            printf("Cannot open input file, aborted\n");
        }
    }
    else {
        fflush(stdin);
        printf("Canceled\n");
    }

    return res;
}

static int recv_file(calc_handle * handle) {
    int res = 0;
    int err;
    files_var_entry request;
    files_var_entry * entry;
    char typestr[11];
    wchar_t filename[FILES_VARNAME_MAXLEN + 1];

    memset((void *)&request, 0, sizeof(request));
    printf("\nEnter input filename (without computer-side extension): ");
    err = scanf("%" xstr(FILES_VARNAME_MAXLEN) "ls", filename);
    if (err >= 1) {
        printf("Enter file type:");

        err = scanf("%10s", typestr);
        if (err >= 1) {
            uint8_t type = hpfiles_str2vartype(hpcalcs_get_model(handle), typestr);
            if (type != HPLIBS_FILE_TYPE_UNKNOWN) {
                /*const char * fext = hpfiles_vartype2fext(hpcalcs_get_model(handle), type);
                if (fext != NULL && fext[0] != 0) {
                    wcscat(request.name, L".");
                    crude_convert_8bit_to_UTF16LE(fext, request.name + wcslen(request.name) + 1);
                }*/
                crude_convert_wchar_t_to_UTF16LE(filename, request.name);
                request.type = type;
                res = hpcalcs_calc_recv_file(handle, &request, &entry);
                printf("hpcalcs_calc_recv_file finished\n");
                if (res == 0 && entry != NULL) {
                    produce_output_file(handle, entry);
                    hpfiles_ve_delete(entry);
                }
                else {
                    printf("hpcalcs_calc_recv_file failed\n");
                }
            }
            else {
                printf("Unable to determine file type from stringe, aborted (please report the bug !)\n");
            }
        }
        else {
            fflush(stdin);
            printf("Canceled\n");
        }
    }
    else {
        fflush(stdin);
        printf("Canceled\n");
    }

    return res;
}

static int recv_backup(calc_handle * handle) {
    int res;
    files_var_entry ** entries;

    res = hpcalcs_calc_recv_backup(handle, &entries);
    if (res != 0) {
        printf("hpcalcs_calc_recv_backup failed\nWill nevertheless attempt to salvage data, if possible\n");
    }
    if (entries != NULL) {
        files_var_entry ** ptr = entries;
        while (*ptr != NULL) {
            produce_output_file(handle, *ptr++);
        }
        hpfiles_ve_delete_array(entries);
    }
    else {
        printf("hpcalcs_calc_recv_backup returned NULL entries, no data available\n");
    }

    return res;
}

static int send_key(calc_handle * handle) {
    int res = 0;
    int err;
    unsigned int type;

    printf("Enter key ID:");
    err = scanf("%u", &type);
    if (err >= 1) {
        res = hpcalcs_calc_send_key(handle, type);
        if (res == 0) {
            printf("hpcalcs_calc_send_key succeeded\n");
        }
        else {
            printf("hpcalcs_calc_send_key failed\n");
        }
    }
    else {
        printf("Canceled\n");
    }

    return res;
}

static int vpkt_send_experiments(calc_handle * handle) {
    int res = 0;
    int err;
    unsigned int id;

    printf("Enter ID:");
    err = scanf("%u", &id);
    if (err >= 1) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(2);
        if (pkt != NULL) {
            uint8_t * ptr;

            pkt->cmd = (uint8_t)id;
            ptr = pkt->data;
            *ptr++ = (uint8_t)id;
            res = prime_send_data(handle, pkt);

            if (res == 0) {
                printf("prime_send_data succeeded\n");
            }
            else {
                printf("prime_send_data failed\n");
            }

            prime_vtl_pkt_del(pkt);
        }
        else {
            printf("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        fflush(stdin);
        printf("Canceled\n");
    }

    return res;
}

#define NITEMS	10

static const char *str_menu[NITEMS] = {
    "Exit",
    "Check whether calc is ready",
    "Get calculator information",
    "Set date and time",
    "Get screenshot",
    "Send file",
    "Receive file",
    "Receive backup",
    "Send key",
    "Virtual packet send experiments"
};

typedef int (*FNCT_MENU) (calc_handle*);

static const FNCT_MENU fnct_menu[NITEMS] = {
    NULL,
    is_ready,
    get_infos,
    set_date_time,
    recv_screen,
    send_file,
    recv_file,
    recv_backup,
    send_key,
    vpkt_send_experiments
};

int main(int argc, char **argv) {
    cable_model model1 = CABLE_PRIME_HID;
    calc_model model2 = CALC_PRIME;
    cable_handle * cable;
    calc_handle * calc;
    int res;

    // init libs
    res = hpfiles_init(output_log_callback);
    if (res) {
        return 1;
    }
    res = hpcables_init(output_log_callback);
    if (res) {
        return 1;
    }
    res = hpcalcs_init(output_log_callback);
    if (res) {
        return 1;
    }
    res = hpopers_init(output_log_callback);
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
        unsigned int choice;

        // Display menu
        printf("Choose an action:\n");
        for(i = 0; i < NITEMS; i++) {
            printf("%2i. %s\n", i, str_menu[i]);
        }
        printf("Your choice: ");

        err = scanf("%u", &choice);
        if (err < 1) {
            fflush(stdin);
            continue;
        }
        printf("\n");

        if (choice == 0) {
            break;
        }

        // Process choice
        if (choice < (int)(sizeof(fnct_menu)/sizeof(fnct_menu[0])) && fnct_menu[choice]) {
            err = fnct_menu[choice](calc);
            if (err) {
                char * s;
                err = hplibs_error_get(err, &s);
                if (s != NULL) {
                    printf("%d %s", err, s);
                    free(s);
                }
                else {
                    printf("%d <unknown error>\n", err);
                }
            }
        }
        printf("\n");

    } while(1);

    // detach cable
    res = hpcalcs_cable_detach(calc);

    // remove calc & cable
del_calc:
    hpcalcs_handle_del(calc);
del_cable:
    hpcables_handle_del(cable);

final_teardown:
    printf("Exiting program\n");
    hpopers_exit();
    hpcalcs_exit();
    hpcables_exit();
    hpfiles_exit();

    printf("Goodbye world!\n");
    return 0;
}
