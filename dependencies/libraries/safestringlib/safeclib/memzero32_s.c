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
 *    memzero32_s
 *
 * SYNOPSIS
 *    #include "safe_mem_lib.h"
 *    errno_t
 *    memzero32_s(uint32_t *dest, rsize_t len)
 *
 * DESCRIPTION
 *    Zeros len uint32_ts starting at dest.
 *
 * EXTENSION TO
 *    ISO/IEC JTC1 SC22 WG14 N1172, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest         pointer to memory to be zeroed.
 *
 *    len          number of uint32_ts to be zeroed
 *
 * OUTPUT PARAMETERS
 *    dest      is updated
 *
 * RUNTIME CONSTRAINTS
 *    dest shall not be a null pointer.
 *    len shall not be 0 nor greater than RSIZE_MAX_MEM32.
 *    If there is a runtime constraint, the operation is not performed.
 *
 * RETURN VALUE
 *    EOK        successful operation
 *    ESNULLP    NULL pointer
 *    ESZEROL    zero length
 *    ESLEMAX    length exceeds max limit
 *
 * ALSO SEE
 *    memzero_s(), memzero16_s()
 *
 */
errno_t
memzero32_s (uint32_t *dest, rsize_t len)
{

    if (dest == NULL) {
        invoke_safe_mem_constraint_handler("memzero32_s: dest is null",
                   NULL, ESNULLP);
        return (RCNEGATE(ESNULLP));
    }

    if (len == 0) {
        invoke_safe_mem_constraint_handler("memzero32_s: len is 0",
                   NULL, ESZEROL);
        return (RCNEGATE(ESZEROL));
    }

    if (len > RSIZE_MAX_MEM32) {
        invoke_safe_mem_constraint_handler("memzero32_s: len exceeds max",
                   NULL, ESLEMAX);
        return (RCNEGATE(ESLEMAX));
    }

    /*
     * mem_prim_set32(dest, len, 0xDEADBEEF);
     * mem_prim_set32(dest, len, 0xBA5EBA11);
     */
    mem_prim_set32(dest, len, 0);

    return (RCNEGATE(EOK));
}
EXPORT_SYMBOL(memzero32_s)
