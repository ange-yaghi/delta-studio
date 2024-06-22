/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2012 Jonathan Toppins <jtoppins@users.sourceforge.net>
 *  Copyright (c) 2012 Cisco Systems
 *  Copyright (c) 2018-2022 Intel Corp
 *  All rights reserved.
 */

#include "safeclib_private.h"

/**
 * NAME
 *    abort_handler_s
 *
 * SYNOPSIS
 *    #include "safe_lib.h"
 *    void abort_handler_s(const char *msg, void *ptr, errno_t error)
 *
 * DESCRIPTION
 *    This function writes a message on the standard error stream in
 *    an implementation-defined format. The message shall include the
 *    string pointed to by msg. The abort_handler_s function then calls
 *    the abort function.
 *
 * SPECIFIED IN
 *    ISO/IEC JTC1 SC22 WG14 N1172, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    msg       Pointer to the message describing the error
 *
 *    ptr       Pointer to aassociated data.  Can be NULL.
 *
 *    error     The error code encountered.
 *
 * RETURN VALUE
 *    Does not return to caller.
 *
 * ALSO SEE
 *    ignore_handler_s()
 *
 */

void abort_handler_s(const char *msg, void *ptr, errno_t error)
{
    UNUSED(ptr);

    slprintf("ABORT CONSTRAINT HANDLER: (%u) %s\n", error,
         (msg) ? msg : "Null message");
    slabort();
}
EXPORT_SYMBOL(abort_handler_s)
