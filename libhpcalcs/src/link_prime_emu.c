/*
 * libhpcables: hand-helds support libraries.
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
 * \file link_prime_emu.c Cables: Prime Emu cable.
 */

#ifdef _WIN32
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <inttypes.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <hplibs.h>
#include <hpcalcs.h>
#include "logging.h"
#include "error.h"
#include "utils.h"

extern const cable_fncts cable_prime_emu_fncts;
extern const cable_fncts cable_39gII_emu_fncts;

#define HP_39gII_EMU_PIPE_NAME      L"\\\\.\\pipe\\hp88"
#define HP_PRIME_EMU_PIPE_NAME      L"\\\\.\\pipe\\hp89"

// FIXME remove these variables, when the code for closing the virtual cables works.
static HANDLE handle_39gII_pipe = NULL;
static HANDLE handle_prime_pipe = NULL;

static int cable_prime39gII_emu_read_block(cable_handle * handle, uint8_t * data, uint32_t * len) {
    int res;
    DWORD bytes_read = 0;
    DWORD error;
    BOOL read_result;
    HANDLE pipe = (HANDLE)(handle->handle);
    uint8_t * tmp;

    hpcables_info("%s: entering", __FUNCTION__);

    if (PeekNamedPipe(pipe, NULL, 0, NULL, &bytes_read, NULL)) {
        OVERLAPPED overlapped;

        hpcables_info("%s: peeking pipe succeeded", __FUNCTION__);

        ZeroMemory(&overlapped, sizeof(overlapped));
        overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (overlapped.hEvent != NULL) {
            hpcables_info("%s: created event", __FUNCTION__);

            read_result = ReadFile(pipe, data, *len, &bytes_read, &overlapped);
            error = GetLastError();

            if (!read_result) {
                // Failure, or asynchronous operation.
                hpcables_info("%s: ReadFile returned 0", __FUNCTION__);
                if (error == ERROR_IO_PENDING) {
                    hpcables_info("%s: ERROR_IO_PENDING", __FUNCTION__);
                    read_result = GetOverlappedResult(pipe, &overlapped, &bytes_read, TRUE);
                    if (read_result != 0) {
                        hpcables_error("%s: nonzero GetOverlappedResult, last error is 0x%08lx", __FUNCTION__, GetLastError());
                        *len = (uint32_t)bytes_read;
                        if (bytes_read != 0) {
                            hexdump("IN", data, bytes_read, 2);
                            res = ERR_SUCCESS;
                            hpcables_error("%s: read %d bytes asynchronously, last error is 0x%08lx", __FUNCTION__, bytes_read, GetLastError());
                        }
                        else {
                            res = ERR_SUCCESS; // ??
                            hpcables_error("%s: read no bytes asynchronously, last error is 0x%08lx", __FUNCTION__, GetLastError());
                        }
                    }
                    else {
                        res = ERR_CABLE_READ_ERROR;
                        hpcables_error("%s: zero GetOverlappedResult, last error is 0x%08lx", __FUNCTION__, GetLastError());
                    }
                }
                else {
                    CancelIo(pipe);
                    res = ERR_CABLE_READ_ERROR;
                    hpcables_info("%s: not ERROR_IO_PENDING, last error is 0x%08lx", __FUNCTION__, error);
                }
            }
            else {
                // Success.
                if (error == 0) {
                    // ReadFile completed synchronously.
                    *len = (uint32_t)bytes_read;
                        if (bytes_read != 0) {
                            hexdump("IN", data, bytes_read, 2);
                            res = ERR_SUCCESS;
                            hpcables_error("%s: read %d bytes synchronously, last error is 0x%08lx", __FUNCTION__, bytes_read, GetLastError());
                        }
                        else {
                            res = ERR_SUCCESS; // ??
                            hpcables_error("%s: read no bytes synchronously, last error is 0x%08lx", __FUNCTION__, GetLastError());
                        }
                }
                else {
                    res = ERR_CABLE_READ_ERROR;
                    hpcables_info("%s: ReadFile returned %d, bytes_read %d, last error is 0x%08lx", __FUNCTION__, read_result, bytes_read, GetLastError());
                }
            }
            CloseHandle(overlapped.hEvent);
        }
        else {
            res = ERR_CABLE_READ_ERROR;
            hpcables_error("%s: failed to create event, last error is 0x%08lx", __FUNCTION__,  GetLastError());
        }
    }
    else {
        res = ERR_CABLE_READ_ERROR;
        hpcables_info("%s: failed to peek pipe, last error is 0x%08lx", __FUNCTION__,  GetLastError());
    }

    hpcables_info("%s: exiting %d", __FUNCTION__, res);
    return res;
}

static int cable_prime39gII_emu_read_blocks(cable_handle * handle, uint8_t ** data, uint32_t * len) {
    int res = ERR_CABLE_READ_ERROR;
    DWORD bytes_read = 0;
    DWORD total_bytes_read = 0;
    DWORD error;
    BOOL finished = FALSE;
    BOOL read_result;
    OVERLAPPED overlapped;
    HANDLE pipe = (HANDLE)(handle->handle);
    uint8_t * tmp;
    uint32_t orig_len = *len;

    hpcables_info("%s: entering", __FUNCTION__);

    do {
        res = cable_prime39gII_emu_read_block(handle, *data + total_bytes_read, len);
        if (res == ERR_SUCCESS) {
            bytes_read = *len;
            if (bytes_read != 0) {
                hpcables_info("%s: read %d bytes", __FUNCTION__, bytes_read);
                total_bytes_read += bytes_read;
                if (bytes_read < orig_len) {
                    hpcables_info("%s: short read, will shorten data buffer", __FUNCTION__);
                    tmp = realloc(*data, total_bytes_read);
                    if (tmp == NULL) {
                        free(*data);
                        *len = 0;
                        *data = tmp;
                        res = ERR_MALLOC;
                        hpcables_error("%s: failed to shrink buffer, bailing out", __FUNCTION__);
                        break;
                    }
                    else {
                        *len = total_bytes_read;
                        *data = tmp;
                        // res is already ERR_SUCCESS.
                        hpcables_error("%s: successfully shrank buffer, bailing out", __FUNCTION__);
                        break;
                    }
                }
                else {
                    hpcables_info("%s: full read, will grow data buffer", __FUNCTION__);
                    tmp = realloc(*data, total_bytes_read + orig_len);
                    if (tmp != NULL) {
                        // Clear newly allocated part of buffer.
                        memset(tmp + total_bytes_read, 0, orig_len);
                        *data = tmp;
                        hpcables_error("%s: successfully grown buffer", __FUNCTION__);
                    }
                    else {
                        res = ERR_MALLOC;
                        hpcables_error("%s: failed to grow buffer, bailing out, last error is 0x%08lx", __FUNCTION__, GetLastError());
                    }
                }
                *len = orig_len;
            }
            else {
                *len = total_bytes_read;
                // res is already ERR_SUCCESS.
                hpcables_error("%s: read no bytes", __FUNCTION__);
                break; // Bail out now.
            }
        }
        else {
            hpcables_error("%s: res is nonzero, bailing out", __FUNCTION__);
        }
    } while (!res);
    
    hpcables_info("%s: exiting", __FUNCTION__);
    return res;
}

// FIXME this doesn't work.
static int cable_prime39gII_emu_flush_pipe(cable_handle * handle) {
    int res;
    uint32_t size = 128;
    uint8_t * buf;

    hpcables_info("%s: entering", __FUNCTION__);

    buf = malloc(size);
    if (buf != NULL) {
        do {
            hpcables_info("%s: loop", __FUNCTION__);
            res = cable_prime39gII_emu_read_block(handle, buf, &size);
            hpcables_info("%s: size = %d", __FUNCTION__, size);
            if (size < 128) {
                break;
            }
        } while (!res);
    }
    else {
        res = ERR_MALLOC;
        hpcables_error("%s: failed to allocate buffer for flushing the pipe", __FUNCTION__);
    }
    return res;
}

static void build_handle(cable_handle * handle, cable_model model, HANDLE pipe, const cable_fncts * fncts) {
    handle->model = model;
    handle->handle = (void *)pipe;
    handle->fncts = fncts;
    handle->read_timeout = 1000;
    handle->open = 1;
    handle->busy = 0;
}

static int cable_prime39gII_emu_open(cable_handle * handle, const wchar_t * pipe_name, cable_model model, const cable_fncts * fncts) {
    int res;

    hpcables_info("%s: entering", __FUNCTION__);

    if (handle != NULL) {
        DWORD error = ERROR_SUCCESS;
        HANDLE pipe = CreateFileW(pipe_name,
                                  GENERIC_READ | GENERIC_WRITE,
                                  0,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_FLAG_OVERLAPPED,
                                  NULL);

        if (pipe != INVALID_HANDLE_VALUE) {
            hpcables_info("%s: connected to named pipe as client\n", __FUNCTION__);
success:
            build_handle(handle, model, pipe, fncts);
            res = ERR_SUCCESS;
            hpcables_info("%s: cable open succeeded", __FUNCTION__);
        }
        else {
            error = GetLastError();

            // Unless the pipe is busy, there was a hard error.
            if (ERROR_PIPE_BUSY != error) {
                hpcables_error("%s: opening named pipe failed, last error is 0x%08lx\n", __FUNCTION__, error);
            }
            else {
                // Wait a bit for the pipe.
                if (!WaitNamedPipeW(pipe_name, 500)) {
                    hpcables_error("%s: waiting for named pipe failed, last error is 0x%08lx\n", __FUNCTION__, GetLastError());
                    CloseHandle(pipe);
                }
                else {
                    goto success;
                }
            }

            res = ERR_CABLE_NOT_OPEN;
            hpcables_error("%s: cable open failed", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

static int cable_39gII_emu_probe(cable_handle * handle) {
    // FIXME this should be a simple forward to cable_prime39gII_emu_open, when the code for closing the virtual cables works.
    int res;
    if (handle_39gII_pipe != NULL) {
        build_handle(handle, CABLE_39gII_EMU, handle_39gII_pipe, &cable_39gII_emu_fncts);
        res = ERR_SUCCESS;
    }
    else {
        res = cable_prime39gII_emu_open(handle, HP_39gII_EMU_PIPE_NAME, CABLE_39gII_EMU, &cable_39gII_emu_fncts);
        if (res == ERR_SUCCESS) {
            handle_39gII_pipe = handle->handle;
        }
    }
    return res;
}

static int cable_prime_emu_probe(cable_handle * handle) {
    // FIXME this should be a simple forward to cable_prime39gII_emu_open, when the code for closing the virtual cables works.
    int res;
    if (handle_prime_pipe != NULL) {
        build_handle(handle, CABLE_PRIME_EMU, handle_prime_pipe, &cable_prime_emu_fncts);
        res = ERR_SUCCESS;
    }
    else {
        res = cable_prime39gII_emu_open(handle, HP_PRIME_EMU_PIPE_NAME, CABLE_PRIME_EMU, &cable_prime_emu_fncts);
        if (res == ERR_SUCCESS) {
            handle_prime_pipe = handle->handle;
        }
    }
    return res;
}

static int cable_39gII_emu_open(cable_handle * handle) {
    return cable_39gII_emu_probe(handle);
}

static int cable_prime_emu_open(cable_handle * handle) {
    return cable_prime_emu_probe(handle);
}

static int cable_prime39gII_emu_close(cable_handle * handle) {
    int res;

    hpcables_info("%s: entering", __FUNCTION__);

    if (handle != NULL) {
        HANDLE pipe = (HANDLE)(handle->handle);
        if (pipe != NULL) {
            if (handle->open) {
                // FIXME this doesn't work.
                //res = cable_prime39gII_emu_flush_pipe(handle);
                CloseHandle(pipe);
                handle->model = CABLE_NUL;
                handle->handle = NULL;
                handle->fncts = NULL;
                handle->open = 0;
                hpcables_info("%s: cable closed", __FUNCTION__);
            }
            else {
                res = ERR_CABLE_NOT_OPEN;
                hpcables_error("%s: cable was not open", __FUNCTION__);
            }
        }
        else {
            res = ERR_INVALID_HANDLE;
            hpcables_error("%s: pipe is NULL", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

static int cable_prime39gII_emu_set_read_timeout(cable_handle * handle, int read_timeout) {
    int res;
    if (handle != NULL) {
        handle->read_timeout = read_timeout;
    }
    else {
        res = ERR_INVALID_HANDLE;
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}


static int cable_prime39gII_emu_send(cable_handle * handle, uint8_t * data, uint32_t len) {
    int res;
    if (handle != NULL && data != NULL) {
        HANDLE pipe = (HANDLE)(handle->handle);
        if (pipe != NULL) {
            if (handle->open) {
                DWORD written;

                hexdump("OUT", data, len, 2);
                // Wine traces show that the HP CK uses asynchronous writes, but synchronous writes are simpler and seem to work ?
                if (WriteFile(pipe, data, len, &written, NULL)) {
                    res = ERR_SUCCESS;
                    hpcables_info("%s: wrote %u bytes", __FUNCTION__, written);
                }
                else {
                    res = ERR_CABLE_WRITE_ERROR;
                    hpcables_error("%s: writing to named pipe failed, last error is 0x%08lx\n", __FUNCTION__, GetLastError());
                }
            }
            else {
                res = ERR_CABLE_NOT_OPEN;
                hpcables_error("%s: cable was not open", __FUNCTION__);
            }
        }
        else {
            res = ERR_INVALID_HANDLE;
            hpcables_error("%s: pipe is NULL", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcables_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

static int cable_prime39gII_emu_recv(cable_handle * handle, uint8_t ** data, uint32_t * len) {
    int res;
    // Use pre-allocated area pointed to by data, with size pointed to by len.
    if (handle != NULL && data != NULL && *data != NULL && len != NULL) {
        if (handle->handle != NULL) {
            hpcables_info("%s: non-NULL pipe", __FUNCTION__);
            if (handle->open) {
                res = cable_prime39gII_emu_read_blocks(handle, data, len);
            }
            else {
                res = ERR_CABLE_NOT_OPEN;
                hpcables_error("%s: cable was not open", __FUNCTION__);
            }
        }
        else {
            res = ERR_INVALID_HANDLE;
            hpcables_error("%s: device_handle is NULL", __FUNCTION__);
        }
    }
    else {
        res = ERR_INVALID_PARAMETER;
        hpcables_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

const cable_fncts cable_39gII_emu_fncts =
{
    CABLE_39gII_EMU,
    "39gII Emu cable",
    "39gII Emu cable",
    &cable_39gII_emu_probe,
    &cable_39gII_emu_open,
    &cable_prime39gII_emu_close,
    &cable_prime39gII_emu_set_read_timeout,
    &cable_prime39gII_emu_send,
    &cable_prime39gII_emu_recv
};

const cable_fncts cable_prime_emu_fncts =
{
    CABLE_PRIME_EMU,
    "Prime Emu cable",
    "Prime Emu cable",
    &cable_prime_emu_probe,
    &cable_prime_emu_open,
    &cable_prime39gII_emu_close,
    &cable_prime39gII_emu_set_read_timeout,
    &cable_prime39gII_emu_send,
    &cable_prime39gII_emu_recv
};

#endif
