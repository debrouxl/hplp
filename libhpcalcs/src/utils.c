/*
 * libhpfiles, libhpcables, libhpcalcs - hand-helds support library
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
 * \file utils.c Files / Cables / Calcs: utility functions, e.g. UTF-16LE handling.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hpfiles.h>
#include "utils.h"
#include "logging.h"

#include <inttypes.h>
#include <stdlib.h>

uint32_t char16_strlen(char16_t * str) {
    uint32_t i = 0;
    if (str != NULL) {
        while (*str) {
            i++;
            str++;
        }
    }
    return i;
}

char16_t * char16_strncpy(char16_t * dst, const char16_t * src, uint32_t n) {
    if (dst != NULL && src != NULL) {
        uint32_t i = 0;
        char16_t c = *src;
        while (i < n && c != 0) {
            *dst++ = c;
            src++;
            c = *src;
        }
    }
    return dst;
}

void hexdump(const char * direction, uint8_t *data, uint32_t size, uint32_t level)
{
    if (size > 0) {
        if (level == 1) {
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
        }
        else if (level == 2) {
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
        }
    }
}
