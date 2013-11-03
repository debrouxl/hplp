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
 * \file prime_cmd.h Calcs: Prime commands
 */

#ifndef __HPLIBS_CALCS_PRIME_CMD_H__
#define __HPLIBS_CALCS_PRIME_CMD_H__

#define CMD_PRIME_CHECK_READY (0xFF)
#define CMD_PRIME_GET_INFOS (0xFA)
#define CMD_PRIME_RECV_SCREEN (0xFC)
#define CMD_PRIME_RECV_BACKUP (0xF9)
#define CMD_PRIME_REQ_FILE (0xF8)
#define CMD_PRIME_RECV_FILE (0xF7)
#define CMD_PRIME_UNKNOWN (0xE7)

HPEXPORT int HPCALL calc_prime_s_check_ready(calc_handle * handle);
HPEXPORT int HPCALL calc_prime_r_check_ready(calc_handle * handle, uint8_t ** out_data, uint32_t * out_size);

HPEXPORT int HPCALL calc_prime_s_get_infos (calc_handle * handle);
HPEXPORT int HPCALL calc_prime_r_get_infos (calc_handle * handle, calc_infos * infos);

HPEXPORT int HPCALL calc_prime_s_recv_screen(calc_handle * handle, calc_screenshot_format format);
HPEXPORT int HPCALL calc_prime_r_recv_screen(calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size);

HPEXPORT int HPCALL calc_prime_s_send_file(calc_handle * handle, files_var_entry * file);
HPEXPORT int HPCALL calc_prime_r_send_file(calc_handle * handle);

HPEXPORT int HPCALL calc_prime_s_recv_file(calc_handle * handle, files_var_entry * file);
HPEXPORT int HPCALL calc_prime_r_recv_file(calc_handle * handle, files_var_entry ** out_file);

HPEXPORT int HPCALL calc_prime_s_recv_backup(calc_handle * handle);
HPEXPORT int HPCALL calc_prime_r_recv_backup(calc_handle * handle, files_var_entry *** out_vars);

#endif
