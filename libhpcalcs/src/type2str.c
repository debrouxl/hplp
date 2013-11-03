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
 * \file type2str.c Files / cables / calcs: Type (usually enum) <-> string conversion functions.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <hpcalcs.h>
#include <hpcables.h>
#include <hpcalcs.h>

#include "logging.h"
#include "error.h"

#include <string.h>
#include <strings.h>

HPEXPORT const char * HPCALL hpcalcs_model_to_string(calc_model model) {
    return hpfiles_model_to_string(model);
}

HPEXPORT calc_model HPCALL hpcalcs_string_to_model(const char *str) {
    return hpfiles_string_to_model(str);
}

HPEXPORT const char * HPCALL hpcables_model_to_string(calc_model model) {
    return hpfiles_model_to_string(model);
}

HPEXPORT calc_model HPCALL hpcables_string_to_model(const char *str) {
    return hpfiles_string_to_model(str);
}

HPEXPORT const char * HPCALL hpfiles_model_to_string(calc_model model) {
    switch (model) {
        case CALC_NONE: return "<none>";
        case CALC_PRIME: return "Prime";
        default: return "unknown";
    }
}

HPEXPORT calc_model HPCALL hpfiles_string_to_model(const char *str) {
    if (str != NULL) {
        if (!strcasecmp("Prime", str) || !strcasecmp("HP Prime", str)) {
            return CALC_PRIME;
        }
        // else fall through.
    }
    return CALC_NONE;
}
