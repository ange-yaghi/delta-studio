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
 *    memset32_s - Sets a block of memory to value
 *
 * SYNOPSIS
 *    #include "safe_mem_lib.h"
 *    errno_t
 *    memset32_s(uint32_t *dest, rsize_t len, uint32_t value)
 *
 * DESCRIPTION
 *    Sets len uint32_t starting at dest to the specified value.
 *
 * EXTENSION TO
 *    ISO/IEC JTC1 SC22 WG14 N1172, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest       pointer to memory that will be set to the value
 *
 *    len        number of uint32_t to be set
 *
 *    value      uint32_t value to be written
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
 *    memset_s(), memset16_s()
 *
 */
errno_t
memset32_s (uint32_t *dest, rsize_t len, uint32_t value)
{
    if (dest == NULL) {
        invoke_safe_mem_constraint_handler("memset32_s: dest is null",
                   NULL, ESNULLP);
        return (RCNEGATE(ESNULLP));
    }

    if (len == 0) {
        invoke_safe_mem_constraint_handler("memset32_s: len is 0",
                   NULL, ESZEROL);
        return (RCNEGATE(ESZEROL));
    }

    if (len > RSIZE_MAX_MEM32) {
        invoke_safe_mem_constraint_handler("memset32_s: len  exceeds max",
                   NULL, ESLEMAX);
        return (RCNEGATE(ESLEMAX));
    }

    mem_prim_set32(dest, len, value);

    return (RCNEGATE(EOK));
}
EXPORT_SYMBOL(memset32_s)
