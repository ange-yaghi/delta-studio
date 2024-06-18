/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2008-2011 Cisco Systems
 */

#include "safeclib_private.h"
#include "safe_mem_constraint.h"
#include "mem_primitives_lib.h"
#include "safe_mem_lib.h"


/**
 * NAME
 *    memzero_s
 *
 * SYNOPSIS
 *    #include "safe_mem_lib.h"
 *    errno_t
 *    memzero_s(void *dest, rsize_t len)
 *
 * DESCRIPTION
 *    Zeros len bytes starting at dest.
 *
 * EXTENSION TO
 *    ISO/IEC JTC1 SC22 WG14 N1172, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest         pointer to memory to be zeroed.
 *
 *    len          number of bytes to be zeroed
 *
 * OUTPUT PARAMETERS
 *    dest      is updated
 *
 * RUNTIME CONSTRAINTS
 *    dest shall not be a null pointer.
 *    len shall not be 0 nor greater than RSIZE_MAX_MEM.
 *    If there is a runtime constraint, the operation is not performed.
 *
 * RETURN VALUE
 *    EOK        successful operation
 *    ESNULLP    NULL pointer
 *    ESZEROL    zero length
 *    ESLEMAX    length exceeds max limit
 *
 * ALSO SEE
 *    memzero16_s(), memzero32_s()
 *
 */
errno_t
memzero_s (void *dest, rsize_t len)
{
    if (dest == NULL) {
        invoke_safe_mem_constraint_handler("memzero_s: dest is null",
                   NULL, ESNULLP);
        return (RCNEGATE(ESNULLP));
    }

    if (len == 0) {
        invoke_safe_mem_constraint_handler("memzero_s: len is 0",
                   NULL, ESZEROL);
        return (RCNEGATE(ESZEROL));
    }

    if (len > RSIZE_MAX_MEM) {
        invoke_safe_mem_constraint_handler("memzero_s: len exceeds max",
                   NULL, ESLEMAX);
        return (RCNEGATE(ESLEMAX));
    }

    /*
     * mem_prim_set(dest, len, 0xA5);
     * mem_prim_set(dest, len, 0x5A);
     */
    mem_prim_set(dest, len, 0);

    return (RCNEGATE(EOK));
}
EXPORT_SYMBOL(memzero_s)
